#include "CharacterAnimInstanceBase.h"

#include "Kismet/GameplayStatics.h"

#include "CharacterBase.h"
#include "Components/LocomotionComponent.h"
#include "Abilities/AttributeSets/CharacterAttributeSetBase.h"

#include "Library/BOTWMathLibrary.h"
#include "Curves/CurveVector.h"
#include "Components/CapsuleComponent.h"

#include "Equipment/EquipmentManagerComponent.h"
#include "Weapon/WeaponInstance.h"

static const FName NAME_BasePose_CLF(TEXT("BasePose_CLF"));
static const FName NAME_BasePose_N(TEXT("BasePose_N"));
static const FName NAME_Enable_FootIK_R(TEXT("Enable_FootIK_R"));
static const FName NAME_Enable_FootIK_L(TEXT("Enable_FootIK_L"));
static const FName NAME_Enable_HandIK_L(TEXT("Enable_HandIK_L"));
static const FName NAME_Enable_HandIK_R(TEXT("Enable_HandIK_R"));
static const FName NAME_Enable_Transition(TEXT("Enable_Transition"));
static const FName NAME_FootLock_L(TEXT("FootLock_L"));
static const FName NAME_FootLock_R(TEXT("FootLock_R"));
static const FName NAME_Grounded_Slot(TEXT("Grounded Slot"));
static const FName NAME_Layering_Arm_L(TEXT("Layering_Arm_L"));
static const FName NAME_Layering_Arm_L_Add(TEXT("Layering_Arm_L_Add"));
static const FName NAME_Layering_Arm_L_LS(TEXT("Layering_Arm_L_LS"));
static const FName NAME_Layering_Arm_R(TEXT("Layering_Arm_R"));
static const FName NAME_Layering_Arm_R_Add(TEXT("Layering_Arm_R_Add"));
static const FName NAME_Layering_Arm_R_LS(TEXT("Layering_Arm_R_LS"));
static const FName NAME_Layering_Hand_L(TEXT("Layering_Hand_L"));
static const FName NAME_Layering_Hand_R(TEXT("Layering_Hand_R"));
static const FName NAME_Layering_Head_Add(TEXT("Layering_Head_Add"));
static const FName NAME_Layering_Spine_Add(TEXT("Layering_Spine_Add"));
static const FName NAME_Mask_AimOffset(TEXT("Mask_AimOffset"));
static const FName NAME_Mask_LandPrediction(TEXT("Mask_LandPrediction"));
static const FName NAME_CharacterAnimInstance_RotationAmount(TEXT("RotationAmount"));
static const FName NAME_VB_foot_target_l(TEXT("VB foot_target_l"));
static const FName NAME_VB_foot_target_r(TEXT("VB foot_target_r"));
static const FName NAME_W_Gait(TEXT("W_Gait"));
static const FName NAME_CharacterAnimInstance_root(TEXT("root"));

UCharacterAnimInstanceBase::UCharacterAnimInstanceBase()
	: MovementState(EMovementState::None)
	, MovementAction(EMovementAction::None)
	, RotationMode(ERotationMode::VelocityDirection)
	, Gait(EGait::Walking)
	, Stance(EStance::Standing)
	, GroundedEntryState(EGroundedEntryState::None)
	, MovementDirection(EMovementDirection::Forward)
	, FlailRate(0.f)
	, IkFootL_BoneName(FName(TEXT("VB ik_foot_l")))
	, IkFootR_BoneName(FName(TEXT("VB ik_foot_r")))
	, IsEquippedShield(false)
	, IsShieldGuard(false)
	, CanPlayDynamicTransition(true)
{

}

void UCharacterAnimInstanceBase::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();
}

void UCharacterAnimInstanceBase::NativeBeginPlay()
{
	Super::NativeBeginPlay();

	Character = Cast<ACharacterBase>(TryGetPawnOwner());
	if (Character)
	{
		Character->OnJumpedDelegate.AddUniqueDynamic(this, &UCharacterAnimInstanceBase::OnJumped);

		LocomotionComponent = Character->GetLocomotionComponent();

		FTimerDelegate timerDelegate;
		timerDelegate.BindLambda([&]
		{
			AbilitySystemComponent = Character->GetAbilitySystemComponent();
			
			if (AbilitySystemComponent)
			{
				CharacterAttributeSet = AbilitySystemComponent->GetSet<UCharacterAttributeSetBase>();

				ShieldGuardGameplayTagDelegateHandle = AbilitySystemComponent->RegisterGameplayTagEvent(GAMEPLAYTAG.Character_State_ShieldGuard).AddUObject(this, &UCharacterAnimInstanceBase::OnGameplayTagChange);
			}
		});
		GetWorld()->GetTimerManager().SetTimerForNextTick(timerDelegate);

		UEquipmentManagerComponent* equipmentManager = Cast<UEquipmentManagerComponent>(Character->GetComponentByClass(UEquipmentManagerComponent::StaticClass()));
		if (equipmentManager)
		{
			equipmentManager->GetOnEquippedWeaponChangeDelegate().AddUObject(this, &UCharacterAnimInstanceBase::OnEquippedWeaponChange);
			equipmentManager->GetOnEquippedShieldChangeDelegate().AddUObject(this, &UCharacterAnimInstanceBase::OnEquippedShieldChange);
		}
	}
}

