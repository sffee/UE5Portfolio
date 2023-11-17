#include "LocomotionComponent.h"

#include "Kismet/KismetMathLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "TimerManager.h"
#include "Animation/AnimInstance.h"

#include "Library/LogLibrary.h"
#include "Abilities/AttributeSets/CharacterAttributeSetBase.h"

#include "Character/CharacterBase.h"
#include "BOTWCharacterMovementComponent.h"
#include "Character/Player/Components/ClimbingComponent.h"

const FName NAME_RotationAmount(TEXT("RotationAmount"));
const FName NAME_YawOffset(TEXT("YawOffset"));
const FName NAME_pelvis(TEXT("pelvis"));
const FName NAME_root(TEXT("root"));

ULocomotionComponent::ULocomotionComponent()
	: IsMoving(false)
	, HasMovementInput(false)
	, Speed(0.f)
	, MovementInputAmount(0.f)
	, AimYawRate(0.f)
	, EasedMaxAcceleration(0.f)
	, DesiredRotationMode(ERotationMode::VelocityDirection)
	, DesiredGait(EGait::Running)
	, DesiredStance(EStance::Standing)
	, GroundedEntryState(EGroundedEntryState::None)
	, MovementState(EMovementState::Grounded)
	, PrevMovementState(EMovementState::None)
	, MovementAction(EMovementAction::None)
	, RotationMode(ERotationMode::VelocityDirection)
	, Gait(EGait::Walking)
	, Stance(EStance::Standing)
	, YawOffset(0.f)
	, PreviousAimYaw(0.f)
{
	PrimaryComponentTick.bCanEverTick = true;

}

void ULocomotionComponent::BeginPlay()
{
	Super::BeginPlay();
	
	Character = Cast<ACharacterBase>(GetOwner());
	CharacterMovementComponent = Character ? Cast<UBOTWCharacterMovementComponent>(Character->GetMovementComponent()) : nullptr;
	ClimbingComponent = Character ? Character->FindComponentByClass<UClimbingComponent>() : nullptr;

	if (Character == nullptr || CharacterMovementComponent == nullptr || ClimbingComponent == nullptr)
	{
		return;
	}

	Character->GetOnLockOnDelegate().AddUObject(this, &ThisClass::LockOn);

	SetMovementModel();

	ForceUpdateCharacterState();

	UAbilitySystemComponent* abilitySystemComponent = Character->GetAbilitySystemComponent();
	CharacterAttributeSet = abilitySystemComponent ? abilitySystemComponent->GetSet<UCharacterAttributeSetBase>() : nullptr;

	if (Stance == EStance::Standing)
	{
		Character->UnCrouch();
	}
	else if (Stance == EStance::Crouching)
	{
		Character->Crouch();
	}

	TargetRotation = Character->GetActorRotation();
	LastVelocityRotation = TargetRotation;
	LastMovementInputRotation = TargetRotation;

	CharacterMovementComponent->SetMovementSettings(GetTargetMovementSettings());
}

void ULocomotionComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (Character == nullptr || MovementState == EMovementState::Climbing)
	{
		return;
	}

	SetEssentialValues(DeltaTime);

	if (MovementState == EMovementState::Grounded)
	{
		UpdateCharacterMovement();
		UpdateGroundedRotation(DeltaTime);
	}
	else if (MovementState == EMovementState::InAir)
	{
		UpdateInAirRotation(DeltaTime);
	}

	PreviousVelocity = Character->GetVelocity();
	PreviousAimYaw = AimingRotation.Yaw;
}

void ULocomotionComponent::MoveForward(float _Value)
{
	if (Character == nullptr || 0.01f <= Character->GetMesh()->GetAnimInstance()->GetCurveValue(FName("BlockMove")))
	{
		return;
	}

	if (MovementState == EMovementState::Grounded || MovementState == EMovementState::InAir)
	{
		const FRotator dirRotator(0.f, AimingRotation.Yaw, 0.f);
		Character->AddMovementInput(UKismetMathLibrary::GetForwardVector(dirRotator), _Value);
	}
}

