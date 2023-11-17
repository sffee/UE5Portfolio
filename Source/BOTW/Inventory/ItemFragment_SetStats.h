#pragma once

#include "GameplayTagContainer.h"

#include "CoreMinimal.h"
#include "ItemDefinition.h"
#include "ItemFragment_SetStats.generated.h"

class UItemInstance;

UCLASS()
class BOTW_API UItemFragment_SetStats : public UItemFragment
{
	GENERATED_BODY()
	
protected:
	UPROPERTY(EditDefaultsOnly, Category = "Equipment")
	TMap<FGameplayTag, int32> InitStats;

public:
	virtual void OnInstanceCreated(UItemInstance* _Instance) const override;

public:
	int32 GetStatByTag(FGameplayTag _Tag);
};
