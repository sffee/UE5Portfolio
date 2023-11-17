#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "PhysicalMaterials/PhysicalMaterial.h"
#include "Materials/MaterialInterface.h"
#include "LocomotionEnumLibrary.h"

#include "LocomotionStructLibrary.generated.h"

class UCurveFloat;
class UCurveVector;
class UAnimSequenceBase;

USTRUCT(BlueprintType)
struct FMovementSettings
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = "Movement Settings")
	TObjectPtr<UCurveVector> MovementCurve;

	UPROPERTY(EditAnywhere, Category = "Movement Settings")
	TObjectPtr<UCurveFloat> RotationRateCurve;

	FMovementSettings()
		: MovementCurve(nullptr)
		, RotationRateCurve(nullptr)
	{ }
};

USTRUCT(BlueprintType)
struct FMovementStanceSettings
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = "Movement Settings")
	FMovementSettings Standing;

	UPROPERTY(EditAnywhere, Category = "Movement Settings")
	FMovementSettings Crouching;
};

USTRUCT(BlueprintType)
struct FMovementStateSettings : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = "Movement Settings")
	FMovementStanceSettings VelocityDirection;

	UPROPERTY(EditAnywhere, Category = "Movement Settings")
	FMovementStanceSettings Aiming;
};

USTRUCT(BlueprintType)
struct FDynamicMontageParams
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "BOTW|Dynamic Transition")
	UAnimSequenceBase* Animation;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "BOTW|Dynamic Transition")
	float BlendInTime;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "BOTW|Dynamic Transition")
	float BlendOutTime;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "BOTW|Dynamic Transition")
	float PlayRate;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "BOTW|Dynamic Transition")
	float StartTime;

	FDynamicMontageParams()
		: Animation(nullptr)
		, BlendInTime(0.f)
		, BlendOutTime(0.f)
		, PlayRate(0.f)
		, StartTime(0.f)
	{ }
};

USTRUCT(BlueprintType)
struct FVelocityBlend
{
	GENERATED_BODY()

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "BOTW|Anim Graph - Grounded")
	float F;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "BOTW|Anim Graph - Grounded")
	float B;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "BOTW|Anim Graph - Grounded")
	float L;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "BOTW|Anim Graph - Grounded")
	float R;

	FVelocityBlend()
		: F(0.f)
		, B(0.f)
		, L(0.f)
		, R(0.f)
	{ }
};

USTRUCT(BlueprintType)
struct FLeanAmount
{
	GENERATED_BODY()

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "BOTW|Anim Graph - Grounded")
	float LR;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "BOTW|Anim Graph - Grounded")
	float FB;

	FLeanAmount()
		: LR(0.f)
		, FB(0.f)
	{ }
};

USTRUCT(BlueprintType)
struct FAnimCharacterInformation
{
	GENERATED_BODY()

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "BOTW|Character Information")
	FRotator AimingRotation;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "BOTW|Character Information")
	FRotator CharacterActorRotation;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "BOTW|Character Information")
	FVector Velocity;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "BOTW|Character Information")
	FVector ClimbVelocity;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "BOTW|Character Information")
	FVector RelativeVelocityDirection;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "BOTW|Character Information")
	FVector Acceleration;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "BOTW|Character Information")
	FVector MovementInput;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "BOTW|Character Information")
	bool IsMoving;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "BOTW|Character Information")
	bool HasMovementInput;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "BOTW|Character Information")
	float Speed;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "BOTW|Character Information")
	float MovementInputAmount;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "BOTW|Character Information")
	float AimYawRate;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "BOTW|Character Information")
	float ZoomAmount;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "BOTW|Character Information")
	EMovementState PrevMovementState;

	FAnimCharacterInformation()
		: IsMoving(false)
		, HasMovementInput(false)
		, Speed(0.f)
		, MovementInputAmount(0.f)
		, AimYawRate(0.f)
		, ZoomAmount(0.f)
		, PrevMovementState(EMovementState::None)
	{ }
};

