#pragma once

#include "Abilities/AttributeSets/CharacterAttributeSetBase.h"

#include "CoreMinimal.h"
#include "InputActionValue.h"
#include "../CharacterBase.h"
#include "Controllers/BOTWPlayerController.h"
#include "SheikahRune/RuneAbilityStateEnum.h"
#include "PlayerCharacter.generated.h"

class UInteractionComponent;
class UBOTWStatusComponent;
class UClimbingComponent;

UCLASS()
class BOTW_API APlayerCharacter : public ACharacterBase
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintPure)
	UInteractionComponent* GetInteractionComponent() { return InteractionComponent; }

private:
	UPROPERTY(EditDefaultsOnly)
	UInteractionComponent* InteractionComponent;

	UPROPERTY()
	UBOTWStatusComponent* StatusComponent;

	UPROPERTY(EditAnywhere)
	UClimbingComponent* ClimbingComponent;

private:
	UPROPERTY(EditDefaultsOnly, Category = "BOTW|Input")
	float LookUpDownRate;

	UPROPERTY(EditDefaultsOnly, Category = "BOTW|Input")
	float LookAroundRate;

private:
	bool IsBlockMoveInput;

public:
	APlayerCharacter(const class FObjectInitializer& ObjectInitializer);

	virtual void BeginPlay() override;
	virtual void PossessedBy(AController* NewController) override;
	virtual void UnPossessed() override;

	virtual void Tick(float DeltaTime) override;
	virtual void PostInitializeComponents() override;

private:
	void InitAbilitySystem();

public:
	void MoveForward(float _Value);
	void MoveSide(float _Value);

	void LookUp(float _Value);
	void LookAround(float _Value);

private:
	bool Sprint(bool _IsSprinting);

private:
	void OnChangeBlockIMovenputTag(const FGameplayTag _CallbackTag, int32 _NewCount);
};
