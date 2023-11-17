#pragma once

#include "Library/EquipmentEnumLibrary.h"
#include "Weapon/WeaponAnimationData.h"

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "CombatInterface.generated.h"

class UAnimMontage;

UINTERFACE(MinimalAPI)
class UCombatInterface : public UInterface
{
	GENERATED_BODY()
};

class BOTW_API ICombatInterface
{
	GENERATED_BODY()

public:
	virtual const FWeaponAnim* GetWeaponAnimData(const EOverlayType& _OverlayType) { return nullptr; }
	virtual UAnimMontage* GetShieldGuardImpactAnimMontage() const { return nullptr; }
	virtual float GetKnockBackPower() const { return 0.f; }
};