USTRUCT(BlueprintType)
struct FMovementState
{
	GENERATED_BODY()

private:
	UPROPERTY(BlueprintReadOnly, Category = "BOTW|Movement System", meta = (AllowPrivateAccess = true))
	EMovementState State;

	UPROPERTY(BlueprintReadOnly, Category = "BOTW|Movement System", meta = (AllowPrivateAccess = true))
	bool None_;

	UPROPERTY(BlueprintReadOnly, Category = "BOTW|Movement System", meta = (AllowPrivateAccess = true))
	bool Grounded_;

	UPROPERTY(BlueprintReadOnly, Category = "BOTW|Movement System", meta = (AllowPrivateAccess = true))
	bool InAir_;

	UPROPERTY(BlueprintReadOnly, Category = "BOTW|Movement System", meta = (AllowPrivateAccess = true))
	bool Climbing_;

public:
	FMovementState()
		: State(EMovementState::None)
		, None_(true)
		, Grounded_(false)
		, InAir_(false)
		, Climbing_(false)
	{
	}

	FMovementState(const EMovementState _InitialState) { *this = _InitialState; }

	const bool& None() const { return None_; }
	const bool& Grounded() const { return Grounded_; }
	const bool& InAir() const { return InAir_; }
	const bool& Climbing() const { return Climbing_; }

	operator EMovementState() const { return State; }

	void operator=(const EMovementState _NewState)
	{
		State = _NewState;
		None_ = State == EMovementState::None;
		Grounded_ = State == EMovementState::Grounded;
		InAir_ = State == EMovementState::InAir;
		Climbing_ = State == EMovementState::Climbing;
	}
};

USTRUCT(BlueprintType)
struct FMovementAction
{
	GENERATED_BODY()

private:
	UPROPERTY(BlueprintReadOnly, Category = "BOTW|Movement System", meta = (AllowPrivateAccess = true))
	EMovementAction Action;

	UPROPERTY(BlueprintReadOnly, Category = "BOTW|Movement System", meta = (AllowPrivateAccess = true))
	bool None_;

public:
	FMovementAction()
		: Action(EMovementAction::None)
		, None_(true)
	{
	}

	FMovementAction(const EMovementAction _InitialAction) { *this = _InitialAction; }

	const bool& None() const { return None_; }

	operator EMovementAction() const { return Action; }

	void operator=(const EMovementAction _NewAction)
	{
		Action = _NewAction;
		None_ = Action == EMovementAction::None;
	}
};

USTRUCT(BlueprintType)
struct FRotationMode
{
	GENERATED_BODY()

private:
	UPROPERTY(BlueprintReadOnly, Category = "BOTW|Rotation System", meta = (AllowPrivateAccess = true))
	ERotationMode RotationMode;

	UPROPERTY(BlueprintReadOnly, Category = "BOTW|Rotation System", meta = (AllowPrivateAccess = true))
	bool VelocityDirection_;

	UPROPERTY(BlueprintReadOnly, Category = "BOTW|Rotation System", meta = (AllowPrivateAccess = true))
	bool LockOn_;

	UPROPERTY(BlueprintReadOnly, Category = "BOTW|Rotation System", meta = (AllowPrivateAccess = true))
	bool Aiming_;

public:
	FRotationMode()
		: RotationMode(ERotationMode::VelocityDirection)
		, VelocityDirection_(true)
		, LockOn_(false)
		, Aiming_(false)
	{
	}

	FRotationMode(const ERotationMode _InitialRotationMode) { *this = _InitialRotationMode; }

	const bool& VelocityDirection() const { return VelocityDirection_; }
	const bool& LockOn() const { return LockOn_; }
	const bool& Aiming() const { return Aiming_; }

	operator ERotationMode() const { return RotationMode; }

