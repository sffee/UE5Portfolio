#pragma once

#include "GameplayTagContainer.h"
#include "GameplayEffectExtension.h"
#include "GameplayEffectTypes.h"
#include "GameplayAbilitySpec.h"
#include "GameplayTagsManager.h"
#include "AbilitySystemInterface.h"
#include "AbilitysystemBlueprintLibrary.h"

#include "Abilities/BOTWGameplayEffectBase.h"
#include "Abilities/BOTWAbilitySystemComponent.h"
#include "BOTWGameplayTags.h"

#include "Manager/BOTWAssetManager.h"
#include "DataAsset/GameplayEffectData.h"

#include "CoreMinimal.h"

#include "GASLibrary.generated.h"

#define GAMEPLAYTAG FBOTWGameplayTags::Get()

UENUM(BlueprintType)
enum class EAbilityActivationPolicy : uint8
{
	OnInputTriggered,
	WhileInputActive,
	OnSpawn
};

UCLASS()
class BOTW_API UBOTWGASFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Ability")
	static FActiveGameplayEffectHandle ApplyDamage(UAbilitySystemComponent* _SourceASC, UAbilitySystemComponent* _TargetASC, float _Damage, const UObject* _SourceObject = nullptr, const FGameplayTagContainer& _DynamicAssetTag = FGameplayTagContainer());
};