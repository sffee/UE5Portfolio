#include "AT_WaitDelay.h"

UAT_WaitDelay::UAT_WaitDelay(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
	, EndTime(0.f)
{
	bTickingTask = true;
}

UAT_WaitDelay* UAT_WaitDelay::WaitDelay(UGameplayAbility* _OwningAbility, float _Delay, FSimpleDelegate&& _Delegate)
{
	UAT_WaitDelay* task = NewAbilityTask<UAT_WaitDelay>(_OwningAbility);

	task->EndTime = task->GetWorld()->GetRealTimeSeconds() + _Delay;
	task->DelayEndDelegate = MoveTemp(_Delegate);

	return task;
}

void UAT_WaitDelay::TickTask(float _DeltaTime)
{
	Super::TickTask(_DeltaTime);

	float time = GetWorld()->GetRealTimeSeconds();
	if (EndTime <= time)
	{
		DelayEndDelegate.ExecuteIfBound();

		EndTask();
	}
}