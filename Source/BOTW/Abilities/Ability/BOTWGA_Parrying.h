#pragma once

#include "Curves/CurveFloat.h"

#include "CoreMinimal.h"
#include "Abilities/Ability/BOTWGameplayAbilityBase.h"
#include "BOTWGA_Parrying.generated.h"

class UShieldInstance;
class UAnimMontage;
class UAT_PlayMontageAndWaitForEvent;

UCLASS()
class BOTW_API UBOTWGA_Parrying : public UBOTWGameplayAbilityBase
{
	GENERATED_BODY()

private:
	UPROPERTY()
	UBOTWAbilitySystemComponent* AbilitySystemComponent;

	UPROPERTY()
	UAT_PlayMontageAndWaitForEvent* PlayMontageAndWaitForEvent;

private:
	UPROPERTY(EditDefaultsOnly, Category = "BOTW")
	UAnimMontage* AnimMontage;

	UPROPERTY(EditDefaultsOnly, Category = "BOTW")
	FRuntimeFloatCurve ParryingSuccessTimeDilationCurve;

public:
	UBOTWGA_Parrying();

	virtual void OnAvatarSet(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec) override;
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	virtual bool CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags = nullptr, const FGameplayTagContainer* TargetTags = nullptr, OUT FGameplayTagContainer* OptionalRelevantTags = nullptr) const override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled);

private:
	UFUNCTION()
	void OnGameplayEvent(FGameplayTag _EventTag, FGameplayEventData _EventData);
};
