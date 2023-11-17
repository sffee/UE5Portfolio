#include "BOTWCharacterMovementComponent.h"

#include "Character/Components/LocomotionComponent.h"
#include "Character/Player/Components/ClimbingComponent.h"
#include "Character/CharacterBase.h"
#include "Abilities/AttributeSets/CharacterAttributeSetBase.h"
#include "Curves/CurveVector.h"

void UBOTWCharacterMovementComponent::BeginPlay()
{
	Super::BeginPlay();

	Character = Cast<ACharacterBase>(GetOwner());

	if (Character)
	{
		FTimerDelegate timerDelegate;
		timerDelegate.BindLambda([&]
		{
			AbilitySystemComponent = Character->GetAbilitySystemComponent();
			CharacterAttributeSet = AbilitySystemComponent ? Character->GetAbilitySystemComponent()->GetSet<UCharacterAttributeSetBase>() : nullptr;
		});

		GetWorld()->GetTimerManager().SetTimerForNextTick(timerDelegate);

		ClimbingComponent = Character->FindComponentByClass<UClimbingComponent>();
	}
}

float UBOTWCharacterMovementComponent::GetMaxAcceleration() const
{
	if (IsMovingOnGround() == false || CurrentMovementSettings.MovementCurve == nullptr)
	{
		return Super::GetMaxAcceleration();
	}

	return CurrentMovementSettings.MovementCurve->GetVectorValue(GetMappedSpeed()).X;
}

float UBOTWCharacterMovementComponent::GetMaxBrakingDeceleration() const
{
	if (IsMovingOnGround() == false || CurrentMovementSettings.MovementCurve == nullptr)
	{
		return Super::GetMaxBrakingDeceleration();
	}

	return CurrentMovementSettings.MovementCurve->GetVectorValue(GetMappedSpeed()).X;
}

float UBOTWCharacterMovementComponent::GetMaxSpeed() const
{
	if (AbilitySystemComponent == nullptr || CharacterAttributeSet == nullptr)
	{
		return Super::GetMaxSpeed();
	}

	if (AbilitySystemComponent->HasMatchingGameplayTag(GAMEPLAYTAG.Character_State_Crouch))
	{
		return CharacterAttributeSet->GetCrouchMoveSpeed();
	}

	if (AbilitySystemComponent->HasMatchingGameplayTag(GAMEPLAYTAG.Character_State_Sprint))
	{
		return CharacterAttributeSet->GetSprintMoveSpeed();
	}

	if (AbilitySystemComponent->HasMatchingGameplayTag(GAMEPLAYTAG.Character_State_Climbing))
	{
		return CharacterAttributeSet->GetClimbingMoveSpeed();
	}

	return CharacterAttributeSet->GetRunMoveSpeed();
}

void UBOTWCharacterMovementComponent::OnMovementModeChanged(EMovementMode PreviousMovementMode, uint8 PreviousCustomMode)
{
	if (PreviousMovementMode == MOVE_Custom && PreviousCustomMode == ECustomMovementMode::MOVE_Climb)
	{
		const FRotator standRotation = FRotator(0.f, UpdatedComponent->GetComponentRotation().Yaw, 0.f);
		UpdatedComponent->SetRelativeRotation(standRotation);

		StopMovementImmediately();
	}

	Super::OnMovementModeChanged(PreviousMovementMode, PreviousCustomMode);
}

