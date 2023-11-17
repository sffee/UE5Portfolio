#pragma once

#include "CoreMinimal.h"
#include "WeaponInstance.h"
#include "MeleeWeaponInstance.generated.h"

class UNiagaraSystem;

USTRUCT(BlueprintType)
struct FComboData
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UCameraShakeBase> CameraShake;

	UPROPERTY(EditDefaultsOnly)
	UNiagaraSystem* HitEffect;
};

UCLASS()
class BOTW_API UMeleeWeaponInstance : public UWeaponInstance
{
	GENERATED_BODY()
	
public:
	UMeleeWeaponInstance();

private:
	UPROPERTY(EditDefaultsOnly, Category = "Combo")
	TArray<FComboData> ComboDatas;

public:
	FORCEINLINE const FComboData* GetComboData(int32 _Index) const { return &ComboDatas[_Index]; }
};
