#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Components/SphereComponent.h"
#include "ItemDefinition.generated.h"

class UItemInstance;

UCLASS(DefaultToInstanced, EditInlineNew, Abstract)
class UItemFragment : public UObject
{
	GENERATED_BODY()

public:
	virtual void OnInstanceCreated(UItemInstance* _Instance) const { }
};

UENUM(BlueprintType)
enum class EItemCategoryType : uint8
{
	WEAPONS,	// 무기
	BOWS,		// 활
	ARROWS,		// 화살
	SHIELDS,	// 방패
	CLOTHING,	// 방어구
	MATERIALS,  // 소재
	FOOD,		// 요리
	SPECIAL,	// 소중한 물건
};

UENUM(BlueprintType)
enum class EBonusEffectType : uint8
{
	NONE,
	BONUS_HEART,
	BONUS_STAMINA,
	STAMINA_UP,
	ATTACK_UP,
	CRITICAL_HIT,
	SHIELD,
	DURABILITY_UP,
	HEAT_RESIST,
	FROST_RESIST,
	FIRE_RESIST,
	ELECTRIC_RESIST,
	QUIET,
	SPEED_UP,
	DEFENSE,
	LONG_THROW,
	CLIMB_SPEED_UP,
	TIMER,
	SWIM_SPEED_UP,
	SNOW,
};

UENUM(BlueprintType)
enum class EItemMaterialType : uint8
{
	NONE,
	WOOD,
	METAL,
	BONE,
};

UCLASS(Blueprintable, Const, Abstract)
class BOTW_API UItemDefinition : public UObject
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditDefaultsOnly, Category = ItemInfo)
	int32 ID;

	UPROPERTY(EditDefaultsOnly, Category = ItemInfo)
	EItemCategoryType Category;

	UPROPERTY(EditDefaultsOnly, Category = ItemInfo)
	FString Name;

	UPROPERTY(EditDefaultsOnly, Category = ItemInfo)
	FString Description;

	UPROPERTY(EditDefaultsOnly, Category = ItemInfo)
	UTexture2D* Image;

	UPROPERTY(EditDefaultsOnly, Category = ItemInfo)
	EItemMaterialType Material;

	UPROPERTY(EditDefaultsOnly, Category = ItemInfo)
	int32 Durability;						// -1: No Durability

	UPROPERTY(EditDefaultsOnly, Category = ItemInfo)
	int32 AttackDefenseValues;				// 0: No Attack Or Defense

	UPROPERTY(EditDefaultsOnly, Category = ItemInfo)
	int32 AttackMultiple;					// 활의 공격력 배수

	UPROPERTY(EditDefaultsOnly, Category = ItemInfo)
	EBonusEffectType BonusEffect;

	UPROPERTY(EditDefaultsOnly, Category = ItemInfo)
	int32 HealValues;

	UPROPERTY(EditDefaultsOnly, Category = ItemInfo)
	int32 StaminaValues;

	UPROPERTY(EditDefaultsOnly, Category = ItemInfo)
	bool IsStack;

	UPROPERTY(EditDefaultsOnly, Category = Fragment, Instanced)
	TArray<UItemFragment*> Fragments;

public:
	UItemDefinition();

	const UItemFragment* FindFragmentByClass(TSubclassOf<UItemFragment> _FragmentClass) const;
};
