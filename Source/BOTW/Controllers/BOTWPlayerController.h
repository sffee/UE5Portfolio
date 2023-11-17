#pragma once

#include "Library/GASLibrary.h"

#include "CoreMinimal.h"
#include "InputActionValue.h"
#include "Inventory/InventoryComponent.h"
#include "GameFramework/PlayerController.h"
#include "BOTWPlayerController.generated.h"

class UInputMappingContext;
class APlayerCharacter;
class UBOTWInputConfig;
class UBOTWUserWidgetBase;

UCLASS()
class BOTW_API ABOTWPlayerController : public APlayerController
{
	GENERATED_UCLASS_BODY()

private:
	UPROPERTY()
	APlayerCharacter* PossessedPlayer;

	UPROPERTY()
	UBOTWAbilitySystemComponent* AbilitySystemComponent;

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UInventoryComponent* Inventory;

protected:
	UPROPERTY(EditDefaultsOnly, Category = "BOTW|Input")
	UBOTWInputConfig* InputConfig;

	UPROPERTY(EditDefaultsOnly, Category = "BOTW|Input")
	UInputMappingContext* InputMappingContext;

	TArray<uint32> BindHandles;

protected:
	UPROPERTY(EditDefaultsOnly, Category = "BOTW|Widget")
	TArray<FName> BaseWidgetNames;

public:
	virtual void OnPossess(APawn* NewPawn) override;
	virtual void SetupInputComponent() override;
	virtual void PostProcessInput(const float DeltaTime, const bool bGamePaused) override;

public:
	void SetInputLock_Look(bool bLock) { bInputLock_Look = bLock; }

private:
	void BindActions();
	void SetupInputs();
	void SetupWidget();

	void SetupCamera();

protected:
	void AbilityInputTagPressed(FGameplayTag _InputTag);
	void AbilityInputTagReleased(FGameplayTag _InputTag);

	UFUNCTION()
	void MoveForward(const FInputActionValue& _Value);

	UFUNCTION()
	void MoveSide(const FInputActionValue& _Value);

	UFUNCTION()
	void LookUp(const FInputActionValue& _Value);

	UFUNCTION()
	void LookAround(const FInputActionValue& _Value);

	UFUNCTION()
	void TestKey(const FInputActionValue& _Value);
};