void ULocomotionComponent::MoveSide(float _Value)
{
	if (Character == nullptr || 0.01f <= Character->GetMesh()->GetAnimInstance()->GetCurveValue(FName("BlockMove")))
	{
		return;
	}

	if (MovementState == EMovementState::Grounded || MovementState == EMovementState::InAir)
	{
		const FRotator dirRotator(0.f, AimingRotation.Yaw, 0.f);
		Character->AddMovementInput(UKismetMathLibrary::GetRightVector(dirRotator), _Value);
	}
}

void ULocomotionComponent::OnMovementStateChanged(EMovementState _PreviousState)
{
	if (Character == nullptr)
	{
		return;
	}

	if (MovementState == EMovementState::InAir)
	{
		if (MovementAction == EMovementAction::None)
		{
			InAirRotation = Character->GetActorRotation();
			if (Stance == EStance::Crouching)
			{
				Character->UnCrouch();
			}
		}
	}

	if (CameraAnimInstance)
	{
		CameraAnimInstance->SetMovementState(MovementState);
	}
}

void ULocomotionComponent::OnMovementActionChanged(EMovementAction _PreviousAction)
{
	if (CameraAnimInstance)
	{
		CameraAnimInstance->SetMovementAction(MovementAction);
	}
}

void ULocomotionComponent::OnStanceChanged(EStance _PreviousStance)
{
	if (CameraAnimInstance)
	{
		CameraAnimInstance->SetStance(_PreviousStance);
	}

	if (CharacterMovementComponent)
	{
		CharacterMovementComponent->SetMovementSettings(GetTargetMovementSettings());
	}
}

void ULocomotionComponent::OnRotationModeChanged(ERotationMode _PreviousRotationMode)
{
	if (CameraAnimInstance)
	{
		CameraAnimInstance->SetRotationMode(_PreviousRotationMode);
	}

	if (CharacterMovementComponent)
	{
		CharacterMovementComponent->SetMovementSettings(GetTargetMovementSettings());
	}
}

void ULocomotionComponent::OnGaitChanged(EGait _PreviousGait)
{
	if (CameraAnimInstance)
	{
		CameraAnimInstance->SetGait(Gait);
	}
}

void ULocomotionComponent::OnJumped()
{
	if (Character)
	{
		InAirRotation = 100.f < Speed ? LastVelocityRotation : Character->GetActorRotation();
	}
}

void ULocomotionComponent::OnLanded(const FHitResult& _HitResult)
{
	if (CharacterMovementComponent)
	{
		CharacterMovementComponent->BrakingFrictionFactor = HasMovementInput ? 0.5f : 3.f;
	}

	GetWorld()->GetTimerManager().SetTimer(OnLandedFrictionResetTimer, this, &ULocomotionComponent::OnLandFrictionReset, 0.5f, false);
}

void ULocomotionComponent::OnLandFrictionReset()
{
	if (CharacterMovementComponent)
	{
		CharacterMovementComponent->BrakingFrictionFactor = 0.f;
	}
}

void ULocomotionComponent::SetEssentialValues(float _DeltaTime)
{
	if (Character == nullptr || CharacterMovementComponent == nullptr)
	{
		return;
	}

	CurrentAcceleration = CharacterMovementComponent->GetCurrentAcceleration();
	ControlRotation = Character->GetControlRotation();
	EasedMaxAcceleration = CharacterMovementComponent->GetMaxAcceleration();

	AimingRotation = FMath::RInterpTo(AimingRotation, ControlRotation, _DeltaTime, 30.f);

	const FVector currentVelocity = Character->GetVelocity();
	Acceleration = (currentVelocity - PreviousVelocity) / _DeltaTime;

	Speed = currentVelocity.Size2D();
	IsMoving = 1.f < Speed;
	if (IsMoving)
	{
		LastVelocityRotation = currentVelocity.ToOrientationRotator();
	}

	MovementInputAmount = CurrentAcceleration.Size() / EasedMaxAcceleration;
	HasMovementInput = 0.f < MovementInputAmount;
	if (HasMovementInput)
	{
		LastMovementInputRotation = CurrentAcceleration.ToOrientationRotator();
	}

	AimYawRate = FMath::Abs((AimingRotation.Yaw - PreviousAimYaw) / _DeltaTime);
}

