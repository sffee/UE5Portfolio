#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "GameplayEffectData.generated.h"

class UGameplayEffect;

UCLASS(BlueprintType, Const)
class BOTW_API UGameplayEffectData : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, Category = "BOTW|Status")
	TSoftClassPtr<UGameplayEffect> ConsumeStaminaEffect_Instance;

	UPROPERTY(EditDefaultsOnly, Category = "BOTW|Status")
	TSoftClassPtr<UGameplayEffect> ConsumeStaminaEffect_Continuous;

	UPROPERTY(EditDefaultsOnly, Category = "BOTW|Damage")
	TSoftClassPtr<UGameplayEffect> ApplyDamageEffect_SetByCaller;

public:
	static const UGameplayEffectData& Get();
};
