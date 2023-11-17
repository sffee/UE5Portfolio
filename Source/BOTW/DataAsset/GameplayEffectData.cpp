#include "GameplayEffectData.h"

#include "Manager/BOTWAssetManager.h"

const UGameplayEffectData& UGameplayEffectData::Get()
{
	return UBOTWAssetManager::Get().GetGameplayEffectData();
}
