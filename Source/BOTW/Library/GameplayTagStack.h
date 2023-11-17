#pragma once

#include "GameplayTagContainer.h"

#include "CoreMinimal.h"
#include "GameplayTagStack.generated.h"

struct FGameplayTagStackContainer;

USTRUCT(BlueprintType)
struct FGameplayTagStack
{
	GENERATED_BODY()

private:
	friend FGameplayTagStackContainer;

	FGameplayTag Tag;
	int32 StackCount;
	
public:
	FGameplayTagStack()
		: StackCount(0)
	{
	}

	FGameplayTagStack(FGameplayTag _Tag, int32 _StackCount)
		: Tag(_Tag)
		, StackCount(_StackCount)
	{
	}
};

USTRUCT(BlueprintType)
struct FGameplayTagStackContainer
{
	GENERATED_BODY()

private:
	UPROPERTY()
	TArray<FGameplayTagStack> TagStacks;

	TMap<FGameplayTag, int32> TagCountMap;

public:
	void AddStack(FGameplayTag _Tag, int32 _StackCount);
	void RemoveStack(FGameplayTag _Tag, int32 _StackCount);

	int32 GetStackCount(FGameplayTag _Tag) const;
	bool ContainsTag(FGameplayTag _Tag) const;
};