void UCharacterAnimInstanceBase::NativeUninitializeAnimation()
{
	Super::NativeUninitializeAnimation();

	if (AbilitySystemComponent && ShieldGuardGameplayTagDelegateHandle.IsValid())
	{
		AbilitySystemComponent->UnregisterGameplayTagEvent(ShieldGuardGameplayTagDelegateHandle, GAMEPLAYTAG.Character_State_ShieldGuard);
	}
}

void UCharacterAnimInstanceBase::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	UpdateLocomotion(DeltaSeconds);
}

void UCharacterAnimInstanceBase::UpdateLocomotion(float _DeltaSeconds)
{
	if (Character == false || LocomotionComponent == nullptr || _DeltaSeconds == 0.0f)
	{
		return;
	}

	CharacterInformation.MovementInputAmount = LocomotionComponent->GetMovementInputAmount();
	CharacterInformation.HasMovementInput = LocomotionComponent->GetHasMovementInput();
	CharacterInformation.IsMoving = LocomotionComponent->GetIsMoving();
	CharacterInformation.Acceleration = LocomotionComponent->GetAcceleration();
	CharacterInformation.AimYawRate = LocomotionComponent->GetAimYawRate();
	CharacterInformation.Speed = LocomotionComponent->GetSpeed();
	CharacterInformation.Velocity = Character->GetCharacterMovement()->Velocity;
	CharacterInformation.MovementInput = LocomotionComponent->GetMovementInput();
	CharacterInformation.AimingRotation = LocomotionComponent->GetAimingRotation();
	CharacterInformation.CharacterActorRotation = Character->GetActorRotation();
	CharacterInformation.PrevMovementState = LocomotionComponent->GetPrevMovementState();
	MovementState = LocomotionComponent->GetMovementState();
	MovementAction = LocomotionComponent->GetMovementAction();
	Stance = LocomotionComponent->GetStance();
	RotationMode = LocomotionComponent->GetRotationMode();
	Gait = LocomotionComponent->GetGait();
	GroundedEntryState = LocomotionComponent->GetGroundedEntryState();

	if (MovementState.Climbing())
	{
		CharacterInformation.ClimbVelocity = CharacterInformation.CharacterActorRotation.UnrotateVector(CharacterInformation.Velocity);
	}

	UpdateAimingValues(_DeltaSeconds);
	UpdateLayerValues();
	UpdateFootIK(_DeltaSeconds);

	if (MovementState.Grounded())
	{
		const bool isPrevShouldMove = Grounded.IsShouldMove;
		Grounded.IsShouldMove = ShouldMoveCheck();

		if (isPrevShouldMove == false && Grounded.IsShouldMove)
		{
			Grounded.IsRotateL = false;
			Grounded.IsRotateR = false;
		}

		if (Grounded.IsShouldMove)
		{
			UpdateMovementValues(_DeltaSeconds);
			UpdateRotationValues();
		}
		else
		{
			if (CanRotateInPlace())
			{
				RotateInPlaceCheck();
			}
			else
			{
				Grounded.IsRotateL = false;
				Grounded.IsRotateR = false;
			}

			//if (CanDynamicTransition())
			//{
			//	DynamicTransitionCheck();
			//}
		}
	}
	else if (MovementState.InAir())
	{
		UpdateInAirValues(_DeltaSeconds);
	}
}

void UCharacterAnimInstanceBase::OnJumped()
{
	InAir.IsJumped = true;
	InAir.JumpPlayRate = FMath::GetMappedRangeValueClamped<float, float>({ 0.0f, 600.0f }, { 1.2f, 1.5f }, CharacterInformation.Speed);

	GetWorld()->GetTimerManager().SetTimer(OnJumpedTimer, this, &UCharacterAnimInstanceBase::OnJumpedDelay, 0.1f, false);
}

void UCharacterAnimInstanceBase::OnPivot()
{
	Grounded.IsPivot = CharacterInformation.Speed < Config.TriggerPivotSpeedLimit;
	GetWorld()->GetTimerManager().SetTimer(OnPivotTimer, this, &UCharacterAnimInstanceBase::OnPivotDelay, 0.1f, false);
}

bool UCharacterAnimInstanceBase::ShouldMoveCheck() const
{
	return (CharacterInformation.IsMoving && CharacterInformation.HasMovementInput) || 150.0f < CharacterInformation.Speed;
}

bool UCharacterAnimInstanceBase::CanRotateInPlace() const
{
	return RotationMode.Aiming();
}

bool UCharacterAnimInstanceBase::CanDynamicTransition() const
{
	return 0.99f <= GetCurveValue(NAME_Enable_Transition);
}

