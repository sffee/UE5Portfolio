#pragma once

#include "Weapon/WeaponAnimationData.h"

#include "CoreMinimal.h"
#include "BOTWGameplayAbilityBase.h"
#include "BOTWGA_MeleeAttack.generated.h"

class AEquipmentActor;
class UMeleeWeaponInstance;
class UBOTWAbilitySystemComponent;
class UPlayerCombatComponent;
class UMotionWarpingComponent;
class ACharacterBase;
class UAT_PlayMontageAndWaitForEvent;
class UAT_TraceMeleeAttack;
class UCurveFloat;

UCLASS()
class BOTW_API UBOTWGA_MeleeAttack : public UBOTWGameplayAbilityBase
{
	GENERATED_BODY()
	
private:
	UPROPERTY()
	UMeleeWeaponInstance* MeleeWeaponInstance;

	UPROPERTY()
	AEquipmentActor* MeleeWeaponActor;

	UPROPERTY()
	UBOTWAbilitySystemComponent* AbilitySystemComponent;

	UPROPERTY()
	UPlayerCombatComponent* PlayerCombatComponent;

	UPROPERTY()
	UMotionWarpingComponent* MotionWarpingComponent;

	UPROPERTY()
	ACharacterBase* Character;

	const FWeaponAnim* MeleeAttackMontages;

	UPROPERTY()
	UAT_PlayMontageAndWaitForEvent* PlayMontageAndWaitForEvent;

	UPROPERTY()
	UAT_TraceMeleeAttack* TraceMeleeAttack;

private:
	UPROPERTY(EditDefaultsOnly, Category = "BOTW|TimeDilation")
	UCurveFloat* HitTimeDilationCurve;

	UPROPERTY(EditDefaultsOnly, Category = "BOTW|Trace")
	float TraceSphereRadius;

	UPROPERTY(EditDefaultsOnly, Category = "BOTW|Trace")
	float DrawDebugDuration;

	int32 AttackIndex;
	bool IsAttackable;
	bool IsApplyTimeDilation;

private:
	bool IsFirstRushAttack;

public:
	UBOTWGA_MeleeAttack();

public:
	virtual void OnAvatarSet(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec) override;
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	virtual bool CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags = nullptr, const FGameplayTagContainer* TargetTags = nullptr, OUT FGameplayTagContainer* OptionalRelevantTags = nullptr) const override;
	virtual void InputPressed(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo);
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled);

private:
	void PlayMontage(UAnimMontage* _Montage);

private:
	UFUNCTION()
	void OnGameplayEvent(FGameplayTag _EventTag, FGameplayEventData _EventData);

	void OnHit(const FHitResult& _HitResult);
};
