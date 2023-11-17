#include "ClimbingComponent.h"

#include "Character/Player/PlayerCharacter.h"
#include "Controllers/BOTWPlayerController.h"
#include "Character/Components/BOTWCharacterMovementComponent.h"
#include "Character/Player/PlayerAnimInstance.h"
#include "Animation/AnimMontage.h"

#include "Character/Components/BOTWInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputMappingContext.h"
#include "InputActionValue.h"

#include "Kismet/KismetSystemLibrary.h"
#include "Components/CapsuleComponent.h"
#include "Kismet/KismetMathLibrary.h"


UClimbingComponent::UClimbingComponent()
	: IsDrawDebug(false)
	, CanStartClimbing(false)
	, EndedClimbing(false)
	, TraceUpDistance(80.f)
	, TraceForwardDistance(50.f)
	, ClimbingCapsuleHalfHeight(0.f)
	, OriginCapsuleHalfHeight(0.f)
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UClimbingComponent::BeginPlay()
{
	Super::BeginPlay();

	Player = GetOwner<APlayerCharacter>();
	check(Player);

	AbilitySystemComponent = Player->GetAbilitySystemComponent();
	check(AbilitySystemComponent);

	CharacterMovementComponent = Player->GetCharacterMovement<UBOTWCharacterMovementComponent>();
	check(CharacterMovementComponent);

	PlayerCapsuleComponent = Player->GetCapsuleComponent();
	check(PlayerCapsuleComponent);

	OriginCapsuleHalfHeight = PlayerCapsuleComponent->GetScaledCapsuleHalfHeight();

	BindClimbingActions();

	CharacterMovementComponent->AddTickPrerequisiteComponent(this);
}

void UClimbingComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	CanStartClimbing = CheckStartClimbing() ? true : false;

	if (AbilitySystemComponent->HasMatchingGameplayTag(GAMEPLAYTAG.Character_State_Climbing))
	{
		CalcSurfaceInfo();

		if (CheckFloor())
		{
			ReachFloor();
		}
		else if (CheckLedge())
		{
			ClimbUpLedge();
		}
	}
}

bool UClimbingComponent::CheckStartClimbing()
{
	if (TraceSurface() == false)
	{
		return false;
	}

	if (TraceHead().bBlockingHit == false)
	{
		return false;
	}

	return true;
}

void UClimbingComponent::StartClimbing()
{
	EndedClimbing = false;
	PlayerCapsuleComponent->SetCapsuleHalfHeight(ClimbingCapsuleHalfHeight);

	TraceSurface();
	CalcSurfaceInfo();

	Player->SetActorRotation(FRotationMatrix::MakeFromX(-SurfaceNormal).ToQuat());

	BindClimbingInputs();

	PlayMontage(Up_Start_AnimMontage);

	if (Player->GetMesh() && Player->GetMesh()->GetAnimInstance())
	{
		Player->GetMesh()->GetAnimInstance()->OnMontageEnded.AddDynamic(this, &UClimbingComponent::OnMontageEnded);
	}
}

void UClimbingComponent::EndClimbing()
{
	PlayerCapsuleComponent->SetCapsuleHalfHeight(OriginCapsuleHalfHeight);

	UnBindClimbingInputs();

	if (Player->GetMesh() && Player->GetMesh()->GetAnimInstance())
	{
		Player->GetMesh()->GetAnimInstance()->OnMontageEnded.RemoveDynamic(this, &UClimbingComponent::OnMontageEnded);
	}

	OnEndClimbing.Broadcast();

	SetComponentTickEnabled(true);
}

void UClimbingComponent::ReachFloor()
{
	EndClimbing();
}

void UClimbingComponent::ClimbUpLedge()
{
	SetComponentTickEnabled(false);
	EndedClimbing = true;

	PlayMontage(Up_End_AnimMontage);
}

void UClimbingComponent::BindClimbingActions()
{
	if (ClimbingInputConfig == nullptr)
	{
		return;
	}

	if (UBOTWInputComponent* enhancedInputComponent = Cast<UBOTWInputComponent>(Player->InputComponent))
	{
		enhancedInputComponent->BindNativeAction(ClimbingInputConfig, GAMEPLAYTAG.InputTag_MoveForward, ETriggerEvent::Triggered, this, &UClimbingComponent::Climbing_MoveForward);
		enhancedInputComponent->BindNativeAction(ClimbingInputConfig, GAMEPLAYTAG.InputTag_MoveSide, ETriggerEvent::Triggered, this, &UClimbingComponent::Climbing_MoveSide);
	}
}

void UClimbingComponent::BindClimbingInputs()
{
	if (ClimbingInputConfig == nullptr || ClimbingInputMappingContext == nullptr)
	{
		return;
	}

	if (ABOTWPlayerController* playerController = Player->GetController<ABOTWPlayerController>())
	{
		if (UEnhancedInputLocalPlayerSubsystem* subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(playerController->GetLocalPlayer()))
		{
			subsystem->AddMappingContext(ClimbingInputMappingContext, 2);
		}
	}
}

void UClimbingComponent::UnBindClimbingInputs()
{
	if (ABOTWPlayerController* playerController = Player->GetController<ABOTWPlayerController>())
	{
		if (UEnhancedInputLocalPlayerSubsystem* subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(playerController->GetLocalPlayer()))
		{
			subsystem->RemoveMappingContext(ClimbingInputMappingContext);
		}
	}
}

