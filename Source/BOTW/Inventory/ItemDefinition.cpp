#include "ItemDefinition.h"

UItemDefinition::UItemDefinition()
{
	Material = EItemMaterialType::NONE;
	Durability = -1;
	AttackDefenseValues = 0;
	AttackMultiple = 1;
	BonusEffect = EBonusEffectType::NONE;
	HealValues = 0;
	IsStack = false;
}

const UItemFragment* UItemDefinition::FindFragmentByClass(TSubclassOf<UItemFragment> _FragmentClass) const
{
	if (_FragmentClass != nullptr)
	{
		for (UItemFragment* fragment : Fragments)
		{
			if (fragment && fragment->IsA(_FragmentClass))
			{
				return fragment;
			}
		}
	}

	return nullptr;
}
