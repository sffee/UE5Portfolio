#pragma once

#include "CoreMinimal.h"
#include "Abilities/Ability/BOTWGameplayAbilityBase.h"
#include "BOTWGA_ShieldGuard.generated.h"

class UAT_PlayMontageAndWaitForEvent;
class UBOTWAbilitySystemComponent;
class UMotionWarpingComponent;

UCLASS()
class BOTW_API UBOTWGA_ShieldGuard : public UBOTWGameplayAbilityBase
{
	GENERATED_BODY()

private:
	UPROPERTY()
	UBOTWAbilitySystemComponent* AbilitySystemComponent;

	UPROPERTY()
	UAT_PlayMontageAndWaitForEvent* PlayMontageAndWaitForEvent;

	UPROPERTY()
	UMotionWarpingComponent* MotionWarpingComponent;

	bool IsShieldImpactPlaying;
	FDelegateHandle GameplayEventTagDelegateHandle;

public:
	UBOTWGA_ShieldGuard();

	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled);
	virtual void InputReleased(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo);

private:
	void OnGameplayEvent(FGameplayTag _EventTag, const FGameplayEventData* _Payload);

	UFUNCTION()
	void OnMontageEnd(FGameplayTag _EventTag, FGameplayEventData _EventData);
};
