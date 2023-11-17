#pragma once

#include "GameplayTagContainer.h"
#include "Engine/DataAsset.h"

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "EquipmentDefinition.generated.h"

class AEquipmentActor;
class UEquipmentInstance;
class UBOTWAbilitySet;

USTRUCT()
struct FEquipmentActorToSpawn
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, Category = "Equipment")
	TSubclassOf<AEquipmentActor> ActorToSpawn;

	UPROPERTY(EditDefaultsOnly, Category = "Equipment")
	FName AttachSocket;

	UPROPERTY(EditDefaultsOnly, Category = "Equipment")
	FTransform AttachTransform;
};

UCLASS(BlueprintType, Blueprintable, Const, Abstract)
class BOTW_API UEquipmentDefinition : public UObject
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditDefaultsOnly, Category = "Equipment")
	TSubclassOf<UEquipmentInstance> InstanceType;

	UPROPERTY(EditDefaultsOnly, Category = "Equipment")
	TArray<const UBOTWAbilitySet*> AbilitySetsToGrant;

	UPROPERTY(EditDefaultsOnly, Category = "Equipment")
	FEquipmentActorToSpawn ActorToSpawn;
};
