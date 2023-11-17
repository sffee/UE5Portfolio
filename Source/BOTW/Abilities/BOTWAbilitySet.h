#pragma once

#include "Library/GASLibrary.h"

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "BOTWAbilitySet.generated.h"

class UBOTWAbilitySystemComponent;
class UBOTWGameplayAbilityBase;
class UBOTWGameplayEffectBase;

USTRUCT(BlueprintType)
struct FBOTWAbilitySet_GameplayAbility
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UBOTWGameplayAbilityBase> Ability;

	UPROPERTY(EditDefaultsOnly, Meta = (Categories = "InputTag"))
	FGameplayTag InputTag;

	UPROPERTY(EditDefaultsOnly)
	bool IsOnlyPlayer;

public:
	FBOTWAbilitySet_GameplayAbility()
		: IsOnlyPlayer(false)
	{ }
};

USTRUCT(BlueprintType)
struct FBOTWAbilitySet_GrantedHandles
{
	GENERATED_BODY()

protected:
	UPROPERTY()
	TArray<FGameplayAbilitySpecHandle> AbilitySpecHandles;

	UPROPERTY()
	TArray<FActiveGameplayEffectHandle> GameplayEffectHandles;

	UPROPERTY()
	TArray<UAttributeSet*> GrantedAttributeSets;
	
public:
	void AddAbilitySpecHandle(const FGameplayAbilitySpecHandle& _Handle);
	void AddGameplayEffectHandle(const FActiveGameplayEffectHandle& _Handle);
	void AddAttributeSet(UAttributeSet* _AttributeSet);

	void TakeFromAbilitySystem(UAbilitySystemComponent* _AbilitySystemComponent);
};

UCLASS(BlueprintType, Const)
class BOTW_API UBOTWAbilitySet : public UPrimaryDataAsset
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditDefaultsOnly, Category = "Gameplay Abilities", meta = (TitleProperty = Ability))
	TArray<FBOTWAbilitySet_GameplayAbility> GrantedGameplayAbilities;

	UPROPERTY(EditDefaultsOnly, Category = "Gameplay Effects")
	TArray<TSubclassOf<UBOTWGameplayEffectBase>> GrantedGameplayEffects;

	UPROPERTY(EditDefaultsOnly, Category = "AttributeSet")
	TArray<TSubclassOf<UAttributeSet>> GrantedAttributes;

public:
	void GiveToAbilitySystem(UBOTWAbilitySystemComponent* _AbilitySystemComponent, FBOTWAbilitySet_GrantedHandles* _OutGrantedHandles, UObject* _SourceObject = nullptr) const;
};