	void operator=(const ERotationMode _NewRotationMode)
	{
		RotationMode = _NewRotationMode;
		VelocityDirection_ = RotationMode == ERotationMode::VelocityDirection;
		LockOn_ = RotationMode == ERotationMode::LockOn;
		Aiming_ = RotationMode == ERotationMode::Aiming;
	}
};

USTRUCT(BlueprintType)
struct FGait
{
	GENERATED_BODY()

private:
	UPROPERTY(BlueprintReadOnly, Category = "BOTW|Movement System", meta = (AllowPrivateAccess = true))
	EGait Gait;

	UPROPERTY(BlueprintReadOnly, Category = "BOTW|Movement System", meta = (AllowPrivateAccess = true))
	bool Walking_;

	UPROPERTY(BlueprintReadOnly, Category = "BOTW|Movement System", meta = (AllowPrivateAccess = true))
	bool Running_;

	UPROPERTY(BlueprintReadOnly, Category = "BOTW|Movement System", meta = (AllowPrivateAccess = true))
	bool Sprinting_;

public:
	FGait()
		: Gait(EGait::Walking)
		, Walking_(true)
		, Running_(false)
		, Sprinting_(false)
	{
	}

	FGait(const EGait _InitialGait) { *this = _InitialGait; }

	const bool& Walking() const { return Walking_; }
	const bool& Running() const { return Running_; }
	const bool& Sprinting() const { return Sprinting_; }

	operator EGait() const { return Gait; }

	void operator=(const EGait _NewGait)
	{
		Gait = _NewGait;
		Walking_ = Gait == EGait::Walking;
		Running_ = Gait == EGait::Running;
		Sprinting_ = Gait == EGait::Sprinting;
	}
};

USTRUCT(BlueprintType)
struct FStance
{
	GENERATED_BODY()

private:
	UPROPERTY(BlueprintReadOnly, Category = "BOTW|Character States", meta = (AllowPrivateAccess = true))
	EStance Stance;

	UPROPERTY(BlueprintReadOnly, Category = "BOTW|Character States", meta = (AllowPrivateAccess = true))
	bool Standing_;

	UPROPERTY(BlueprintReadOnly, Category = "BOTW|Character States", meta = (AllowPrivateAccess = true))
	bool Crouching_;

public:
	FStance()
		: Stance(EStance::Standing)
		, Standing_(true)
		, Crouching_(false)
	{
	}

	FStance(const EStance _InitialStance) { *this = _InitialStance; }

	const bool& Standing() const { return Standing_; }
	const bool& Crouching() const { return Crouching_; }

	operator EStance() const { return Stance; }

	void operator=(const EStance _NewStance)
	{
		Stance = _NewStance;
		Standing_ = Stance == EStance::Standing;
		Crouching_ = Stance == EStance::Crouching;
	}
};

