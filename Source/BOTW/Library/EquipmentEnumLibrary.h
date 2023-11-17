#pragma once

#include "CoreMinimal.h"
#include "EquipmentEnumLibrary.generated.h"

UENUM(BlueprintType)
enum class EEquipmentType : uint8
{
	Weapon,
	Shield,
	
	Head,
	Body,
	Leg,

	MAX
};

UENUM(BlueprintType)
enum class EOverlayType : uint8
{
	Default,
	OneHandedWeapon UMETA(DisplayName = "One-Handed Weapon"),

	Carry
};