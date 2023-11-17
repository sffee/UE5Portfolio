#include "WeaponInstance.h"

UWeaponInstance::UWeaponInstance()
	: Damage(0.f)
{

}

void UWeaponInstance::OnEquipped()
{
	Super::OnEquipped();
}

void UWeaponInstance::OnUnEquipped()
{
	Super::OnUnEquipped();
}