USTRUCT(BlueprintType)
struct FAnimGraphGrounded
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, Category = "BOTW|Anim Graph - Grounded")
	EHipsDirection TrackedHipsDirection;

	UPROPERTY(BlueprintReadWrite, Category = "BOTW|Anim Graph - Grounded")
	bool IsShouldMove;

	UPROPERTY(BlueprintReadOnly, Category = "BOTW|Movement System")
	bool IsRotateL;

	UPROPERTY(BlueprintReadOnly, Category = "BOTW|Movement System")
	bool IsRotateR;

	UPROPERTY(BlueprintReadWrite, Category = "BOTW|Anim Graph - Grounded")
	bool IsPivot;

	UPROPERTY(BlueprintReadOnly, Category = "BOTW|Movement System")
	float RotateRate;

	UPROPERTY(BlueprintReadOnly, Category = "BOTW|Movement System")
	float RotationScale;

	UPROPERTY(BlueprintReadOnly, Category = "BOTW|Movement System")
	float DiagonalScaleAmount;

	UPROPERTY(BlueprintReadOnly, Category = "BOTW|Movement System")
	float WalkRunBlend;

	UPROPERTY(BlueprintReadOnly, Category = "BOTW|Movement System")
	float StandingPlayRate;

	UPROPERTY(BlueprintReadOnly, Category = "BOTW|Movement System")
	float CrouchingPlayRate;

	UPROPERTY(BlueprintReadOnly, Category = "BOTW|Movement System")
	float StrideBlend;

	UPROPERTY(BlueprintReadOnly, Category = "BOTW|Movement System")
	float FYaw;

	UPROPERTY(BlueprintReadOnly, Category = "BOTW|Movement System")
	float BYaw;

	UPROPERTY(BlueprintReadOnly, Category = "BOTW|Movement System")
	float LYaw;

	UPROPERTY(BlueprintReadOnly, Category = "BOTW|Movement System")
	float RYaw;

	FAnimGraphGrounded()
		: TrackedHipsDirection(EHipsDirection::F)
		, IsShouldMove(false)
		, IsRotateL(false)
		, IsRotateR(false)
		, IsPivot(false)
		, RotateRate(1.f)
		, RotationScale(0.f)
		, DiagonalScaleAmount(0.f)
		, WalkRunBlend(0.f)
		, StandingPlayRate(1.f)
		, CrouchingPlayRate(1.f)
		, StrideBlend(0.f)
		, FYaw(0.f)
		, BYaw(0.f)
		, LYaw(0.f)
		, RYaw(0.f)
	{ }
};

USTRUCT(BlueprintType)
struct FGroundedEntryState
{
	GENERATED_BODY()

private:
	UPROPERTY(BlueprintReadOnly, Category = "BOTW|Breakfall System", meta = (AllowPrivateAccess = true))
	EGroundedEntryState State;

	UPROPERTY(BlueprintReadOnly, Category = "BOTW|Breakfall System", meta = (AllowPrivateAccess = true))
	bool None_;

public:
	FGroundedEntryState()
		: State(EGroundedEntryState::None)
		, None_(true)
	{
	}

	FGroundedEntryState(const EGroundedEntryState _InitialState) { *this = _InitialState; }

	const bool& None() const { return None_; }

	operator EGroundedEntryState() const { return State; }

	void operator=(const EGroundedEntryState _NewState)
	{
		State = _NewState;
		None_ = State == EGroundedEntryState::None;
	}
};

USTRUCT(BlueprintType)
struct FMovementDirection
{
	GENERATED_BODY()

private:
	UPROPERTY(BlueprintReadOnly, Category = "BOTW|Movement System", meta = (AllowPrivateAccess = true))
	EMovementDirection MovementDirection;

	UPROPERTY(BlueprintReadOnly, Category = "BOTW|Movement System", meta = (AllowPrivateAccess = true))
	bool Forward_;

	UPROPERTY(BlueprintReadOnly, Category = "BOTW|Movement System", meta = (AllowPrivateAccess = true))
	bool Right_;

	UPROPERTY(BlueprintReadOnly, Category = "BOTW|Movement System", meta = (AllowPrivateAccess = true))
	bool Left_;

	UPROPERTY(BlueprintReadOnly, Category = "BOTW|Movement System", meta = (AllowPrivateAccess = true))
	bool Backward_;

public:
	FMovementDirection()
		: MovementDirection(EMovementDirection::Forward)
		, Forward_(true)
		, Right_(false)
		, Left_(false)
		, Backward_(false)
	{
	}

	FMovementDirection(const EMovementDirection _InitialMovementDirection)
	{
		*this = _InitialMovementDirection;
	}

	const bool& Forward() const { return Forward_; }
	const bool& Right() const { return Right_; }
	const bool& Left() const { return Left_; }
	const bool& Backward() const { return Backward_; }

	operator EMovementDirection() const { return MovementDirection; }