void UCharacterAnimInstanceBase::PlayTransition(const FDynamicMontageParams& _Parameters)
{
	PlaySlotAnimationAsDynamicMontage(_Parameters.Animation, NAME_Grounded_Slot,
		_Parameters.BlendInTime, _Parameters.BlendOutTime, _Parameters.PlayRate, 1,
		0.f, _Parameters.StartTime);
}

void UCharacterAnimInstanceBase::PlayTransitionChecked(const FDynamicMontageParams& _Parameters)
{
	if (Stance.Standing() && !Grounded.IsShouldMove)
	{
		PlayTransition(_Parameters);
	}
}

void UCharacterAnimInstanceBase::PlayDynamicTransition(float _ReTriggerDelay, FDynamicMontageParams _Parameters)
{
	if (CanPlayDynamicTransition)
	{
		CanPlayDynamicTransition = false;

		PlayTransition(_Parameters);

		UWorld* world = GetWorld();
		check(world);
		world->GetTimerManager().SetTimer(PlayDynamicTransitionTimer, this,
			&UCharacterAnimInstanceBase::PlayDynamicTransitionDelay,
			_ReTriggerDelay, false);
	}
}

void UCharacterAnimInstanceBase::SetTrackedHipsDirection(EHipsDirection _HipsDirection)
{
	Grounded.TrackedHipsDirection = _HipsDirection;
}

void UCharacterAnimInstanceBase::SetGroundedEntryState(EGroundedEntryState _NewState)
{
	GroundedEntryState = _NewState;
}

void UCharacterAnimInstanceBase::PlayDynamicTransitionDelay()
{
	CanPlayDynamicTransition = true;
}

void UCharacterAnimInstanceBase::OnJumpedDelay()
{
	InAir.IsJumped = false;
}

void UCharacterAnimInstanceBase::OnPivotDelay()
{
	Grounded.IsPivot = false;
}

FVector UCharacterAnimInstanceBase::CalculateRelativeAccelerationAmount() const
{
	if (0.f < FVector::DotProduct(CharacterInformation.Acceleration, CharacterInformation.Velocity))
	{
		const float maxAcc = Character->GetCharacterMovement()->GetMaxAcceleration();
		return CharacterInformation.CharacterActorRotation.UnrotateVector(
			CharacterInformation.Acceleration.GetClampedToMaxSize(maxAcc) / maxAcc);
	}

	const float maxBrakingDec = Character->GetCharacterMovement()->GetMaxBrakingDeceleration();
	return
		CharacterInformation.CharacterActorRotation.UnrotateVector(
			CharacterInformation.Acceleration.GetClampedToMaxSize(maxBrakingDec) / maxBrakingDec);
}

float UCharacterAnimInstanceBase::CalculateStrideBlend() const
{
	if (StrideBlend_N_Walk == nullptr || StrideBlend_N_Run == nullptr)
	{
		return 0.f;
	}

	const float curveTime = CharacterInformation.Speed / GetOwningComponent()->GetComponentScale().Z;
	const float clampedGait = GetAnimCurveClamped(NAME_W_Gait, -1., 0.f, 1.f);
	const float lerpedStrideBlend = FMath::Lerp(StrideBlend_N_Walk->GetFloatValue(curveTime), StrideBlend_N_Run->GetFloatValue(curveTime), clampedGait);

	return FMath::Lerp(lerpedStrideBlend, StrideBlend_C_Walk->GetFloatValue(CharacterInformation.Speed), GetCurveValue(NAME_BasePose_CLF));
}

float UCharacterAnimInstanceBase::CalculateWalkRunBlend() const
{
	return Gait.Walking() ? 0.0f : 1.0;
}

float UCharacterAnimInstanceBase::CalculateStandingPlayRate() const
{
	if (Character == nullptr)
	{
		return 0.f;
	}

	float defaultWalkMoveSpeed = 0.f, defaultRunMoveSpeed = 0.f, defaultSprintMoveSpeed = 0.f;
	if (CharacterAttributeSet)
	{
		defaultWalkMoveSpeed = CharacterAttributeSet->GetBaseWalkMoveSpeed();
		defaultRunMoveSpeed = CharacterAttributeSet->GetBaseRunMoveSpeed();
		defaultSprintMoveSpeed = CharacterAttributeSet->GetBaseSprintMoveSpeed();
	}

	const float lerpedSpeed = FMath::Lerp(CharacterInformation.Speed / defaultWalkMoveSpeed, CharacterInformation.Speed / defaultRunMoveSpeed, GetAnimCurveClamped(NAME_W_Gait, -1.f, 0.f, 1.f));

	const float sprintAffectedSpeed = FMath::Lerp(lerpedSpeed, CharacterInformation.Speed / defaultSprintMoveSpeed, GetAnimCurveClamped(NAME_W_Gait, -2.f, 0.f, 1.f));

	return FMath::Clamp(sprintAffectedSpeed / Grounded.StrideBlend / GetOwningComponent()->GetComponentScale().Z, 0.f, 3.f);
}

