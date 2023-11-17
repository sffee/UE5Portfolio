#pragma once

#include "CoreMinimal.h"
#include "Abilities/Ability/BOTWGameplayAbilityBase.h"
#include "BOTWGA_Carrying.generated.h"

class UCarryComponent;

UCLASS()
class BOTW_API UBOTWGA_Carrying : public UBOTWGameplayAbilityBase
{
	GENERATED_BODY()
	
private:
	UPROPERTY()
	UCarryComponent* CarryComponent;

	FDelegateHandle DelegateHandle;

private:
	UBOTWGA_Carrying();

	virtual void OnAvatarSet(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec) override;
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	virtual bool CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags = nullptr, const FGameplayTagContainer* TargetTags = nullptr, OUT FGameplayTagContainer* OptionalRelevantTags = nullptr) const override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

	virtual void InputPressed(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo) override;

private:
	void OnCarryEndTag(FGameplayTag _EventTag, const FGameplayEventData* _Payload);
};