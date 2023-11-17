#pragma once

#include "CoreMinimal.h"
#include "Equipment/EquipmentInstance.h"
#include "ShieldInstance.generated.h"

UCLASS()
class BOTW_API UShieldInstance : public UEquipmentInstance
{
	GENERATED_BODY()
	
private:
	UPROPERTY(EditDefaultsOnly, Category = "Info")
	int32 Armor;

public:
	UShieldInstance();

	virtual void OnEquipped() override;
	virtual void OnUnEquipped() override;

public:
	FORCEINLINE int32 GetArmor() const { return Armor; }
};
