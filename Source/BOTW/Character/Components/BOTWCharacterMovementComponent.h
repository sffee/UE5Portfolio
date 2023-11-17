#pragma once

#include "Library/LocomotionStructLibrary.h"
#include "Library/GASLibrary.h"

#include "CoreMinimal.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "BOTWCharacterMovementComponent.generated.h"

class ACharacterBase;
class UAbilitySystemComponent;
class ULocomotionComponent;
class UCharacterAttributeSetBase;
class UClimbingComponent;

UENUM(BlueprintType)
namespace ECustomMovementMode
{
	enum Type
	{
		MOVE_Climb
	};
}

UCLASS()
class BOTW_API UBOTWCharacterMovementComponent : public UCharacterMovementComponent
{
	GENERATED_BODY()
	
private:
	UPROPERTY()
	ACharacterBase* Character;

	UPROPERTY()
	UAbilitySystemComponent* AbilitySystemComponent;

	UPROPERTY()
	ULocomotionComponent* LocomotionComponent;

	UPROPERTY()
	const UCharacterAttributeSetBase* CharacterAttributeSet;

	UPROPERTY()
	UClimbingComponent* ClimbingComponent;

private:
	FMovementSettings CurrentMovementSettings;

public:
	virtual void BeginPlay() override;

public:
	virtual float GetMaxAcceleration() const override;
	virtual float GetMaxBrakingDeceleration() const override;
	virtual float GetMaxSpeed() const override;
	virtual void OnMovementModeChanged(EMovementMode PreviousMovementMode, uint8 PreviousCustomMode) override;
	virtual void PhysCustom(float deltaTime, int32 Iterations) override;

public:
	float GetMappedSpeed() const;

private:
	void SnapMovementToClimableSurfaces(float DeltaTime, const FVector& _SurfaceLocation, const FVector& _SurfaceNormal);
	FQuat GetClimbRotation(float DeltaTime, const FVector& _SurfaceNormal);

public:
	void SetMovementSettings(FMovementSettings _NewMovementSettings);

public:
	FMovementSettings GetCurrentMovementSettings() const { return CurrentMovementSettings; }
};
