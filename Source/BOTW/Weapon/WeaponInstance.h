#pragma once

#include "CoreMinimal.h"
#include "Equipment/EquipmentInstance.h"
#include "WeaponInstance.generated.h"

UCLASS(meta = (PrioritizeCategories = "Type Info"))
class BOTW_API UWeaponInstance : public UEquipmentInstance
{
	GENERATED_BODY()
	
private:
	UPROPERTY(EditDefaultsOnly, Category = "Type")
	EOverlayType OverlayType;

private:
	UPROPERTY(EditDefaultsOnly, Category = "Info")
	float Damage;

public:
	UWeaponInstance();

	virtual void OnEquipped() override;
	virtual void OnUnEquipped() override;

public:
	FORCEINLINE EOverlayType GetOverlayType() const { return OverlayType; }

	const float GetDamage() const { return Damage; }
};