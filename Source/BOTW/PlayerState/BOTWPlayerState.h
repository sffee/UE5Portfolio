#pragma once

#include "Library/GASLibrary.h"

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "BOTWPlayerState.generated.h"

class UCharacterAttributeSetBase;

UCLASS()
class BOTW_API ABOTWPlayerState : public APlayerState, public IAbilitySystemInterface
{
	GENERATED_BODY()
	
private:
	UPROPERTY()
	UBOTWAbilitySystemComponent* AbilitySystemComponent;

	UPROPERTY()
	UCharacterAttributeSetBase* AttributeSet;

public:
	ABOTWPlayerState();

public:
	FORCEINLINE virtual UBOTWAbilitySystemComponent* GetAbilitySystemComponent() const override { return AbilitySystemComponent; };
	FORCEINLINE UCharacterAttributeSetBase* GetAttributeSet() const { return AttributeSet; }
};
