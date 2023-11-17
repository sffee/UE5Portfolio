#include "BOTWGA_LockOn.h"

#include "Character/Enemy/EnemyCharacter.h"
#include "Character/Player/PlayerCharacter.h"
#include "Abilities/AbilityTask/AT_CheckLockOnDistance.h"

UBOTWGA_LockOn::UBOTWGA_LockOn()
	: Radius(800.f)
	, LimitDistance(800.f)
	, LimitAngle(90.f)
	, IsDrawDebug(false)
	, DrawDebugDuration(1.f)
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;

	UGameplayTagsManager::Get().CallOrRegister_OnDoneAddingNativeTagsDelegate(FSimpleDelegate::CreateLambda([&]
	{
		AbilityInputTag = GAMEPLAYTAG.InputTag_LockOn;
		AbilityTags.AddTag(GAMEPLAYTAG.Character_Special_LockOn);
		ActivationOwnedTags.AddTag(GAMEPLAYTAG.Character_Special_LockOn);
	}));
}

void UBOTWGA_LockOn::OnAvatarSet(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec)
{
	Super::OnAvatarSet(ActorInfo, Spec);

	Player = Cast<APlayerCharacter>(ActorInfo->AvatarActor.Get());
	AbilitySystemComponent = GetBOTWAbilitySystemComponent();
}

void UBOTWGA_LockOn::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	if (IsDrawDebug)
	{
		DrawDebugSphere(GetWorld(), Player->GetActorLocation(), Radius, 20, FColor::Red, false, DrawDebugDuration);
	}

	CheckLockOnDistance = UAT_CheckLockOnDistance::CheckLockOnDistance(this, NearestEnemyCharacter, LimitDistance);
	if (CheckLockOnDistance)
	{
		CheckLockOnDistance->OnCancel.BindUObject(this, &UBOTWGA_LockOn::OnCancel);

		CheckLockOnDistance->ReadyForActivation();
	}

	GameplayEventTagDelegateHandle = AbilitySystemComponent->AddGameplayEventTagContainerDelegate(FGameplayTagContainer(GAMEPLAYTAG.Event_Weapon_Attack_RushEnd),
		FGameplayEventTagMulticastDelegate::FDelegate::CreateUObject(this, &UBOTWGA_LockOn::OnGameplayEvent));

	Player->LockOn(NearestEnemyCharacter);
}

bool UBOTWGA_LockOn::CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags /*= nullptr*/, const FGameplayTagContainer* TargetTags /*= nullptr*/, OUT FGameplayTagContainer* OptionalRelevantTags /*= nullptr*/) const
{
	if (Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags) == false)
	{
		return false;
	}

	if (Player == nullptr || AbilitySystemComponent == nullptr)
	{
		return false;
	}

	TArray<FOverlapResult> overlaps;
	if (SphereOverlap(overlaps) == false)
	{
		return false;
	}

	NearestEnemyCharacter = GetNearestCharacterInCameraCenter(overlaps);

	return true;
}

void UBOTWGA_LockOn::InputReleased(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo)
{
	if (AbilitySystemComponent->HasMatchingGameplayTag(GAMEPLAYTAG.Character_Special_Rush) == false)
	{
		CancelAbility(Handle, ActorInfo, ActivationInfo, false);
	}
}

void UBOTWGA_LockOn::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	if (Player)
	{
		Player->LockOff();
	}

	if (GameplayEventTagDelegateHandle.IsValid())
	{
		AbilitySystemComponent->RemoveGameplayEventTagContainerDelegate(FGameplayTagContainer(GAMEPLAYTAG.Event_Weapon_Attack_RushEnd), GameplayEventTagDelegateHandle);
	}

	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

bool UBOTWGA_LockOn::SphereOverlap(OUT TArray<FOverlapResult>& _Overlaps) const
{
	UWorld* world = Player->GetWorld();
	if (world == nullptr)
	{
		return false;
	}

	FCollisionShape collisionShape = FCollisionShape::MakeSphere(Radius);
	FCollisionObjectQueryParams objectQueryParams(ECC_Pawn);
	FCollisionQueryParams queryParams;
	queryParams.AddIgnoredActor(Player);

	world->OverlapMultiByObjectType(_Overlaps, Player->GetActorLocation(), FQuat::Identity, objectQueryParams, collisionShape, queryParams);

	if (_Overlaps.IsEmpty())
	{
		return false;
	}

	return true;
}

AEnemyCharacter* UBOTWGA_LockOn::GetNearestCharacterInCameraCenter(const TArray<FOverlapResult>& _Overlaps) const
{
	FVector yawRotation = FRotator(0.f, Player->GetControlRotation().Yaw, 0.f).Vector();

	TArray<AActor*> actors;
	for (const FOverlapResult& overlap : _Overlaps)
	{
		actors.AddUnique(overlap.GetActor());
	}

	AEnemyCharacter* nearEnemyCharacter = nullptr;
	float minAngle = 360.f;
	for (AActor* actor : actors)
	{
		AEnemyCharacter* enemyCharacter = Cast<AEnemyCharacter>(actor);
		if (enemyCharacter == nullptr)
		{
			continue;
		}

		float distance = FVector::Distance(Player->GetActorLocation(), enemyCharacter->GetActorLocation());
		if (LimitDistance < distance)
		{
			continue;
		}

		FVector dirVector = (enemyCharacter->GetActorLocation() - Player->GetActorLocation()).GetSafeNormal2D();
		float dot = FVector::DotProduct(dirVector, yawRotation);
		float dirAngle = FMath::RadiansToDegrees(FMath::Acos(dot));
		if (LimitAngle < dirAngle)
		{
			continue;
		}

		if (dirAngle < minAngle)
		{
			nearEnemyCharacter = enemyCharacter;
			minAngle = dirAngle;
		}
	}

	return nearEnemyCharacter;
}

void UBOTWGA_LockOn::OnCancel()
{
	CancelAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, false);
}

void UBOTWGA_LockOn::OnGameplayEvent(FGameplayTag _EventTag, const FGameplayEventData* _Payload)
{
	FGameplayAbilitySpec* spec = GetCurrentAbilitySpec();
	if (spec && spec->InputPressed == false)
	{
		ExternalEndAbility();
	}
}