float UCharacterAnimInstanceBase::CalculateDiagonalScaleAmount() const
{
	return DiagonalScaleAmountCurve ? DiagonalScaleAmountCurve->GetFloatValue(FMath::Abs(VelocityBlend.F + VelocityBlend.B)) : 0.f;
}

float UCharacterAnimInstanceBase::CalculateCrouchingPlayRate() const
{
	if (Character == nullptr)
	{
		return 0.f;
	}

	float defaultCrouchMoveSpeed = 0.f;
	if (CharacterAttributeSet)
	{
		defaultCrouchMoveSpeed = CharacterAttributeSet->GetBaseCrouchMoveSpeed();
	}

	return FMath::Clamp(CharacterInformation.Speed / defaultCrouchMoveSpeed / Grounded.StrideBlend / GetOwningComponent()->GetComponentScale().Z, 0.f, 2.f);
}

float UCharacterAnimInstanceBase::CalculateLandPrediction() const
{
	if (-200.f <= InAir.FallSpeed)
	{
		return 0.0f;
	}

	const UCapsuleComponent* capsuleComp = Character->GetCapsuleComponent();
	const FVector& capsuleWorldLoc = capsuleComp->GetComponentLocation();
	const float velocityZ = CharacterInformation.Velocity.Z;
	FVector velocityClamped = CharacterInformation.Velocity;
	velocityClamped.Z = FMath::Clamp(velocityZ, -4000.0f, -200.0f);
	velocityClamped.Normalize();

	const FVector traceLength = velocityClamped * FMath::GetMappedRangeValueClamped<float, float>({ 0.f, -4000.f }, { 50.f, 2000.f }, velocityZ);

	UWorld* world = GetWorld();
	check(world);

	FCollisionQueryParams params;
	params.AddIgnoredActor(Character);

	FHitResult hitResult;
	const FCollisionShape CapsuleCollisionShape = FCollisionShape::MakeCapsule(capsuleComp->GetUnscaledCapsuleRadius(), capsuleComp->GetUnscaledCapsuleHalfHeight());
	const bool isHit = world->SweepSingleByChannel(hitResult, capsuleWorldLoc, capsuleWorldLoc + traceLength, FQuat::Identity, ECC_Visibility, CapsuleCollisionShape, params);

	if (Character->GetCharacterMovement()->IsWalkable(hitResult) && LandPredictionCurve)
	{
		return FMath::Lerp(LandPredictionCurve->GetFloatValue(hitResult.Time), 0.0f, GetCurveValue(NAME_Mask_LandPrediction));
	}

	return 0.0f;
}

FLeanAmount UCharacterAnimInstanceBase::CalculateAirLeanAmount() const
{
	FLeanAmount calcLeanAmount;

	const FVector& unrotatedVel = CharacterInformation.CharacterActorRotation.UnrotateVector(CharacterInformation.Velocity) / 350.f;

	FVector2D inversedVect(unrotatedVel.Y, unrotatedVel.X);
	if (LeanInAirCurve)
	{
		inversedVect *= LeanInAirCurve->GetFloatValue(InAir.FallSpeed);
	}

	calcLeanAmount.LR = inversedVect.X;
	calcLeanAmount.FB = inversedVect.Y;
	return calcLeanAmount;
}

EMovementDirection UCharacterAnimInstanceBase::CalculateMovementDirection() const
{
	if (Gait.Sprinting() || RotationMode.VelocityDirection())
	{
		return EMovementDirection::Forward;
	}

	FRotator delta = CharacterInformation.Velocity.ToOrientationRotator() - CharacterInformation.AimingRotation;
	delta.Normalize();

	return UBOTWMathLibrary::CalculateQuadrant(MovementDirection, 70.f, -70.f, 110.f, -110.f, 5.f, delta.Yaw);
}

