#pragma once

#include "Library/GASLibrary.h"
#include "Abilities/BOTWAbilitySet.h"

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "BOTWGameplayAbilityBase.generated.h"

class USkeletalMeshComponent;
class UAnimMontage;
class UBOTWAbilitySystemComponent;

USTRUCT()
struct BOTW_API FAbilityMeshMontage
{
	GENERATED_BODY()

public:
	UPROPERTY()
	USkeletalMeshComponent* Mesh;

	UPROPERTY()
	UAnimMontage* Montage;

	FAbilityMeshMontage()
		: Mesh(nullptr)
		, Montage(nullptr)
	{
	}

	FAbilityMeshMontage(class USkeletalMeshComponent* _Mesh, class UAnimMontage* _Montage)
		: Mesh(_Mesh)
		, Montage(_Montage)
	{
	}

};
UCLASS()
class BOTW_API UBOTWGameplayAbilityBase : public UGameplayAbility
{
	GENERATED_BODY()
	
protected:
	UPROPERTY(EditAnywhere, Category = "Ability")
	FGameplayTag AbilityInputTag;

	UPROPERTY(EditAnywhere, Category = "Ability")
	EAbilityActivationPolicy AbilityActivationPolicy;

	UPROPERTY(EditAnywhere, Category = "Ability")
	UBOTWAbilitySet* AbilitySet;

	FBOTWAbilitySet_GrantedHandles GrantedAbilityHandles;

private:
	FActiveGameplayEffectHandle StaminaSpecHandle;

	UPROPERTY(EditDefaultsOnly, Category = "Cost|Stamina")
	float StaminaRequired;

	UPROPERTY(EditDefaultsOnly, Category = "Cost|Stamina")
	float StaminaCost;

	UPROPERTY(EditDefaultsOnly, Category = "Cost|Stamina")
	bool IsStaminaCostOnce;

private:
	UPROPERTY()
	TArray<FAbilityMeshMontage> CurrentAbilityMeshMontages;

public:
	UBOTWGameplayAbilityBase();

	virtual void OnGiveAbility(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec) override;
	virtual void OnAvatarSet(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec) override;
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	virtual bool CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags = nullptr, const FGameplayTagContainer* TargetTags = nullptr, OUT FGameplayTagContainer* OptionalRelevantTags = nullptr) const override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled);

	void TryActivatePassiveAbility(const FGameplayAbilityActorInfo* _ActorInfo, const FGameplayAbilitySpec& _Spec);

private:
	void StaminaEmpty();

protected:
	FActiveGameplayEffectHandle ApplyDamage(AActor* _TargetActor, const UObject* _SourceObject, float _Damage, const FGameplayTagContainer& _DynamicAssetTag = FGameplayTagContainer());
	FActiveGameplayEffectHandle ApplyDamage(UAbilitySystemComponent* _TargetASC, const UObject* _SourceObject, float _Damage, const FGameplayTagContainer& _DynamicAssetTag = FGameplayTagContainer());

protected:
	int32 HandleGameplayEvent(FGameplayTag _EventTag);
	int32 HandleGameplayEvent(FGameplayTag _EventTag, const FGameplayEventData* _Payload);

public:
	void ExternalEndAbility();

protected:
	FORCEINLINE UBOTWAbilitySystemComponent* GetBOTWAbilitySystemComponent() { return Cast<UBOTWAbilitySystemComponent>(GetAbilitySystemComponentFromActorInfo()); }

public:
	FORCEINLINE FGameplayTag GetAbilityInputTag() const { return AbilityInputTag; }
	FORCEINLINE EAbilityActivationPolicy GetAbilityActivationPolicy() const { return AbilityActivationPolicy; }
};