void ULocomotionComponent::UpdateCharacterMovement()
{
	const EGait allowedGait = GetAllowedGait();

	const EGait actualGait = GetActualGait(allowedGait);

	if (actualGait != Gait)
	{
		SetGait(actualGait);
	}
}

void ULocomotionComponent::UpdateGroundedRotation(float _DeltaTime)
{
	if (MovementAction == EMovementAction::None)
	{
		const bool canUpdateMovingRot = ((IsMoving && HasMovementInput) || 150.f < Speed) && Character->HasAnyRootMotion() == false;
		if (canUpdateMovingRot)
		{
			const float groundedRotationRate = CalculateGroundedRotationRate();
			if (RotationMode == ERotationMode::VelocityDirection)
			{
				SmoothCharacterRotation({ 0.f, LastVelocityRotation.Yaw, 0.f }, 800.f, groundedRotationRate, _DeltaTime);
			}
			else if (RotationMode == ERotationMode::LookingDirection)
			{
				float yawValue;
				if (Gait == EGait::Sprinting)
				{
					yawValue = LastVelocityRotation.Yaw;
				}
				else
				{
					const float yawOffsetCurveVal = GetAnimCurveValue(NAME_YawOffset);
					yawValue = AimingRotation.Yaw + yawOffsetCurveVal;
				}

				SmoothCharacterRotation({ 0.0f, yawValue, 0.0f }, 500.0f, groundedRotationRate, _DeltaTime);
			}
			else if (RotationMode == ERotationMode::Aiming)
			{
				const float controlYaw = AimingRotation.Yaw;
				SmoothCharacterRotation({ 0.f, controlYaw, 0.f }, 1000.f, 20.f, _DeltaTime);
			}
		}
		else
		{
			if (RotationMode == ERotationMode::Aiming)
			{
				LimitRotation(-100.0f, 100.0f, 20.0f, _DeltaTime);
			}

			const float rotAmountCurve = GetAnimCurveValue(NAME_RotationAmount);

			if (0.001f < FMath::Abs(rotAmountCurve))
			{
				TargetRotation.Yaw = UKismetMathLibrary::NormalizeAxis(TargetRotation.Yaw + (rotAmountCurve * (_DeltaTime / (1.f / 30.f))));
				Character->SetActorRotation(TargetRotation);
				
				TargetRotation = Character->GetActorRotation();
			}
		}

		if (RotationMode == ERotationMode::LockOn)
		{
			float yawValue = Gait == EGait::Sprinting ? LastVelocityRotation.Yaw : AimingRotation.Yaw;
			SmoothCharacterRotation({ 0.0f, yawValue, 0.0f }, 1000.0f, 1000.f, _DeltaTime);
		}
	}
}

void ULocomotionComponent::UpdateInAirRotation(float _DeltaTime)
{
	if (Character == nullptr)
	{
		return;
	}

	if (RotationMode == ERotationMode::VelocityDirection || RotationMode == ERotationMode::LookingDirection)
	{
		SmoothCharacterRotation({ 0.f, InAirRotation.Yaw, 0.f }, 0.f, 5.f, _DeltaTime);
	}
	else if (RotationMode == ERotationMode::Aiming)
	{
		SmoothCharacterRotation({ 0.f, AimingRotation.Yaw, 0.f }, 0.f, 15.f, _DeltaTime);
		InAirRotation = Character->GetActorRotation();
	}

	if (RotationMode == ERotationMode::LockOn)
	{
		SmoothCharacterRotation({ 0.0f, AimingRotation.Yaw, 0.0f }, 1000.0f, 1000.f, _DeltaTime);
	}
}