void UCharacterAnimInstanceBase::UpdateAimingValues(float _DeltaSeconds)
{
	AimingValues.SmoothedAimingRotation = FMath::RInterpTo(AimingValues.SmoothedAimingRotation, CharacterInformation.AimingRotation, _DeltaSeconds, Config.SmoothedAimingRotationInterpSpeed);

	FRotator delta = CharacterInformation.AimingRotation - CharacterInformation.CharacterActorRotation;
	delta.Normalize();
	AimingValues.AimingAngle.X = delta.Yaw;
	AimingValues.AimingAngle.Y = delta.Pitch;

	delta = AimingValues.SmoothedAimingRotation - CharacterInformation.CharacterActorRotation;
	delta.Normalize();

	SmoothedAimingAngle.X = delta.Yaw;
	SmoothedAimingAngle.Y = delta.Pitch;

	if (RotationMode.VelocityDirection() == false)
	{
		AimingValues.AimSweepTime = FMath::GetMappedRangeValueClamped<float, float>({ -90.f, 90.f }, { 1.f, 0.f }, AimingValues.AimingAngle.Y);

		AimingValues.SpineRotation.Roll = 0.0f;
		AimingValues.SpineRotation.Pitch = 0.0f;
		AimingValues.SpineRotation.Yaw = AimingValues.AimingAngle.X / 4.0f;
	}
	else if (CharacterInformation.HasMovementInput)
	{
		delta = CharacterInformation.MovementInput.ToOrientationRotator() - CharacterInformation.CharacterActorRotation;
		delta.Normalize();
		const float interpTarget = FMath::GetMappedRangeValueClamped<float, float>({ -180.f, 180.f }, { 0.f, 1.f }, delta.Yaw);

		AimingValues.InputYawOffsetTime = FMath::FInterpTo(AimingValues.InputYawOffsetTime, interpTarget, _DeltaSeconds, Config.InputYawOffsetInterpSpeed);
	}

	AimingValues.LeftYawTime = FMath::GetMappedRangeValueClamped<float, float>({ 0.f, 180.f }, { 0.5f, 0.f }, FMath::Abs(SmoothedAimingAngle.X));
	AimingValues.RightYawTime = FMath::GetMappedRangeValueClamped<float, float>({ 0.f, 180.f }, { 0.5f, 1.f }, FMath::Abs(SmoothedAimingAngle.X));
	AimingValues.ForwardYawTime = FMath::GetMappedRangeValueClamped<float, float>({ -180.f, 180.f }, { 0.f, 1.f }, SmoothedAimingAngle.X);
}

void UCharacterAnimInstanceBase::UpdateLayerValues()
{
	LayerBlendingValues.EnableAimOffset = FMath::Lerp(1.f, 0.f, GetCurveValue(NAME_Mask_AimOffset));
	
	LayerBlendingValues.BasePose_N = GetCurveValue(NAME_BasePose_N);
	LayerBlendingValues.BasePose_CLF = GetCurveValue(NAME_BasePose_CLF);
	
	LayerBlendingValues.Spine_Add = GetCurveValue(NAME_Layering_Spine_Add);
	LayerBlendingValues.Head_Add = GetCurveValue(NAME_Layering_Head_Add);
	LayerBlendingValues.Arm_L_Add = GetCurveValue(NAME_Layering_Arm_L_Add);
	LayerBlendingValues.Arm_R_Add = GetCurveValue(NAME_Layering_Arm_R_Add);
	
	LayerBlendingValues.Hand_R = GetCurveValue(NAME_Layering_Hand_R);
	LayerBlendingValues.Hand_L = GetCurveValue(NAME_Layering_Hand_L);
	
	LayerBlendingValues.EnableHandIK_L = FMath::Lerp(0.0f, GetCurveValue(NAME_Enable_HandIK_L), GetCurveValue(NAME_Layering_Arm_L));
	LayerBlendingValues.EnableHandIK_R = FMath::Lerp(0.0f, GetCurveValue(NAME_Enable_HandIK_R),	GetCurveValue(NAME_Layering_Arm_R));
	
	LayerBlendingValues.Arm_L_LS = GetCurveValue(NAME_Layering_Arm_L_LS);
	LayerBlendingValues.Arm_L_MS = static_cast<float>(1 - FMath::FloorToInt(LayerBlendingValues.Arm_L_LS));
	LayerBlendingValues.Arm_R_LS = GetCurveValue(NAME_Layering_Arm_R_LS);
	LayerBlendingValues.Arm_R_MS = static_cast<float>(1 - FMath::FloorToInt(LayerBlendingValues.Arm_R_LS));
}

void UCharacterAnimInstanceBase::UpdateFootIK(float _DeltaSeconds)
{
	FVector footOffsetLTarget = FVector::ZeroVector;
	FVector footOffsetRTarget = FVector::ZeroVector;

	SetFootLocking(_DeltaSeconds, NAME_Enable_FootIK_L, NAME_FootLock_L, IkFootL_BoneName, FootIKValues.FootLock_L_Alpha, FootIKValues.UseFootLockCurve_L, FootIKValues.FootLock_L_Location, FootIKValues.FootLock_L_Rotation);
	SetFootLocking(_DeltaSeconds, NAME_Enable_FootIK_R, NAME_FootLock_R,	IkFootR_BoneName, FootIKValues.FootLock_R_Alpha, FootIKValues.UseFootLockCurve_R, FootIKValues.FootLock_R_Location, FootIKValues.FootLock_R_Rotation);

	if (MovementState.InAir())
	{
		SetPelvisIKOffset(_DeltaSeconds, FVector::ZeroVector, FVector::ZeroVector);
		ResetIKOffsets(_DeltaSeconds);
	}

	SetFootOffsets(_DeltaSeconds, NAME_Enable_FootIK_L, IkFootL_BoneName, NAME_CharacterAnimInstance_root, footOffsetLTarget, FootIKValues.FootOffset_L_Location, FootIKValues.FootOffset_L_Rotation);
	SetFootOffsets(_DeltaSeconds, NAME_Enable_FootIK_R, IkFootR_BoneName, NAME_CharacterAnimInstance_root, footOffsetRTarget, FootIKValues.FootOffset_R_Location, FootIKValues.FootOffset_R_Rotation);
	SetPelvisIKOffset(_DeltaSeconds, footOffsetLTarget, footOffsetRTarget);
}

