#pragma once

#include "Engine/DataTable.h"
#include "Library/LocomotionEnumLibrary.h"
#include "Library/LocomotionStructLibrary.h"
#include "Library/GASLibrary.h"

#include "Character/Player/Camera/BOTWCameraAnimInstance.h"

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "LocomotionComponent.generated.h"

class ACharacterBase;
class UBOTWCharacterMovementComponent;
class UCharacterAttributeSetBase;
class UClimbingComponent;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class BOTW_API ULocomotionComponent : public UActorComponent
{
	GENERATED_BODY()

private:
	UPROPERTY()
	UBOTWCameraAnimInstance* CameraAnimInstance;

	UPROPERTY()
	ACharacterBase* Character;

	UPROPERTY()
	UClimbingComponent* ClimbingComponent;

	UPROPERTY()
	UBOTWCharacterMovementComponent* CharacterMovementComponent;

	UPROPERTY()
	const UCharacterAttributeSetBase* CharacterAttributeSet;

private:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BOTW|Movement System", meta = (AllowPrivateAccess = "true"))
	FDataTableRowHandle MovementModel;

private:
	FVector Acceleration;
	bool IsMoving;
	bool HasMovementInput;
	FRotator LastVelocityRotation;
	FRotator LastMovementInputRotation;
	float Speed;
	float MovementInputAmount;
	float AimYawRate;
	float EasedMaxAcceleration;
	FVector CurrentAcceleration;
	FRotator ControlRotation;

private:
	ERotationMode DesiredRotationMode;
	EGait DesiredGait;
	EStance DesiredStance;
	EGroundedEntryState GroundedEntryState;
	EMovementState MovementState;
	EMovementState PrevMovementState;
	EMovementAction MovementAction;
	ERotationMode RotationMode;
	EGait Gait;
	EStance Stance;
	
	FMovementStateSettings MovementData;

	FRotator TargetRotation;
	FRotator InAirRotation;

	float YawOffset;

	FVector PreviousVelocity;
	float PreviousAimYaw;	

	FTimerHandle OnLandedFrictionResetTimer;

	FRotator AimingRotation;

private:
	UPROPERTY()
	ACharacterBase* LockOnTarget;

public:	
	ULocomotionComponent();

protected:
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

public:
	void MoveForward(float _Value);
	void MoveSide(float _Value);

public:
	void OnMovementStateChanged(EMovementState _PreviousState);
	void OnMovementActionChanged(EMovementAction _PreviousAction);
	void OnStanceChanged(EStance _PreviousStance);
	void OnRotationModeChanged(ERotationMode _PreviousRotationMode);
	void OnGaitChanged(EGait _PreviousGait);
	void OnJumped();
	void OnLanded(const FHitResult& _HitResult);

	void OnLandFrictionReset();

public:
	void SetEssentialValues(float _DeltaTime);
	void UpdateCharacterMovement();
	void UpdateGroundedRotation(float _DeltaTime);
	void UpdateInAirRotation(float _DeltaTime);

	void SmoothCharacterRotation(FRotator _Target, float _TargetInterpSpeed, float _ActorInterpSpeed, float _DeltaTime);
	float CalculateGroundedRotationRate() const;
	void LimitRotation(float _AimYawMin, float _AimYawMax, float _InterpSpeed, float _DeltaTime);
	void SetMovementModel();
	void ForceUpdateCharacterState();

private:
	EGait GetAllowedGait() const;
	EGait GetActualGait(EGait _AllowedGait) const;

public:
	void Sprint(bool _SprintInput);
	bool CanSprint() const;

private:
	void LockOn(ACharacterBase* _LockOnTarget);

private:
	float GetAnimCurveValue(FName _CurveName) const;
	FMovementSettings GetTargetMovementSettings() const;

public:
	void SetMovementState(EMovementState _NewState, bool _IsForce = false);
	void SetMovementAction(EMovementAction _NewAction, bool _IsForce = false);
	void SetStance(EStance _NewStance, bool _IsForce = false);
	void SetDesiredStance(EStance _NewStance);
	void SetGait(EGait _NewGait, bool _IsForce = false);
	void SetDesiredGait(EGait _NewGait);
	void SetDesiredRotationMode(ERotationMode _NewRotMode);
	void SetRotationMode(ERotationMode _NewRotationMode, bool _IsForce = false);
	void SetGroundedEntryState(EGroundedEntryState _NewState);

	void SetCameraAnimInstance(UBOTWCameraAnimInstance* _CameraAnimInstance) { CameraAnimInstance = _CameraAnimInstance; }

public:
	FORCEINLINE EMovementState GetMovementState() const { return MovementState; }
	FORCEINLINE EMovementState GetPrevMovementState() const { return PrevMovementState; }
	FORCEINLINE EMovementAction GetMovementAction() const { return MovementAction; }
	FORCEINLINE EStance GetStance() const { return Stance; }
	FORCEINLINE EStance GetDesiredStance() const { return DesiredStance; }
	FORCEINLINE EGait GetGait() const { return Gait; }
	FORCEINLINE EGait GetDesiredGait() const { return DesiredGait; }
	FORCEINLINE ERotationMode GetRotationMode() const { return RotationMode; }
	FORCEINLINE ERotationMode GetDesiredRotationMode() const { return DesiredRotationMode; }
	FORCEINLINE EGroundedEntryState GetGroundedEntryState() const { return GroundedEntryState; }
	
	FORCEINLINE FVector GetAcceleration() const { return Acceleration; }
	FORCEINLINE bool GetIsMoving() const { return IsMoving; }
	FORCEINLINE float GetMovementInputAmount() const { return MovementInputAmount; }
	FORCEINLINE float GetSpeed() const { return Speed; }
	FORCEINLINE FRotator GetAimingRotation() const { return AimingRotation; }
	FORCEINLINE float GetAimYawRate() const { return AimYawRate; }
	FORCEINLINE bool GetHasMovementInput() const { return HasMovementInput; }

	FORCEINLINE FVector GetMovementInput() const { return CurrentAcceleration; }
};
