#pragma once

#include "Library/LocomotionEnumLibrary.h"

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "BOTWCameraAnimInstance.generated.h"

class ACharacterBase;

UCLASS()
class BOTW_API UBOTWCameraAnimInstance : public UAnimInstance
{
	GENERATED_BODY()
	
private:
	UPROPERTY(BlueprintReadOnly, Category = "BOTW|Character Information", meta = (AllowPrivateAccess = true))
	EMovementState MovementState;

	UPROPERTY(BlueprintReadOnly, Category = "BOTW|Character Information", meta = (AllowPrivateAccess = true))
	EMovementAction MovementAction;

	UPROPERTY(BlueprintReadOnly, Category = "BOTW|Character Information", meta = (AllowPrivateAccess = true))
	bool IsVelocityDirection;

	UPROPERTY(BlueprintReadOnly, Category = "BOTW|Character Information", meta = (AllowPrivateAccess = true))
	bool IsAiming;

	UPROPERTY(BlueprintReadOnly, Category = "BOTW|Character Information", meta = (AllowPrivateAccess = true))
	EGait Gait;

	UPROPERTY(BlueprintReadOnly, Category = "BOTW|Character Information", meta = (AllowPrivateAccess = true))
	EStance Stance;

	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	bool IsLockOn;

public:
	UBOTWCameraAnimInstance();

public:
	void SetRotationMode(ERotationMode RotationMode);
	void SetMovementState(EMovementState _MovementState);
	void SetMovementAction(EMovementAction _MovementAction);
	void SetIsVelocityDirection(bool _IsVelocityDirection);
	void SetIsAiming(bool _IsAiming);
	void SetGait(EGait _Gait);
	void SetStance(EStance _Stance);
	
	FORCEINLINE void SetIsLockOn(bool _IsLockOn) { IsLockOn = _IsLockOn; }
};
