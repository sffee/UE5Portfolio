#pragma once

#include "CoreMinimal.h"

#include "LocomotionEnumLibrary.generated.h"

UENUM(BlueprintType)
enum class EMovementState : uint8
{
	None,
	Grounded,
	InAir,
	Climbing
};

UENUM(BlueprintType)
enum class EMovementAction : uint8
{
	None
};

UENUM(BlueprintType)
enum class EStance : uint8
{
	Standing,
	Crouching
};

UENUM(BlueprintType)
enum class EGait : uint8
{
	Walking,
	Running,
	Sprinting
};

UENUM(BlueprintType)
enum class ERotationMode : uint8
{
	VelocityDirection,
	LookingDirection,
	LockOn,
	Aiming
};

UENUM(BlueprintType)
enum class EGroundedEntryState : uint8
{
	None
};

UENUM(BlueprintType)
enum class EHipsDirection : uint8
{
	F,
	B,
	RF,
	RB,
	LF,
	LB
};

UENUM(BlueprintType)
enum class EMovementDirection : uint8
{
	Forward,
	Right,
	Left,
	Backward
};