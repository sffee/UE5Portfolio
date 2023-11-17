#include "ShieldInstance.h"

UShieldInstance::UShieldInstance()
	: Armor(0)
{

}

void UShieldInstance::OnEquipped()
{
	Super::OnEquipped();
}

void UShieldInstance::OnUnEquipped()
{
	Super::OnUnEquipped();
}