void ULocomotionComponent::SmoothCharacterRotation(FRotator _Target, float _TargetInterpSpeed, float _ActorInterpSpeed, float _DeltaTime)
{
	if (Character == nullptr)
	{
		return;
	}
	
	TargetRotation = FMath::RInterpConstantTo(TargetRotation, _Target, _DeltaTime, _TargetInterpSpeed);
	Character->SetActorRotation(FMath::RInterpTo(Character->GetActorRotation(), TargetRotation, _DeltaTime, _ActorInterpSpeed));
}

float ULocomotionComponent::CalculateGroundedRotationRate() const
{
	if (CharacterMovementComponent == nullptr)
	{
		return 0.f;
	}

	const float mappedSpeedVal = CharacterMovementComponent->GetMappedSpeed();
	const float curveVal = CharacterMovementComponent->GetCurrentMovementSettings().RotationRateCurve->GetFloatValue(mappedSpeedVal);
	const float clampedAimYawRate = FMath::GetMappedRangeValueClamped<float, float>({ 0.f, 300.0f }, { 1.f, 3.f }, AimYawRate);
	return curveVal * clampedAimYawRate;
}

void ULocomotionComponent::LimitRotation(float _AimYawMin, float _AimYawMax, float _InterpSpeed, float _DeltaTime)
{
	if (Character == nullptr)
	{
		return;
	}

	FRotator delta = AimingRotation - Character->GetActorRotation();
	delta.Normalize();
	const float rangeVal = delta.Yaw;

	if (rangeVal < _AimYawMin || _AimYawMax < rangeVal)
	{
		const float controlRotYaw = AimingRotation.Yaw;
		const float targetYaw = controlRotYaw + (0.f < rangeVal ? _AimYawMin : _AimYawMax);
		SmoothCharacterRotation({ 0.0f, targetYaw, 0.0f }, 0.0f, _InterpSpeed, _DeltaTime);
	}
}

void ULocomotionComponent::SetMovementModel()
{
	if (MovementModel.DataTable == nullptr)
	{
		PRINT_ERROR("MovementModel.DataTable == nullptr");
		return;
	}
	
	const FString ContextString = GetFullName();
	FMovementStateSettings* OutRow =
		MovementModel.DataTable->FindRow<FMovementStateSettings>(MovementModel.RowName, ContextString);
	check(OutRow);
	MovementData = *OutRow;
}

void ULocomotionComponent::ForceUpdateCharacterState()
{
	SetGait(DesiredGait, true);
	SetStance(DesiredStance, true);
	SetRotationMode(DesiredRotationMode, true);
	SetMovementState(MovementState, true);
	SetMovementAction(MovementAction, true);
}

EGait ULocomotionComponent::GetAllowedGait() const
{
	if (Stance == EStance::Standing)
	{
		if (RotationMode != ERotationMode::Aiming)
		{
			if (DesiredGait == EGait::Sprinting)
			{
				if (CanSprint())
				{
					return EGait::Sprinting;
				}
				else
				{
					if (Gait == EGait::Sprinting)
					{
						FGameplayTagContainer tagContainer(GAMEPLAYTAG.Character_State_Sprint);
						Character->GetAbilitySystemComponent()->CancelAbilities(&tagContainer);
					}
				}
			}

			return DesiredGait;
		}
	}

	if (DesiredGait == EGait::Sprinting)
	{
		return EGait::Running;
	}

	return DesiredGait;
}

EGait ULocomotionComponent::GetActualGait(EGait _AllowedGait) const
{
	if (CharacterMovementComponent == nullptr || CharacterAttributeSet == nullptr)
	{
		return EGait::Walking;
	}

	if (CharacterAttributeSet->GetRunMoveSpeed() + 10.f < Speed)
	{
		if (_AllowedGait == EGait::Sprinting)
		{
			return EGait::Sprinting;
		}

		return EGait::Running;
	}

	if (CharacterAttributeSet->GetWalkMoveSpeed()  + 10.f <= Speed)
	{
		return EGait::Running;
	}

	return EGait::Walking;
}

