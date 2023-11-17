#pragma once

#include "CoreMinimal.h"
#include "ItemDefinition.h"
#include "ItemFragment_EquippableItem.generated.h"

class UEquipmentDefinition;

UCLASS()
class BOTW_API UItemFragment_EquippableItem : public UItemFragment
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditDefaultsOnly, Category = "Equipment")
	TSubclassOf<UEquipmentDefinition> EquipmentDefinition;
};
