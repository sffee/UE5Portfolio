#include "AT_CheckLockOnDistance.h"

UAT_CheckLockOnDistance::UAT_CheckLockOnDistance(const FObjectInitializer& ObjectInitializer)
	: LimitDistance(0.f)
{
	bTickingTask = true;
}

UAT_CheckLockOnDistance* UAT_CheckLockOnDistance::CheckLockOnDistance(UGameplayAbility* _OwningAbility, AActor* _Target, float _LimitDistance)
{
	UAT_CheckLockOnDistance* task = NewAbilityTask<UAT_CheckLockOnDistance>(_OwningAbility);
	task->OwingActor = _OwningAbility->GetCurrentActorInfo()->AvatarActor.Get();
	task->Target = _Target;
	task->LimitDistance = _LimitDistance;

	return task;
}

void UAT_CheckLockOnDistance::Activate()
{
	if (OwingActor == nullptr || Target == nullptr || LimitDistance <= 0.f)
	{
		EndTask();
		return;
	}
}

void UAT_CheckLockOnDistance::TickTask(float DeltaTime)
{
	Super::TickTask(DeltaTime);

	float distance = FVector::Distance(OwingActor->GetActorLocation(), Target->GetActorLocation());
	if (LimitDistance < distance)
	{
		OnCancel.ExecuteIfBound();
		
		EndTask();
	}
}
