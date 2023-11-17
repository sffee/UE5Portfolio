#pragma once

#include "CoreMinimal.h"
#include "Abilities/Tasks/AbilityTask.h"
#include "AT_CheckLockOnDistance.generated.h"

UCLASS()
class BOTW_API UAT_CheckLockOnDistance : public UAbilityTask
{
	GENERATED_BODY()
	
private:
	UPROPERTY()
	AActor* OwingActor;

	UPROPERTY()
	AActor* Target;

	float LimitDistance;

public:
	FSimpleDelegate OnCancel;
	
public:
	UAT_CheckLockOnDistance(const FObjectInitializer& ObjectInitializer);

	UFUNCTION()
	static UAT_CheckLockOnDistance* CheckLockOnDistance(UGameplayAbility* _OwningAbility, AActor* _Target, float _LimitDistance);

public:
	virtual void Activate() override;
	virtual void TickTask(float DeltaTime) override;
};
