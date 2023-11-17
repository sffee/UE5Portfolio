#pragma once

#include "Library/EquipmentEnumLibrary.h"

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "WeaponAnimationData.generated.h"

USTRUCT(BlueprintType)
struct FWeaponAnim
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditDefaultsOnly)
	TArray<UAnimMontage*> AttackMontage;
};

UCLASS()
class BOTW_API UWeaponAnimationData : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly)
	TMap<EOverlayType, FWeaponAnim> Data;
};