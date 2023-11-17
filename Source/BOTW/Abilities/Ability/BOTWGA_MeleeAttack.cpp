#include "BOTWGA_MeleeAttack.h"

#include "Abilities/BOTWAbilitySystemComponent.h"
#include "Character/Player/Components/PlayerCombatComponent.h"
#include "MotionWarpingComponent.h"
#include "Components/CapsuleComponent.h"
#include "Abilities/AbilityTask/AT_PlayMontageAndWaitForEvent.h"
#include "Abilities/AbilityTask/AT_TraceMeleeAttack.h"
#include "Character/CharacterBase.h"
#include "Character/Enemy/EnemyCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "NiagaraFunctionLibrary.h"
#include "Weapon/MeleeWeaponInstance.h"
#include "Equipment/EquipmentActor.h"
#include "Interface/CombatInterface.h"
#include "Library/LogLibrary.h"

#include "Character/Enemy/EnemyCharacter.h"
#include "Character/Enemy/Controller/EnemyController.h"
#include "BehaviorTree/BlackboardComponent.h"

UBOTWGA_MeleeAttack::UBOTWGA_MeleeAttack()
	: TraceSphereRadius(10.f)
	, DrawDebugDuration(0.f)
	, AttackIndex(0)
	, IsAttackable(false)
	, IsApplyTimeDilation(true)
	, IsFirstRushAttack(false)
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;

	UGameplayTagsManager::Get().CallOrRegister_OnDoneAddingNativeTagsDelegate(FSimpleDelegate::CreateLambda([&]
	{
		AbilityInputTag = GAMEPLAYTAG.InputTag_Attack;
		AbilityTags.AddTag(GAMEPLAYTAG.Character_State_MeleeAttack);
		ActivationOwnedTags.AddTag(GAMEPLAYTAG.Character_State_MeleeAttack);

		FAbilityTriggerData AbilityTriggerData;
		AbilityTriggerData.TriggerSource = EGameplayAbilityTriggerSource::Type::GameplayEvent;
		AbilityTriggerData.TriggerTag = GAMEPLAYTAG.InputTag_Attack;
		AbilityTriggers.Add(AbilityTriggerData);
	}));
}

void UBOTWGA_MeleeAttack::OnAvatarSet(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec)
{
	Super::OnAvatarSet(ActorInfo, Spec);

	MeleeWeaponInstance = Cast<UMeleeWeaponInstance>(GetSourceObject(CurrentSpecHandle, ActorInfo));
	MeleeWeaponActor = MeleeWeaponInstance ? Cast<AEquipmentActor>(MeleeWeaponInstance->GetSpawnedActor()) : nullptr;
	if (MeleeWeaponActor == nullptr)
	{
		PRINT_ERROR("MeleeWeapon Is Null");
		return;
	}

	AbilitySystemComponent = GetBOTWAbilitySystemComponent();
	if (AbilitySystemComponent == nullptr)
	{
		PRINT_ERROR("AbilitySystemComponent Is Null");
		return;
	}

	Character = Cast<ACharacterBase>(ActorInfo->AvatarActor.Get());
	if (Character == nullptr)
	{
		PRINT_ERROR("Character Is Null");
		return;
	}

	ICombatInterface* combatInterface = Cast<ICombatInterface>(Character->FindComponentByInterface<UCombatInterface>());
	MeleeAttackMontages = combatInterface ? combatInterface->GetWeaponAnimData(MeleeWeaponInstance->GetOverlayType()) : nullptr;
	if (MeleeAttackMontages == nullptr)
	{
		return;
	}

	PlayerCombatComponent = Character->FindComponentByClass<UPlayerCombatComponent>();
	MotionWarpingComponent = Character->FindComponentByClass<UMotionWarpingComponent>();
}

void UBOTWGA_MeleeAttack::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	TraceMeleeAttack = UAT_TraceMeleeAttack::TraceMeleeAttack(this, MeleeWeaponActor, TraceSphereRadius, DrawDebugDuration);
	if (TraceMeleeAttack)
	{
		TraceMeleeAttack->OnHit.BindUObject(this, &UBOTWGA_MeleeAttack::OnHit);
		TraceMeleeAttack->ReadyForActivation();
	}

	AttackIndex = 0;
	IsAttackable = false;
	IsFirstRushAttack = false;

	PlayMontage(MeleeAttackMontages->AttackMontage[AttackIndex]);
}

bool UBOTWGA_MeleeAttack::CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags /*= nullptr*/, const FGameplayTagContainer* TargetTags /*= nullptr*/, OUT FGameplayTagContainer* OptionalRelevantTags /*= nullptr*/) const
{
	if (Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags) == false)
	{
		return false;
	}

	if (MeleeWeaponActor == nullptr || AbilitySystemComponent == nullptr || Character == nullptr || MeleeAttackMontages == nullptr || MeleeAttackMontages->AttackMontage.IsEmpty())
	{
		return false;
	}

	if (AbilitySystemComponent->HasMatchingGameplayTag(GAMEPLAYTAG.Character_Special_Rush) &&
		AbilitySystemComponent->HasMatchingGameplayTag(GAMEPLAYTAG.Character_Special_RushAttackEnable) == false)
	{
		// 러쉬가 발동됐는데 공격 가능 상태가 아닐 경우 return;
		return false;
	}
		
	return true;
}

void UBOTWGA_MeleeAttack::InputPressed(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo)
{
	if (IsAttackable == false)
	{
		return;
	}

	IsAttackable = false;
	AttackIndex = ++AttackIndex % MeleeAttackMontages->AttackMontage.Num();

	PlayMontage(MeleeAttackMontages->AttackMontage[AttackIndex]);
}

