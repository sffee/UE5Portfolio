#pragma once

#include "CoreMinimal.h"
#include "Engine/AssetManager.h"
#include "BOTWAssetManager.generated.h"

class UGameplayEffectData;

UCLASS(Config = Game)
class BOTW_API UBOTWAssetManager : public UAssetManager
{
	GENERATED_BODY()
	
private:
	UPROPERTY(Config)
	TSoftObjectPtr<UGameplayEffectData> GameplayEffectDataPath;

	UPROPERTY(Transient)
	TMap<UClass*, UPrimaryDataAsset*> DataMap;
	
public:
	static UBOTWAssetManager& Get();

	const UGameplayEffectData& GetGameplayEffectData();

	template<typename AssetType>
	static TSubclassOf<AssetType> GetSubclass(const TSoftClassPtr<AssetType>& _AssetPointer);

protected:
	virtual void StartInitialLoading() override;

protected:
	static UObject* SynchronousLoadAsset(const FSoftObjectPath& _AssetPath);
	UPrimaryDataAsset* LoadDataOfClass(TSubclassOf<UPrimaryDataAsset> _DataClass, const TSoftObjectPtr<UPrimaryDataAsset>& _DataClassPath, FPrimaryAssetType _PrimaryAssetType);

	template <typename DataClass>
	const DataClass& GetOrLoadTypedData(const TSoftObjectPtr<DataClass>& _DataPath);
};

template<typename AssetType>
TSubclassOf<AssetType> UBOTWAssetManager::GetSubclass(const TSoftClassPtr<AssetType>& _AssetPointer)
{
	TSubclassOf<AssetType> loadedSubclass;

	const FSoftObjectPath& assetPath = _AssetPointer.ToSoftObjectPath();

	if (assetPath.IsValid())
	{
		loadedSubclass = _AssetPointer.Get();
		if (loadedSubclass == nullptr)
		{
			loadedSubclass = Cast<UClass>(SynchronousLoadAsset(assetPath));
		}
	}

	return loadedSubclass;
}

template <typename DataClass>
const DataClass& UBOTWAssetManager::GetOrLoadTypedData(const TSoftObjectPtr<DataClass>& _DataPath)
{
	const UPrimaryDataAsset* const* result = DataMap.Find(DataClass::StaticClass());
	if (result)
	{
		return *Cast<DataClass>(*result);
	}

	return *Cast<const DataClass>(LoadDataOfClass(DataClass::StaticClass(), _DataPath, DataClass::StaticClass()->GetFName()));
}
