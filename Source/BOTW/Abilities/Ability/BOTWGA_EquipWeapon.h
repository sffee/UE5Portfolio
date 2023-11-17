#pragma once

#include "CoreMinimal.h"
#include "Abilities/Ability/BOTWGameplayAbilityBase.h"
#include "BOTWGA_EquipWeapon.generated.h"

class UEquipmentManagerComponent;

UCLASS()
class BOTW_API UBOTWGA_EquipWeapon : public UBOTWGameplayAbilityBase
{
	GENERATED_BODY()
	
private:
	UPROPERTY()
	UEquipmentManagerComponent* EquipmentManagerComponent;

public:
	UBOTWGA_EquipWeapon();

public:
	virtual void OnAvatarSet(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec) override;
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	virtual bool CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags = nullptr, const FGameplayTagContainer* TargetTags = nullptr, OUT FGameplayTagContainer* OptionalRelevantTags = nullptr) const override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled);
};