	void operator=(const EMovementDirection _NewMovementDirection)
	{
		MovementDirection = _NewMovementDirection;
		Forward_ = MovementDirection == EMovementDirection::Forward;
		Right_ = MovementDirection == EMovementDirection::Right;
		Left_ = MovementDirection == EMovementDirection::Left;
		Backward_ = MovementDirection == EMovementDirection::Backward;
	}
};

USTRUCT(BlueprintType)
struct FAnimGraphInAir
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category = "BOTW|Anim Graph - In Air")
	bool IsJumped;

	UPROPERTY(BlueprintReadOnly, Category = "BOTW|Anim Graph - In Air")
	float JumpPlayRate;

	UPROPERTY(BlueprintReadOnly, Category = "BOTW|Anim Graph - In Air")
	float FallSpeed;

	UPROPERTY(BlueprintReadOnly, Category = "BOTW|Anim Graph - In Air")
	float LandPrediction;

	FAnimGraphInAir()
		: IsJumped(false)
		, JumpPlayRate(1.2f)
		, FallSpeed(0.f)
		, LandPrediction(1.f)
	{ }
};

USTRUCT(BlueprintType)
struct FAnimGraphAimingValues
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category = "BOTW|Anim Graph - Aiming Values")
	FRotator SmoothedAimingRotation;

	UPROPERTY(BlueprintReadOnly, Category = "BOTW|Anim Graph - Aiming Values")
	FRotator SpineRotation;

	UPROPERTY(BlueprintReadOnly, Category = "BOTW|Anim Graph - Aiming Values")
	FVector2D AimingAngle;

	UPROPERTY(BlueprintReadOnly, Category = "BOTW|Anim Graph - Aiming Values")
	float AimSweepTime;

	UPROPERTY(BlueprintReadOnly, Category = "BOTW|Anim Graph - Aiming Values")
	float InputYawOffsetTime;

	UPROPERTY(BlueprintReadOnly, Category = "BOTW|Anim Graph - Aiming Values")
	float ForwardYawTime;

	UPROPERTY(BlueprintReadOnly, Category = "BOTW|Anim Graph - Aiming Values")
	float LeftYawTime;

	UPROPERTY(BlueprintReadOnly, Category = "BOTW|Anim Graph - Aiming Values")
	float RightYawTime;

	FAnimGraphAimingValues()
		: AimSweepTime(0.f)
		, InputYawOffsetTime(0.f)
		, ForwardYawTime(0.f)
		, LeftYawTime(0.f)
		, RightYawTime(0.f)
	{ }
};

