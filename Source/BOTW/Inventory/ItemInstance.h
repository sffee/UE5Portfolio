#pragma once

#include "CoreMinimal.h"

#include "Library/GameplayTagStack.h"
#include "UObject/NoExportTypes.h"
#include "ItemInstance.generated.h"

class UItemDefinition;

UCLASS()
class BOTW_API UItemInstance : public UObject
{
	GENERATED_BODY()
	
public:
	UItemInstance();

public:
	FGameplayTagStackContainer StatTags;

	UPROPERTY()
	TSubclassOf<UItemDefinition> ItemDefinition;

	UPROPERTY()
	int32 ItemStack;
};
