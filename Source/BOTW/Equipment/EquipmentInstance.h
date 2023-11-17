#pragma once

#include "Library/GameplayTagStack.h"
#include "Library/EquipmentEnumLibrary.h"

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "EquipmentInstance.generated.h"

struct FEquipmentActorToSpawn;
class AEquipmentActor;

UCLASS(BlueprintType, Blueprintable)
class BOTW_API UEquipmentInstance : public UObject
{
	GENERATED_BODY()
	
private:
	UPROPERTY()
	UObject* Instigator;

	UPROPERTY()
	AEquipmentActor* SpawnedActor;

	UPROPERTY(EditDefaultsOnly, Category = "Type", meta = (DisplayPriority = 1))
	EEquipmentType EquipmentType;
	
public:
	UEquipmentInstance(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	virtual void SpawnEquipmentActor(const FEquipmentActorToSpawn& _ActorToSpawn);
	virtual void DestroyEquipmentActor();

	virtual void OnEquipped();
	virtual void OnUnEquipped();

public:
	void SetInstigator(UObject* _Instigator);

public:
	FORCEINLINE EEquipmentType GetEquipmentType() const { return EquipmentType; }
	FORCEINLINE AEquipmentActor* GetSpawnedActor() const { return SpawnedActor; }
};