USTRUCT(BlueprintType)
struct FAnimGraphLayerBlending
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category = "BOTW|Anim Graph - Layer Blending")
	float EnableAimOffset;

	UPROPERTY(BlueprintReadOnly, Category = "BOTW|Anim Graph - Layer Blending")
	float BasePose_N;

	UPROPERTY(BlueprintReadOnly, Category = "BOTW|Anim Graph - Layer Blending")
	float BasePose_CLF;

	UPROPERTY(BlueprintReadOnly, Category = "BOTW|Anim Graph - Layer Blending")
	float Arm_L;

	UPROPERTY(BlueprintReadOnly, Category = "BOTW|Anim Graph - Layer Blending")
	float Arm_L_Add;

	UPROPERTY(BlueprintReadOnly, Category = "BOTW|Anim Graph - Layer Blending")
	float Arm_L_LS;

	UPROPERTY(BlueprintReadOnly, Category = "BOTW|Anim Graph - Layer Blending")
	float Arm_L_MS;

	UPROPERTY(BlueprintReadOnly, Category = "BOTW|Anim Graph - Layer Blending")
	float Arm_R;

	UPROPERTY(BlueprintReadOnly, Category = "BOTW|Anim Graph - Layer Blending")
	float Arm_R_Add;

	UPROPERTY(BlueprintReadOnly, Category = "BOTW|Anim Graph - Layer Blending")
	float Arm_R_LS;

	UPROPERTY(BlueprintReadOnly, Category = "BOTW|Anim Graph - Layer Blending")
	float Arm_R_MS;

	UPROPERTY(BlueprintReadOnly, Category = "BOTW|Anim Graph - Layer Blending")
	float Hand_L;

	UPROPERTY(BlueprintReadOnly, Category = "BOTW|Anim Graph - Layer Blending")
	float Hand_R;

	UPROPERTY(BlueprintReadOnly, Category = "BOTW|Anim Graph - Layer Blending")
	float Legs;

	UPROPERTY(BlueprintReadOnly, Category = "BOTW|Anim Graph - Layer Blending")
	float Legs_Add;

	UPROPERTY(BlueprintReadOnly, Category = "BOTW|Anim Graph - Layer Blending")
	float Pelvis;

	UPROPERTY(BlueprintReadOnly, Category = "BOTW|Anim Graph - Layer Blending")
	float Pelvis_Add;

	UPROPERTY(BlueprintReadOnly, Category = "BOTW|Anim Graph - Layer Blending")
	float Spine;

	UPROPERTY(BlueprintReadOnly, Category = "BOTW|Anim Graph - Layer Blending")
	float Spine_Add;

	UPROPERTY(BlueprintReadOnly, Category = "BOTW|Anim Graph - Layer Blending")
	float Head;

	UPROPERTY(BlueprintReadOnly, Category = "BOTW|Anim Graph - Layer Blending")
	float Head_Add;

	UPROPERTY(BlueprintReadOnly, Category = "BOTW|Anim Graph - Layer Blending")
	float EnableHandIK_L;

	UPROPERTY(BlueprintReadOnly, Category = "BOTW|Anim Graph - Layer Blending")
	float EnableHandIK_R;

	FAnimGraphLayerBlending()
		: EnableAimOffset(1.f)
		, BasePose_N(1.f)
		, BasePose_CLF(0.f)
		, Arm_L(0.f)
		, Arm_L_Add(0.f)
		, Arm_L_LS(0.f)
		, Arm_L_MS(0.f)
		, Arm_R(0.f)
		, Arm_R_Add(0.f)
		, Arm_R_LS(0.f)
		, Hand_L(0.f)
		, Hand_R(0.f)
		, Legs(0.f)
		, Legs_Add(0.f)
		, Pelvis(0.f)
		, Pelvis_Add(0.f)
		, Spine(0.f)
		, Spine_Add(0.f)
		, Head(0.f)
		, Head_Add(0.f)
		, EnableHandIK_L(1.f)
		, EnableHandIK_R(1.f)
	{ }
};

