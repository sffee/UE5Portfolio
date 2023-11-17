#include "AT_TraceMeleeAttack.h"

#include "Equipment/EquipmentActor.h"
#include "Kismet/KismetSystemLibrary.h"

const FName NAME_AttackSocket1(TEXT("AttackSocket1"));
const FName NAME_AttackSocket2(TEXT("AttackSocket2"));

UAT_TraceMeleeAttack::UAT_TraceMeleeAttack(const FObjectInitializer& ObjectInitializer)
	: TraceSphereRadius(0.f)
	, DrawDebugDuration(0.f)
	, IsTrace(false)
{
	bTickingTask = true;
}

UAT_TraceMeleeAttack* UAT_TraceMeleeAttack::TraceMeleeAttack(UGameplayAbility* _OwningAbility, AEquipmentActor* _WeaponActor, float _TraceSphereRadius, float _DrawDebugDuration)
{
	UAT_TraceMeleeAttack* task = NewAbilityTask<UAT_TraceMeleeAttack>(_OwningAbility);
	task->OwingActor = _OwningAbility->GetCurrentActorInfo()->AvatarActor.Get();
	task->WeaponActor = _WeaponActor;
	task->Mesh = _WeaponActor->GetMesh();
	task->TraceSphereRadius = _TraceSphereRadius;
	task->DrawDebugDuration = _DrawDebugDuration;

	FGameplayTagContainer tagContainer;
	tagContainer.AddTag(GAMEPLAYTAG.Event_Weapon_Attack_CollisionEnable);
	tagContainer.AddTag(GAMEPLAYTAG.Event_Weapon_Attack_CollisionDisable);
	task->EventTagContainer = tagContainer;

	return task;
}

void UAT_TraceMeleeAttack::Activate()
{
	if (WeaponActor == nullptr || Mesh == nullptr)
	{
		EndTask();
		return;
	}

	if (Mesh->DoesSocketExist(NAME_AttackSocket1) == false || Mesh->DoesSocketExist(NAME_AttackSocket2) == false)
	{
		EndTask();
		return;
	}

	IgnoreActors.Add(OwingActor);

	SocketNames.Add(NAME_AttackSocket1);
	SocketNames.Add(NAME_AttackSocket2);

	LastSocketLocations.Add(Mesh->GetSocketLocation(NAME_AttackSocket1));
	LastSocketLocations.Add(Mesh->GetSocketLocation(NAME_AttackSocket2));

	EventHandle = AbilitySystemComponent->AddGameplayEventTagContainerDelegate(EventTagContainer, FGameplayEventTagMulticastDelegate::FDelegate::CreateUObject(this, &UAT_TraceMeleeAttack::OnGameplayEvent));
}

void UAT_TraceMeleeAttack::TickTask(float DeltaTime)
{
	Super::TickTask(DeltaTime);

	if (IsTrace == false)
	{
		return;
	}

	TArray<FHitResult> hitResults;
	Trace(hitResults);

	for (FHitResult& hitResult : hitResults)
	{
		if (hitResult.GetActor() == nullptr)
		{
			continue;
		}

		OnHit.ExecuteIfBound(hitResult);
	}

	LastSocketLocations[0] = Mesh->GetSocketLocation(NAME_AttackSocket1);
	LastSocketLocations[1] = Mesh->GetSocketLocation(NAME_AttackSocket2);
}

void UAT_TraceMeleeAttack::OnDestroy(bool AbilityEnded)
{
	Super::OnDestroy(AbilityEnded);

	AbilitySystemComponent->RemoveGameplayEventTagContainerDelegate(EventTagContainer, EventHandle);
}

void UAT_TraceMeleeAttack::HitArrayClear()
{
	HitArray.Empty();
}

void UAT_TraceMeleeAttack::Trace(TArray<FHitResult>& _HitResult)
{	
	for (const FName& socketName : SocketNames)
	{
		for (const FVector& lastSocketLocation : LastSocketLocations)
		{
			Trace(Mesh->GetSocketLocation(socketName), lastSocketLocation, _HitResult);
		}
	}

	Trace(Mesh->GetSocketLocation(SocketNames[0]), Mesh->GetSocketLocation(SocketNames[1]), _HitResult);
	Trace(LastSocketLocations[0], LastSocketLocations[1], _HitResult);
}

void UAT_TraceMeleeAttack::Trace(const FVector& _TraceStart, const FVector& _TraceEnd, TArray<FHitResult>& _HitResult)
{
	TArray<FHitResult> hitResults;
	EDrawDebugTrace::Type drawDebugTrace = 0.f < DrawDebugDuration ? EDrawDebugTrace::ForDuration : EDrawDebugTrace::None;

	UKismetSystemLibrary::SphereTraceMultiForObjects(OwingActor, _TraceStart, _TraceEnd, TraceSphereRadius, { UEngineTypes::ConvertToObjectType(ECC_Pawn) },
													 false, IgnoreActors, drawDebugTrace, hitResults, true, FLinearColor::Red, FLinearColor::Green, DrawDebugDuration);

	for (FHitResult& hitResult : hitResults)
	{
		if (HitArray.Contains(hitResult.GetActor()) == false)
		{
			HitArray.AddUnique(hitResult.GetActor());
			_HitResult.Add(hitResult);
		}
	}
}

void UAT_TraceMeleeAttack::OnGameplayEvent(FGameplayTag _EventTag, const FGameplayEventData* _Payload)
{
	if (_EventTag == GAMEPLAYTAG.Event_Weapon_Attack_CollisionEnable)
	{
		IsTrace = true;
	}
	else if (_EventTag == GAMEPLAYTAG.Event_Weapon_Attack_CollisionDisable)
	{
		IsTrace = false;
	}
}