void UClimbingComponent::PlayMontage(UAnimMontage* _Montage)
{
	if (_Montage == nullptr)
	{
		return;
	}

	if (Player->GetMesh() && Player->GetMesh()->GetAnimInstance())
	{
		Player->GetMesh()->GetAnimInstance()->Montage_Play(_Montage);
	}
}

void UClimbingComponent::CalcSurfaceInfo()
{
	SurfaceLocation = FVector::ZeroVector;
	SurfaceNormal = FVector::ZeroVector;

	if (SurfaceHitResults.IsEmpty())
	{
		return;
	}

	for (const FHitResult& hitResult : SurfaceHitResults)
	{
		SurfaceLocation += hitResult.ImpactPoint;
		SurfaceNormal += hitResult.ImpactNormal;
	}

	SurfaceLocation /= SurfaceHitResults.Num();
	SurfaceNormal = SurfaceNormal.GetSafeNormal();
}

bool UClimbingComponent::TraceSurface(const FVector& _Offset)
{
	const FVector start = Player->GetActorLocation() + (Player->GetActorForwardVector() * TraceForwardDistance);
	const FVector end = start + Player->GetActorForwardVector();

	SurfaceHitResults = CapsuleTraceMulti(start, end);

	return SurfaceHitResults.IsEmpty() == false;
}

FHitResult UClimbingComponent::TraceHead()
{
	const FVector start = Player->GetActorLocation() + (Player->GetActorUpVector() * TraceUpDistance);
	const FVector end = start + (Player->GetActorForwardVector() * TraceForwardDistance);

	return LineTraceSingle(start, end);
}

bool UClimbingComponent::CheckFloor()
{
	const FVector downVector = -Player->GetActorUpVector();

	const FVector start = Player->GetActorLocation() + (downVector * 50.f);
	const FVector end = start + downVector;

	TArray<FHitResult> hitResults = CapsuleTraceMulti(start, end, 30.f, 50.f);
	if (hitResults.IsEmpty())
	{
		return false;
	}

	for (const FHitResult& hitResult : hitResults)
	{
		const FVector climbVelocity = GetClimbVelocity();
		const bool floorReached = FVector::Parallel(hitResult.ImpactNormal, FVector::UpVector) && climbVelocity.Z < -10.f;

		if (floorReached)
		{
			return true;
		}
	}

	return false;
}

bool UClimbingComponent::CheckLedge()
{
	FHitResult hitResult = TraceHead();

	if (hitResult.bBlockingHit == false)
	{
		const FVector traceStart = hitResult.TraceEnd;
		const FVector traceEnd = traceStart + (-Player->GetActorUpVector() * 100.f);

		FHitResult walkabkeSurfaceHitResult = LineTraceSingle(traceStart, traceEnd);

		if (walkabkeSurfaceHitResult.bBlockingHit && 100.f <= GetClimbVelocity().Z)
		{
			return true;
		}
	}

	return false;
}

TArray<FHitResult> UClimbingComponent::CapsuleTraceMulti(const FVector& _Start, const FVector& _End, float _Radius, float _HalfHeight)
{
	TArray<FHitResult> hitResults;

	EDrawDebugTrace::Type debugTraceType = IsDrawDebug ? EDrawDebugTrace::ForDuration : EDrawDebugTrace::None;

	TArray<TEnumAsByte<EObjectTypeQuery>> traceType;
	traceType.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_WorldStatic));
	traceType.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_WorldDynamic));

	UKismetSystemLibrary::CapsuleTraceMultiForObjects(this, _Start,	_End, _Radius, _HalfHeight, traceType, false, TArray<AActor*>(), debugTraceType, hitResults, true, FLinearColor::Red, FLinearColor::Green, GetWorld()->GetDeltaSeconds() + 0.01f);

	return hitResults;
}

FHitResult UClimbingComponent::LineTraceSingle(const FVector& _Start, const FVector& _End)
{
	FHitResult hitResult;

	EDrawDebugTrace::Type debugTraceType = IsDrawDebug ? EDrawDebugTrace::ForDuration : EDrawDebugTrace::None;

	TArray<TEnumAsByte<EObjectTypeQuery>> traceType;
	traceType.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_WorldStatic));
	traceType.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_WorldDynamic));

	UKismetSystemLibrary::LineTraceSingleForObjects(this, _Start, _End, traceType, false, TArray<AActor*>(), debugTraceType, hitResult, true, FLinearColor::Red, FLinearColor::Green, GetWorld()->GetDeltaSeconds() + 0.01f);

	return hitResult;
}

void UClimbingComponent::Climbing_MoveForward(const FInputActionValue& _Value)
{
	const FVector forwardDirection = FVector::CrossProduct(-SurfaceNormal, Player->GetActorRightVector());
	Player->AddMovementInput(forwardDirection, _Value.GetMagnitude());
}

void UClimbingComponent::Climbing_MoveSide(const FInputActionValue& _Value)
{
	const FVector sideDirection = FVector::CrossProduct(-SurfaceNormal, -Player->GetActorUpVector());
	Player->AddMovementInput(sideDirection, _Value.GetMagnitude());
}

void UClimbingComponent::OnMontageEnded(UAnimMontage* _Montage, bool _Interrupted)
{
	if (_Montage == Up_End_AnimMontage)
	{
		EndClimbing();
	}
}

FVector UClimbingComponent::GetClimbVelocity() const
{
	if (Player)
	{
		return Player->GetActorRotation().UnrotateVector(Player->GetVelocity());
	}

	return FVector::ZeroVector;
}
