#pragma once

#include "CoreMinimal.h"
#include "BOTWGameplayAbilityBase.h"
#include "BOTWGA_Interaction.generated.h"

class UInteractionComponent;

UCLASS()
class BOTW_API UBOTWGA_Interaction : public UBOTWGameplayAbilityBase
{
	GENERATED_BODY()
	
public:
	UBOTWGA_Interaction();

	virtual void OnAvatarSet(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec) override;
	virtual bool CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags = nullptr, const FGameplayTagContainer* TargetTags = nullptr, OUT FGameplayTagContainer* OptionalRelevantTags = nullptr) const override;
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

private:
	TWeakObjectPtr<UInteractionComponent> InteractionComp;
};