void UBOTWCharacterMovementComponent::PhysCustom(float deltaTime, int32 Iterations)
{
	if (MovementMode == MOVE_Custom && CustomMovementMode == ECustomMovementMode::MOVE_Climb)
	{
		if (deltaTime < MIN_TICK_TIME)
		{
			return;
		}

		FVector surfaceLocation = ClimbingComponent->GetSurfaceLocation();
		FVector surfaceNormal = ClimbingComponent->GetSurfaceNormal();

		RestorePreAdditiveRootMotionVelocity();

		if (!HasAnimRootMotion() && !CurrentRootMotion.HasOverrideVelocity())
		{
			CalcVelocity(deltaTime, 0.f, true, 400.f);
		}

		ApplyRootMotionToVelocity(deltaTime);

		FVector oldLocation = UpdatedComponent->GetComponentLocation();
		const FVector adjusted = Velocity * deltaTime;
		FHitResult hit(1.f);

		const FQuat climbRotation = GetClimbRotation(deltaTime, surfaceNormal);
		SafeMoveUpdatedComponent(adjusted, climbRotation, true, hit);

		if (hit.Time < 1.f)
		{
			HandleImpact(hit, deltaTime, adjusted);
			SlideAlongSurface(adjusted, (1.f - hit.Time), hit.Normal, hit, true);
		}

		if (!HasAnimRootMotion() && !CurrentRootMotion.HasOverrideVelocity())
		{
			Velocity = (UpdatedComponent->GetComponentLocation() - oldLocation) / deltaTime;
		}

		if (ClimbingComponent->GetEndedClimbing() == false && (surfaceNormal - climbRotation.Vector()).IsNearlyZero(0.1f))
		{
			SnapMovementToClimableSurfaces(deltaTime, surfaceLocation, surfaceNormal);
		}
	}

	Super::PhysCustom(deltaTime, Iterations);
}

float UBOTWCharacterMovementComponent::GetMappedSpeed() const
{
	if (CharacterAttributeSet == nullptr)
	{
		return 0.f;
	}

	if (AbilitySystemComponent->HasMatchingGameplayTag(GAMEPLAYTAG.Character_State_Climbing))
	{
		return 4.f;
	}

	float currentSpeed = Character->GetVelocity().Size2D();

	float walkSpeed = CharacterAttributeSet->GetWalkMoveSpeed();
	float runSpeed = CharacterAttributeSet->GetRunMoveSpeed();
	float sprintSpeed = CharacterAttributeSet->GetSprintMoveSpeed();

	if (runSpeed < currentSpeed)
	{
		return FMath::GetMappedRangeValueClamped<float, float>({ runSpeed, sprintSpeed }, { 2.f, 3.f }, currentSpeed);
	}

	if (walkSpeed < currentSpeed)
	{
		return FMath::GetMappedRangeValueClamped<float, float>({ walkSpeed, runSpeed }, { 1.f, 2.f }, currentSpeed);
	}

	return FMath::GetMappedRangeValueClamped<float, float>({ 0.f, walkSpeed }, { 0.f, 1.f }, currentSpeed);
}

void UBOTWCharacterMovementComponent::SnapMovementToClimableSurfaces(float DeltaTime, const FVector& _SurfaceLocation, const FVector& _SurfaceNormal)
{
	const FVector forwardVector = UpdatedComponent->GetForwardVector();
	const FVector location = UpdatedComponent->GetComponentLocation();

	const FVector projectedCharacterToSurface =
		(_SurfaceLocation - location).ProjectOnTo(forwardVector);

	const FVector snapVector = -_SurfaceNormal * projectedCharacterToSurface.Length();
	UpdatedComponent->MoveComponent(snapVector * DeltaTime * GetMaxSpeed(), UpdatedComponent->GetComponentQuat(), true);
}

FQuat UBOTWCharacterMovementComponent::GetClimbRotation(float DeltaTime, const FVector& _SurfaceNormal)
{
	const FQuat currentQuat = UpdatedComponent->GetComponentQuat();

	if (HasAnimRootMotion() || CurrentRootMotion.HasOverrideVelocity())
	{
		return currentQuat;
	}

	const FQuat targetQuat = FRotationMatrix::MakeFromX(-_SurfaceNormal).ToQuat();

	return FMath::QInterpTo(currentQuat, targetQuat, DeltaTime, 5.f);
}

void UBOTWCharacterMovementComponent::SetMovementSettings(FMovementSettings _NewMovementSettings)
{
	if (_NewMovementSettings.MovementCurve == nullptr || _NewMovementSettings.RotationRateCurve == nullptr)
	{
		return;
	}

	CurrentMovementSettings = _NewMovementSettings;
}
