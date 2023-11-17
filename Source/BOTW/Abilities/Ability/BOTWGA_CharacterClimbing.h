#pragma once

#include "CoreMinimal.h"
#include "BOTWGameplayAbilityBase.h"
#include "BOTWGA_CharacterClimbing.generated.h"

class APlayerCharacter;
class UClimbingComponent;
class ULocomotionComponent;

UCLASS()
class BOTW_API UBOTWGA_CharacterClimbing : public UBOTWGameplayAbilityBase
{
	GENERATED_BODY()

private:
	UPROPERTY()
	APlayerCharacter* Player;

	UPROPERTY()
	UClimbingComponent* ClimbingComponent;

	UPROPERTY()
	ULocomotionComponent* LocomotionComponent;

private:
	FDelegateHandle OnEndClimbingDelegateHandle;

public:
	UBOTWGA_CharacterClimbing();

	virtual void OnAvatarSet(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec) override;
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	virtual bool CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags = nullptr, const FGameplayTagContainer* TargetTags = nullptr, OUT FGameplayTagContainer* OptionalRelevantTags = nullptr) const override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled);
};