void UCharacterAnimInstanceBase::UpdateMovementValues(float _DeltaSeconds)
{
	const FVelocityBlend& targetBlend = CalculateVelocityBlend();
	VelocityBlend.F = FMath::FInterpTo(VelocityBlend.F, targetBlend.F, _DeltaSeconds, Config.VelocityBlendInterpSpeed);
	VelocityBlend.B = FMath::FInterpTo(VelocityBlend.B, targetBlend.B, _DeltaSeconds, Config.VelocityBlendInterpSpeed);
	VelocityBlend.L = FMath::FInterpTo(VelocityBlend.L, targetBlend.L, _DeltaSeconds, Config.VelocityBlendInterpSpeed);
	VelocityBlend.R = FMath::FInterpTo(VelocityBlend.R, targetBlend.R, _DeltaSeconds, Config.VelocityBlendInterpSpeed);

	Grounded.DiagonalScaleAmount = CalculateDiagonalScaleAmount();

	RelativeAccelerationAmount = CalculateRelativeAccelerationAmount();
	LeanAmount.LR = FMath::FInterpTo(LeanAmount.LR, RelativeAccelerationAmount.Y, _DeltaSeconds, Config.GroundedLeanInterpSpeed);
	LeanAmount.FB = FMath::FInterpTo(LeanAmount.FB, RelativeAccelerationAmount.X, _DeltaSeconds, Config.GroundedLeanInterpSpeed);

	Grounded.WalkRunBlend = CalculateWalkRunBlend();
	Grounded.StrideBlend = CalculateStrideBlend();

	Grounded.StandingPlayRate = CalculateStandingPlayRate();
	Grounded.CrouchingPlayRate = CalculateCrouchingPlayRate();
}

void UCharacterAnimInstanceBase::UpdateRotationValues()
{
	MovementDirection = CalculateMovementDirection();

	FRotator delta = CharacterInformation.Velocity.ToOrientationRotator() - CharacterInformation.AimingRotation;
	delta.Normalize();

	if (YawOffset_FB && YawOffset_LR)
	{
		const FVector& fbOffset = YawOffset_FB->GetVectorValue(delta.Yaw);
		Grounded.FYaw = fbOffset.X;
		Grounded.BYaw = fbOffset.Y;

		const FVector& lrOffset = YawOffset_LR->GetVectorValue(delta.Yaw);
		Grounded.LYaw = lrOffset.X;
		Grounded.RYaw = lrOffset.Y;
	}
}

void UCharacterAnimInstanceBase::UpdateInAirValues(float _DeltaSeconds)
{
	InAir.FallSpeed = CharacterInformation.Velocity.Z;

	InAir.LandPrediction = CalculateLandPrediction();

	const FLeanAmount& inAirLeanAmount = CalculateAirLeanAmount();
	LeanAmount.LR = FMath::FInterpTo(LeanAmount.LR, inAirLeanAmount.LR, _DeltaSeconds, Config.GroundedLeanInterpSpeed);
	LeanAmount.FB = FMath::FInterpTo(LeanAmount.FB, inAirLeanAmount.FB, _DeltaSeconds, Config.GroundedLeanInterpSpeed);
}

void UCharacterAnimInstanceBase::SetFootLocking(float _DeltaSeconds, FName _EnableFootIKCurve, FName _FootLockCurve, FName _IKFootBone, float& _CurFootLockAlpha, bool& _UseFootLockCurve, FVector& _CurFootLockLoc, FRotator& _CurFootLockRot)
{
	if (GetCurveValue(_EnableFootIKCurve) <= 0.0f)
	{
		return;
	}

	float footLockCurveVal;

	if (_UseFootLockCurve)
	{
		_UseFootLockCurve = FMath::Abs(GetCurveValue(NAME_CharacterAnimInstance_RotationAmount)) <= 0.001f;
		footLockCurveVal = GetCurveValue(_FootLockCurve) * (1.f / GetSkelMeshComponent()->AnimUpdateRateParams->UpdateRate);
	}
	else
	{
		_UseFootLockCurve = 0.99f <= GetCurveValue(_FootLockCurve);
		footLockCurveVal = 0.f;
	}

	if (0.99f <= footLockCurveVal || footLockCurveVal < _CurFootLockAlpha)
	{
		_CurFootLockAlpha = footLockCurveVal;
	}

	if (0.99f <= _CurFootLockAlpha)
	{
		const FTransform& ownerTransform = GetOwningComponent()->GetSocketTransform(_IKFootBone, RTS_Component);
		_CurFootLockLoc = ownerTransform.GetLocation();
		_CurFootLockRot = ownerTransform.Rotator();
	}

	if (0.f < _CurFootLockAlpha)
	{
		SetFootLockOffsets(_DeltaSeconds, _CurFootLockLoc, _CurFootLockRot);
	}
}