void UBOTWGA_MeleeAttack::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	Character->GetMesh()->GetAnimInstance()->SetRootMotionMode(ERootMotionMode::RootMotionFromMontagesOnly);

	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UBOTWGA_MeleeAttack::PlayMontage(UAnimMontage* _Montage)
{
	if (IsValid(PlayMontageAndWaitForEvent))
	{
		PlayMontageAndWaitForEvent->StopPlayingMontage();
		PlayMontageAndWaitForEvent->ExternalCancel();
	}
	
	FGameplayTagContainer tagContainer(GAMEPLAYTAG.Event_Weapon);
	PlayMontageAndWaitForEvent = UAT_PlayMontageAndWaitForEvent::PlayMontageAndWaitForEvent(this, _Montage, tagContainer);
	if (PlayMontageAndWaitForEvent)
	{
		PlayMontageAndWaitForEvent->EventReceived.AddDynamic(this, &UBOTWGA_MeleeAttack::OnGameplayEvent);
		PlayMontageAndWaitForEvent->OnInterrupted.AddDynamic(this, &UBOTWGA_MeleeAttack::OnGameplayEvent);
		PlayMontageAndWaitForEvent->OnCompleted.AddDynamic(this, &UBOTWGA_MeleeAttack::OnGameplayEvent);

		PlayMontageAndWaitForEvent->ReadyForActivation();
	}

	if (TraceMeleeAttack)
	{
		TraceMeleeAttack->HitArrayClear();
	}

	if (AbilitySystemComponent->HasMatchingGameplayTag(GAMEPLAYTAG.Character_Special_Rush) &&
		MotionWarpingComponent && PlayerCombatComponent && PlayerCombatComponent->GetLockOnTarget() &&
		Character->GetCapsuleComponent() && PlayerCombatComponent->GetLockOnTarget()->GetCapsuleComponent())
	{
		if (IsFirstRushAttack == false && Character->GetCharacterMovement()->IsFalling())
		{
			Character->GetCharacterMovement()->SetMovementMode(MOVE_Flying);
		}

		IsFirstRushAttack = true;

		ACharacterBase* lockOnTarget = PlayerCombatComponent->GetLockOnTarget();
		float characterCapsuleRadius = Character->GetCapsuleComponent()->GetScaledCapsuleRadius();
		float targetCapsuleRadius = lockOnTarget->GetCapsuleComponent()->GetScaledCapsuleRadius();
		float targetDistance = (characterCapsuleRadius + targetCapsuleRadius) * 2.f;

		FVector dirVec = (Character->GetActorLocation() - lockOnTarget->GetActorLocation()).GetSafeNormal();
		dirVec.Z = 0.f;

		FVector targetLocation = lockOnTarget->GetActorLocation();
		targetLocation.Z -= lockOnTarget->GetCapsuleComponent()->GetScaledCapsuleHalfHeight();

		FVector motionWarpingLocation = targetLocation + (dirVec * targetDistance);

		MotionWarpingComponent->AddOrUpdateWarpTargetFromLocation(FName("RushAttack"), motionWarpingLocation);

		HandleGameplayEvent(GAMEPLAYTAG.Event_Weapon_Attack_RushAttack);

		Character->GetMesh()->GetAnimInstance()->SetRootMotionMode(ERootMotionMode::RootMotionFromMontagesOnly);
	}
	else if (MotionWarpingComponent)
	{
		MotionWarpingComponent->RemoveWarpTarget(FName("RushAttack"));

		Character->GetMesh()->GetAnimInstance()->SetRootMotionMode(ERootMotionMode::IgnoreRootMotion);
	}

	IsApplyTimeDilation = false;
}

void UBOTWGA_MeleeAttack::OnGameplayEvent(FGameplayTag _EventTag, FGameplayEventData _EventData)
{
	if (_EventTag == GAMEPLAYTAG.Event_Weapon_Attack_Attackable)
	{
		IsAttackable = true;
	}
	else if (_EventTag == GAMEPLAYTAG.Event_Weapon_Attack_RushAttackable)
	{
		if (IsFirstRushAttack && Character->GetCharacterMovement()->IsFlying())
		{
			Character->GetCharacterMovement()->SetMovementMode(MOVE_Falling);
		}

		if (AbilitySystemComponent->HasMatchingGameplayTag(GAMEPLAYTAG.Character_Special_Rush))
		{
			IsAttackable = true;
		}
	}
	else if (_EventTag == FGameplayTag::EmptyTag)
	{
		CancelAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, false);
	}
}

void UBOTWGA_MeleeAttack::OnHit(const FHitResult& _HitResult)
{
	if (IsValid(_HitResult.GetActor()))
	{
		ApplyDamage(_HitResult.GetActor(), MeleeWeaponInstance, MeleeWeaponInstance->GetDamage());
	}

	if (IsApplyTimeDilation == false && PlayerCombatComponent)
	{
		IsApplyTimeDilation = true;

		PlayerCombatComponent->StartTimeDilation(HitTimeDilationCurve);
	}

	if (AttackIndex < MeleeAttackMontages->AttackMontage.Num())
	{
		const FComboData* comboData = MeleeWeaponInstance->GetComboData(AttackIndex);
		if (comboData)
		{
			if (_HitResult.GetActor() && comboData->HitEffect)
			{
				UNiagaraFunctionLibrary::SpawnSystemAtLocation(Character, comboData->HitEffect, _HitResult.ImpactPoint);
			}

			if (comboData->CameraShake && CurrentActorInfo->PlayerController.IsValid())
			{
				CurrentActorInfo->PlayerController->ClientStartCameraShake(comboData->CameraShake);
			}
		}
	}
}