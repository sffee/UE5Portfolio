#include "ItemFragment_SetStats.h"

#include "Inventory/ItemInstance.h"

void UItemFragment_SetStats::OnInstanceCreated(UItemInstance* _Instance) const
{
	for (const auto& stat : InitStats)
	{
		//_Instance->AddStatTagStack(stat.Key, stat.Value);
	}
}

int32 UItemFragment_SetStats::GetStatByTag(FGameplayTag _Tag)
{
	const int32* stat = InitStats.Find(_Tag);

	return stat ? *stat : 0;
}