#include "BOTWAssetManager.h"

#include "AbilitySystemGlobals.h"
#include "Library/BOTWGameplayTags.h"

#include "DataAsset/GameplayEffectData.h"

const UGameplayEffectData& UBOTWAssetManager::GetGameplayEffectData()
{
	return GetOrLoadTypedData<UGameplayEffectData>(GameplayEffectDataPath);
}

void UBOTWAssetManager::StartInitialLoading()
{
	Super::StartInitialLoading();

	GetGameplayEffectData();

	UAbilitySystemGlobals::Get().InitGlobalData();
	FBOTWGameplayTags::InitializeTags();
}

UBOTWAssetManager& UBOTWAssetManager::Get()
{
	check(GEngine);

	UBOTWAssetManager* assetManager = Cast<UBOTWAssetManager>(GEngine->AssetManager);
	if (assetManager)
	{
		return *assetManager;
	}

	return *NewObject<UBOTWAssetManager>();
}

UObject* UBOTWAssetManager::SynchronousLoadAsset(const FSoftObjectPath& _AssetPath)
{
	if (_AssetPath.IsValid())
	{
		if (UAssetManager::IsValid())
		{
			return UAssetManager::GetStreamableManager().LoadSynchronous(_AssetPath);
		}

		return _AssetPath.TryLoad();
	}

	return nullptr;
}

UPrimaryDataAsset* UBOTWAssetManager::LoadDataOfClass(TSubclassOf<UPrimaryDataAsset> _DataClass, const TSoftObjectPtr<UPrimaryDataAsset>& _DataClassPath, FPrimaryAssetType _PrimaryAssetType)
{
	UPrimaryDataAsset* asset = nullptr;

	if (_DataClassPath.IsNull() == false)
	{
		if (GIsEditor)
		{
			asset = _DataClassPath.LoadSynchronous();
			LoadPrimaryAssetsWithType(_PrimaryAssetType);
		}
		else
		{
			TSharedPtr<FStreamableHandle> handle = LoadPrimaryAssetsWithType(_PrimaryAssetType);
			if (handle.IsValid())
			{
				handle->WaitUntilComplete(0.0f, false);
				asset = Cast<UPrimaryDataAsset>(handle->GetLoadedAsset());
			}
		}
	}

	if (asset)
	{
		DataMap.Add(_DataClass, asset);
	}

	return asset;
}