void ULocomotionComponent::Sprint(bool _SprintInput)
{
	if (_SprintInput)
	{
		SetDesiredGait(EGait::Sprinting);
	}
	else
	{
		SetDesiredGait(EGait::Running);
	}
}

bool ULocomotionComponent::CanSprint() const
{
	if (HasMovementInput == false || RotationMode == ERotationMode::Aiming)
	{
		return false;
	}

	return 0.9f < MovementInputAmount;
}

void ULocomotionComponent::LockOn(ACharacterBase* _LockOnTarget)
{
	LockOnTarget = _LockOnTarget;

	if (LockOnTarget)
	{
		SetRotationMode(ERotationMode::LockOn);
	}
	else
	{
		SetRotationMode(ERotationMode::VelocityDirection);
	}
}

float ULocomotionComponent::GetAnimCurveValue(FName _CurveName) const
{
	if (Character && Character->GetMesh()->GetAnimInstance())
	{
		return Character->GetMesh()->GetAnimInstance()->GetCurveValue(_CurveName);
	}

	return 0.0f;
}

FMovementSettings ULocomotionComponent::GetTargetMovementSettings() const
{
	if (RotationMode == ERotationMode::VelocityDirection)
	{
		if (Stance == EStance::Standing)
		{
			return MovementData.VelocityDirection.Standing;
		}
		if (Stance == EStance::Crouching)
		{
			return MovementData.VelocityDirection.Crouching;
		}
	}
	else if (RotationMode == ERotationMode::Aiming)
	{
		if (Stance == EStance::Standing)
		{
			return MovementData.Aiming.Standing;
		}
		if (Stance == EStance::Crouching)
		{
			return MovementData.Aiming.Crouching;
		}
	}

	return MovementData.VelocityDirection.Standing;
}

void ULocomotionComponent::SetMovementState(EMovementState _NewState, bool _IsForce /*= false*/)
{
	if (_IsForce || MovementState != _NewState)
	{
		PrevMovementState = MovementState;
		MovementState = _NewState;
		OnMovementStateChanged(PrevMovementState);
	}
}

void ULocomotionComponent::SetMovementAction(EMovementAction _NewAction, bool _IsForce /*= false*/)
{
	if (_IsForce || MovementAction != _NewAction)
	{
		const EMovementAction prev = MovementAction;
		MovementAction = _NewAction;
		OnMovementActionChanged(prev);
	}
}

void ULocomotionComponent::SetStance(EStance _NewStance, bool _IsForce /*= false*/)
{
	if (_IsForce || Stance != _NewStance)
	{
		const EStance prev = Stance;
		Stance = _NewStance;
		OnStanceChanged(prev);
	}
}

void ULocomotionComponent::SetDesiredStance(EStance _NewStance)
{
	DesiredStance = _NewStance;
}

void ULocomotionComponent::SetGait(EGait _NewGait, bool _IsForce /*= false*/)
{
	if (_IsForce || Gait != _NewGait)
	{
		const EGait prev = Gait;
		Gait = _NewGait;
		OnGaitChanged(prev);
	}
}

void ULocomotionComponent::SetDesiredGait(EGait _NewGait)
{
	DesiredGait = _NewGait;
}

void ULocomotionComponent::SetDesiredRotationMode(ERotationMode _NewRotMode)
{
	DesiredRotationMode = _NewRotMode;
}

void ULocomotionComponent::SetRotationMode(ERotationMode _NewRotationMode, bool _IsForce /*= false*/)
{
	if (_IsForce || RotationMode != _NewRotationMode)
	{
		const ERotationMode prev = RotationMode;
		RotationMode = _NewRotationMode;
		OnRotationModeChanged(prev);
	}
}

void ULocomotionComponent::SetGroundedEntryState(EGroundedEntryState _NewState)
{
	GroundedEntryState = _NewState;
}