USTRUCT(BlueprintType)
struct FAnimGraphFootIK
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category = "BOTW|Anim Graph - Foot IK")
	float FootLock_L_Alpha;
	
	UPROPERTY(BlueprintReadOnly, Category = "BOTW|Anim Graph - Foot IK")
	float FootLock_R_Alpha;

	UPROPERTY(BlueprintReadOnly, Category = "BOTW|Anim Graph - Foot IK")
	bool UseFootLockCurve_L;

	UPROPERTY(BlueprintReadOnly, Category = "BOTW|Anim Graph - Foot IK")
	bool UseFootLockCurve_R;

	UPROPERTY(BlueprintReadOnly, Category = "BOTW|Anim Graph - Foot IK")
	FVector FootLock_L_Location;

	UPROPERTY(BlueprintReadOnly, Category = "BOTW|Anim Graph - Foot IK")
	FVector TargetFootLock_R_Location;

	UPROPERTY(BlueprintReadOnly, Category = "BOTW|Anim Graph - Foot IK")
	FVector FootLock_R_Location;

	UPROPERTY(BlueprintReadOnly, Category = "BOTW|Anim Graph - Foot IK")
	FRotator TargetFootLock_L_Rotation;

	UPROPERTY(BlueprintReadOnly, Category = "BOTW|Anim Graph - Foot IK")
	FRotator FootLock_L_Rotation;

	UPROPERTY(BlueprintReadOnly, Category = "BOTW|Anim Graph - Foot IK")
	FRotator TargetFootLock_R_Rotation;

	UPROPERTY(BlueprintReadOnly, Category = "BOTW|Anim Graph - Foot IK")
	FRotator FootLock_R_Rotation;

	UPROPERTY(BlueprintReadOnly, Category = "BOTW|Anim Graph - Foot IK")
	FVector FootOffset_L_Location;

	UPROPERTY(BlueprintReadOnly, Category = "BOTW|Anim Graph - Foot IK")
	FVector FootOffset_R_Location;

	UPROPERTY(BlueprintReadOnly, Category = "BOTW|Anim Graph - Foot IK")
	FRotator FootOffset_L_Rotation;

	UPROPERTY(BlueprintReadOnly, Category = "BOTW|Anim Graph - Foot IK")
	FRotator FootOffset_R_Rotation;

	UPROPERTY(BlueprintReadOnly, Category = "BOTW|Anim Graph - Foot IK")
	FVector PelvisOffset;

	UPROPERTY(BlueprintReadOnly, Category = "BOTW|Anim Graph - Foot IK")
	float PelvisAlpha;

	FAnimGraphFootIK()
		: FootLock_L_Alpha(0.f)
		, FootLock_R_Alpha(0.f)
		, UseFootLockCurve_L(false)
		, UseFootLockCurve_R(false)
		, PelvisAlpha(0.f)
	{ }
};

USTRUCT(BlueprintType)
struct FAnimRotateInPlace
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "BOTW|Rotate In Place")
	float RotateMinThreshold;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "BOTW|Rotate In Place")
	float RotateMaxThreshold;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "BOTW|Rotate In Place")
	float AimYawRateMinRange;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "BOTW|Rotate In Place")
	float AimYawRateMaxRange;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "BOTW|Rotate In Place")
	float MinPlayRate;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "BOTW|Rotate In Place")
	float MaxPlayRate;

	FAnimRotateInPlace()
		: RotateMinThreshold(-50.f)
		, RotateMaxThreshold(50.f)
		, AimYawRateMinRange(90.f)
		, AimYawRateMaxRange(270.f)
		, MinPlayRate(1.15f)
		, MaxPlayRate(3.f)
	{ }
};

USTRUCT(BlueprintType)
struct FAnimConfiguration
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "BOTW|Main Configuration")
	float VelocityBlendInterpSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "BOTW|Main Configuration")
	float GroundedLeanInterpSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "BOTW|Main Configuration")
	float InAirLeanInterpSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "BOTW|Main Configuration")
	float SmoothedAimingRotationInterpSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "BOTW|Main Configuration")
	float InputYawOffsetInterpSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "BOTW|Main Configuration")
	float TriggerPivotSpeedLimit;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "BOTW|Main Configuration")
	float FootHeight;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "BOTW|Main Configuration")
	float DynamicTransitionThreshold;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "BOTW|Main Configuration")
	float IK_TraceDistanceAboveFoot;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "BOTW|Main Configuration")
	float IK_TraceDistanceBelowFoot;

	FAnimConfiguration()
		: VelocityBlendInterpSpeed(12.f)
		, GroundedLeanInterpSpeed(4.f)
		, InAirLeanInterpSpeed(4.f)
		, SmoothedAimingRotationInterpSpeed(10.f)
		, InputYawOffsetInterpSpeed(8.f)
		, TriggerPivotSpeedLimit(200.f)
		, FootHeight(13.5f)
		, DynamicTransitionThreshold(8.f)
		, IK_TraceDistanceAboveFoot(50.f)
		, IK_TraceDistanceBelowFoot(45.f)
	{ }
};