void UCharacterAnimInstanceBase::SetFootLockOffsets(float _DeltaSeconds, FVector& _LocalLoc, FRotator& _LocalRot)
{
	FRotator rotationDifference = FRotator::ZeroRotator;

	if (Character->GetCharacterMovement()->IsMovingOnGround())
	{
		rotationDifference = CharacterInformation.CharacterActorRotation - Character->GetCharacterMovement()->GetLastUpdateRotation();
		rotationDifference.Normalize();
	}

	const FVector& locationDifference = GetOwningComponent()->GetComponentRotation().UnrotateVector(CharacterInformation.Velocity * _DeltaSeconds);

	_LocalLoc = (_LocalLoc - locationDifference).RotateAngleAxis(rotationDifference.Yaw, FVector::DownVector);

	FRotator delta = _LocalRot - rotationDifference;
	delta.Normalize();
	_LocalRot = delta;
}

void UCharacterAnimInstanceBase::SetPelvisIKOffset(float _DeltaSeconds, FVector _FootOffsetLTarget, FVector _FootOffsetRTarget)
{
	FootIKValues.PelvisAlpha = (GetCurveValue(NAME_Enable_FootIK_L) + GetCurveValue(NAME_Enable_FootIK_R)) * 0.5f;

	if (0.0f < FootIKValues.PelvisAlpha)
	{
		const FVector pelvisTarget = _FootOffsetLTarget.Z < _FootOffsetRTarget.Z ? _FootOffsetLTarget : _FootOffsetRTarget;

		const float interpSpeed = FootIKValues.PelvisOffset.Z < pelvisTarget.Z ? 10.f : 15.f;
		FootIKValues.PelvisOffset =	FMath::VInterpTo(FootIKValues.PelvisOffset, pelvisTarget, _DeltaSeconds, interpSpeed);
	}
	else
	{
		FootIKValues.PelvisOffset = FVector::ZeroVector;
	}
}

void UCharacterAnimInstanceBase::ResetIKOffsets(float DeltaSeconds)
{
	FootIKValues.FootOffset_L_Location = FMath::VInterpTo(FootIKValues.FootOffset_L_Location, FVector::ZeroVector, DeltaSeconds, 15.0f);
	FootIKValues.FootOffset_R_Location = FMath::VInterpTo(FootIKValues.FootOffset_R_Location, FVector::ZeroVector, DeltaSeconds, 15.0f);
	FootIKValues.FootOffset_L_Rotation = FMath::RInterpTo(FootIKValues.FootOffset_L_Rotation, FRotator::ZeroRotator, DeltaSeconds, 15.0f);
	FootIKValues.FootOffset_R_Rotation = FMath::RInterpTo(FootIKValues.FootOffset_R_Rotation, FRotator::ZeroRotator, DeltaSeconds, 15.0f);
}

void UCharacterAnimInstanceBase::SetFootOffsets(float _DeltaSeconds, FName _EnableFootIKCurve, FName _IKFootBone, FName _RootBone, FVector& _CurLocationTarget, FVector& _CurLocationOffset, FRotator& _CurRotationOffset)
{
	if (GetCurveValue(_EnableFootIKCurve) <= 0.f)
	{
		_CurLocationOffset = FVector::ZeroVector;
		_CurRotationOffset = FRotator::ZeroRotator;
		return;
	}

	USkeletalMeshComponent* ownerComp = GetOwningComponent();
	FVector ikFootFloorLoc = ownerComp->GetSocketLocation(_IKFootBone);
	ikFootFloorLoc.Z = ownerComp->GetSocketLocation(_RootBone).Z;

	UWorld* world = GetWorld();
	check(world);

	FCollisionQueryParams params;
	params.AddIgnoredActor(Character);

	const FVector traceStart = ikFootFloorLoc + FVector(0.0, 0.0, Config.IK_TraceDistanceAboveFoot);
	const FVector traceEnd = ikFootFloorLoc - FVector(0.0, 0.0, Config.IK_TraceDistanceBelowFoot);

	FHitResult hitResult;
	const bool isHit = world->LineTraceSingleByChannel(hitResult, traceStart, traceEnd, ECC_Visibility, params);

	FRotator targetRotOffset = FRotator::ZeroRotator;
	if (Character->GetCharacterMovement()->IsWalkable(hitResult))
	{
		FVector impactPoint = hitResult.ImpactPoint;
		FVector impactNormal = hitResult.ImpactNormal;

		_CurLocationTarget = (impactPoint + impactNormal * Config.FootHeight) - (ikFootFloorLoc + FVector(0.f, 0.f, Config.FootHeight));

		targetRotOffset.Pitch = -FMath::RadiansToDegrees(FMath::Atan2(impactNormal.X, impactNormal.Z));
		targetRotOffset.Roll = FMath::RadiansToDegrees(FMath::Atan2(impactNormal.Y, impactNormal.Z));
	}

	const float interpSpeed = _CurLocationTarget.Z < _CurLocationOffset.Z ? 30.f : 15.f;
	_CurLocationOffset = FMath::VInterpTo(_CurLocationOffset, _CurLocationTarget, _DeltaSeconds, interpSpeed);
	
	_CurRotationOffset = FMath::RInterpTo(_CurRotationOffset, targetRotOffset, _DeltaSeconds, 30.0f);
}

