#pragma once

#include "GameplayTagContainer.h"

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "BOTWInputConfig.generated.h"

class UInputAction;
class UInputMappingContext;

USTRUCT(BlueprintType)
struct FBOTWInputAction
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly)
	const UInputAction* InputAction;

	UPROPERTY(EditDefaultsOnly, Meta = (Categories = "InputTag"))
	FGameplayTag InputTag;

	FBOTWInputAction()
		: InputAction(nullptr)
	{ }
};

UCLASS(BlueprintType, Const)
class BOTW_API UBOTWInputConfig : public UDataAsset
{
	GENERATED_BODY()
	
public:
	UBOTWInputConfig(const FObjectInitializer& ObjectInitializer);

	const UInputAction* FindNativeInputActionForTag(const FGameplayTag& _InputTag) const;
	const UInputAction* FindAbilityInputActionForTag(const FGameplayTag& _InputTag) const;

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Meta = (TitleProperty = "InputAction"))
	TArray<FBOTWInputAction> NativeInputActions;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Meta = (TitleProperty = "InputAction"))
	TArray<FBOTWInputAction> AbilityInputActions;
};
