#pragma once

#include "CoreMinimal.h"
#include "BOTWGameplayAbilityBase.h"
#include "BOTWGA_LockOn.generated.h"

class APlayerCharacter;
class AEnemyCharacter;
class UAT_CheckLockOnDistance;
class UBOTWAbilitySystemComponent;

UCLASS()
class BOTW_API UBOTWGA_LockOn : public UBOTWGameplayAbilityBase
{
	GENERATED_BODY()
	
private:
	UPROPERTY()
	APlayerCharacter* Player;

	UPROPERTY()
	UBOTWAbilitySystemComponent* AbilitySystemComponent;

	UPROPERTY()
	mutable AEnemyCharacter* NearestEnemyCharacter;

	UPROPERTY()
	UAT_CheckLockOnDistance* CheckLockOnDistance;

private:
	UPROPERTY(EditDefaultsOnly, Category = "BOTW")
	float Radius;

	UPROPERTY(EditDefaultsOnly, Category = "BOTW")
	float LimitDistance;

	UPROPERTY(EditDefaultsOnly, Category = "BOTW")
	float LimitAngle;

	UPROPERTY(EditDefaultsOnly, Category = "BOTW")
	bool IsDrawDebug;

	UPROPERTY(EditDefaultsOnly, Category = "BOTW")
	float DrawDebugDuration;

private:
	FDelegateHandle GameplayEventTagDelegateHandle;

public:
	UBOTWGA_LockOn();

	virtual void OnAvatarSet(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec) override;
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	virtual bool CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags = nullptr, const FGameplayTagContainer* TargetTags = nullptr, OUT FGameplayTagContainer* OptionalRelevantTags = nullptr) const override;
	virtual void InputReleased(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo) override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

private:
	bool SphereOverlap(OUT TArray<FOverlapResult>& _Overlaps) const;
	AEnemyCharacter* GetNearestCharacterInCameraCenter(const TArray<FOverlapResult>& _Overlaps) const;

private:
	void OnCancel();

	void OnGameplayEvent(FGameplayTag _EventTag, const FGameplayEventData* _Payload);
};