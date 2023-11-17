#include "BOTWCameraAnimInstance.h"

UBOTWCameraAnimInstance::UBOTWCameraAnimInstance()
	: IsVelocityDirection(false)
	, IsAiming(false)
	, IsLockOn(false)
{

}

void UBOTWCameraAnimInstance::SetRotationMode(ERotationMode RotationMode)
{
	IsVelocityDirection = RotationMode == ERotationMode::VelocityDirection;
	IsAiming = RotationMode == ERotationMode::Aiming;
}

void UBOTWCameraAnimInstance::SetMovementState(EMovementState _MovementState)
{
	MovementState = _MovementState;
}

void UBOTWCameraAnimInstance::SetMovementAction(EMovementAction _MovementAction)
{
	MovementAction = _MovementAction;
}

void UBOTWCameraAnimInstance::SetIsVelocityDirection(bool _IsVelocityDirection)
{
	IsVelocityDirection = _IsVelocityDirection;
}

void UBOTWCameraAnimInstance::SetIsAiming(bool _IsAiming)
{
	IsAiming = _IsAiming;
}

void UBOTWCameraAnimInstance::SetGait(EGait _Gait)
{
	Gait = _Gait;
}

void UBOTWCameraAnimInstance::SetStance(EStance _Stance)
{
	Stance = _Stance;
}