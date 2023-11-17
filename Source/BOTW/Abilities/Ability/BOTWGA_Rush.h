#pragma once

#include "CoreMinimal.h"
#include "BOTWGameplayAbilityBase.h"
#include "BOTWGA_Rush.generated.h"

class APlayerCharacter;
class UPlayerCombatComponent;
class UBOTWAbilitySystemComponent;
class ABOTWPlayerCameraManager;
class UAT_WaitDelay;
class UAT_CorrectionRushTimeDilation;

UCLASS()
class BOTW_API UBOTWGA_Rush : public UBOTWGameplayAbilityBase
{
	GENERATED_BODY()

private:
	UPROPERTY()
	APlayerCharacter* Player;

	UPROPERTY()
	UPlayerCombatComponent* PlayerCombatComponent;

	UPROPERTY()
	ABOTWPlayerCameraManager* CameraManager;

	UPROPERTY()
	UBOTWAbilitySystemComponent* AbilitySystemComponent;

	UPROPERTY()
	UAT_WaitDelay* WaitDelay;

	UPROPERTY()
	UAT_CorrectionRushTimeDilation* CorrectionRushTimeDilation;

private:
	UPROPERTY(EditDefaultsOnly, Category = "BOTW")
	float RushAttackEnableTime;

	bool IsRushAttackStart;

private:
	UPROPERTY(EditDefaultsOnly, Category = "BOTW")
	UCurveFloat* RushTimeDilationCurve;

	UPROPERTY(EditDefaultsOnly, Category = "BOTW")
	UCurveFloat* RushAttackTimeDilationCurve;

	FDelegateHandle TimeDilationEndDelegateHandle;

private:
	FDelegateHandle GameplayEventTagDelegateHandle;
	
public:
	UBOTWGA_Rush();

	virtual void OnAvatarSet(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec) override;
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	virtual bool CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags = nullptr, const FGameplayTagContainer* TargetTags = nullptr, OUT FGameplayTagContainer* OptionalRelevantTags = nullptr) const override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled);

private:
	void OnGameplayEvent(FGameplayTag _EventTag, const FGameplayEventData* _Payload);
	void RushAttackEnable();
};