void UCharacterAnimInstanceBase::SetOverlayType(EOverlayType _OverlayType)
{
	OverlayType = _OverlayType;
}

void UCharacterAnimInstanceBase::RotateInPlaceCheck()
{
	Grounded.IsRotateL = AimingValues.AimingAngle.X < RotateInPlace.RotateMinThreshold;
	Grounded.IsRotateR = AimingValues.AimingAngle.X > RotateInPlace.RotateMaxThreshold;

	if (Grounded.IsRotateL || Grounded.IsRotateR)
	{
		Grounded.RotateRate = FMath::GetMappedRangeValueClamped<float, float>(
			{ RotateInPlace.AimYawRateMinRange, RotateInPlace.AimYawRateMaxRange },
			{ RotateInPlace.MinPlayRate, RotateInPlace.MaxPlayRate },
			CharacterInformation.AimYawRate);
	}
}

void UCharacterAnimInstanceBase::DynamicTransitionCheck()
{
	FTransform socketTransformA = GetOwningComponent()->GetSocketTransform(IkFootL_BoneName, RTS_Component);
	FTransform socketTransformB = GetOwningComponent()->GetSocketTransform(NAME_VB_foot_target_l, RTS_Component);
	float distance = (socketTransformB.GetLocation() - socketTransformA.GetLocation()).Size();
	if (Config.DynamicTransitionThreshold < distance)
	{
		FDynamicMontageParams params;
		params.Animation = TransitionAnim_R;
		params.BlendInTime = 0.2f;
		params.BlendOutTime = 0.2f;
		params.PlayRate = 1.5f;
		params.StartTime = 0.8f;
		PlayDynamicTransition(0.1f, params);
	}

	socketTransformA = GetOwningComponent()->GetSocketTransform(IkFootR_BoneName, RTS_Component);
	socketTransformB = GetOwningComponent()->GetSocketTransform(NAME_VB_foot_target_r, RTS_Component);
	distance = (socketTransformB.GetLocation() - socketTransformA.GetLocation()).Size();
	if (Config.DynamicTransitionThreshold < distance)
	{
		FDynamicMontageParams params;
		params.Animation = TransitionAnim_L;
		params.BlendInTime = 0.2f;
		params.BlendOutTime = 0.2f;
		params.PlayRate = 1.5f;
		params.StartTime = 0.8f;
		PlayDynamicTransition(0.1f, params);
	}
}

FVelocityBlend UCharacterAnimInstanceBase::CalculateVelocityBlend() const
{
	const FVector locRelativeVelocityDir = CharacterInformation.CharacterActorRotation.UnrotateVector(CharacterInformation.Velocity.GetSafeNormal(0.1f));
	const float sum = FMath::Abs(locRelativeVelocityDir.X) + FMath::Abs(locRelativeVelocityDir.Y) + FMath::Abs(locRelativeVelocityDir.Z);
	const FVector RelativeDir = locRelativeVelocityDir / sum;
	
	FVelocityBlend result;
	result.F = FMath::Clamp(RelativeDir.X, 0.0f, 1.0f);
	result.B = FMath::Abs(FMath::Clamp(RelativeDir.X, -1.0f, 0.0f));
	result.L = FMath::Abs(FMath::Clamp(RelativeDir.Y, -1.0f, 0.0f));
	result.R = FMath::Clamp(RelativeDir.Y, 0.0f, 1.0f);
	return result;
}

void UCharacterAnimInstanceBase::OnEquippedWeaponChange(UEquipmentInstance* _PrevEquipInstance, UEquipmentInstance* _NewEquipInstance)
{
	if (_NewEquipInstance)
	{
		UWeaponInstance* weaponInstance = Cast<UWeaponInstance>(_NewEquipInstance);
		OverlayType = weaponInstance ? weaponInstance->GetOverlayType() : EOverlayType::Default;
	}
}

void UCharacterAnimInstanceBase::OnEquippedShieldChange(UEquipmentInstance* _PrevEquipInstance, UEquipmentInstance* _NewEquipInstance)
{
	IsEquippedShield = IsValid(_NewEquipInstance);
}

void UCharacterAnimInstanceBase::OnGameplayTagChange(const FGameplayTag _Tag, int32 _Count)
{
	if (_Tag == GAMEPLAYTAG.Character_State_ShieldGuard)
	{
		IsShieldGuard = 0 < _Count;
	}
}

float UCharacterAnimInstanceBase::GetAnimCurveClamped(const FName& _Name, float _Bias, float _ClampMin, float _ClampMax) const
{
	return FMath::Clamp(GetCurveValue(_Name) + _Bias, _ClampMin, _ClampMax);
}
