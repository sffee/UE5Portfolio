#pragma once

#include "CoreMinimal.h"
#include "Abilities/Tasks/AbilityTask.h"
#include "AT_WaitDelay.generated.h"

UCLASS()
class BOTW_API UAT_WaitDelay : public UAbilityTask
{
	GENERATED_UCLASS_BODY()

public:
	FSimpleDelegate DelayEndDelegate;

private:
	float EndTime;

public:
	static UAT_WaitDelay* WaitDelay(UGameplayAbility* _OwningAbility, float _Delay, FSimpleDelegate&& _Delegate);

private:
	virtual void TickTask(float _DeltaTime) override;
};
