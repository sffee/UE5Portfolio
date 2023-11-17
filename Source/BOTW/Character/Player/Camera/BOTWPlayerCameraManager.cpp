#include "BOTWPlayerCameraManager.h"

#include "BOTWCameraAnimInstance.h"
#include "Character/Player/PlayerCharacter.h"
#include "Character/Components/LocomotionComponent.h"
#include "Character/Player/Components/PlayerCombatComponent.h"
#include "Character/Enemy/EnemyCharacter.h"
#include "Curves/CurveFloat.h"
#include "Materials/Material.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Kismet/KismetMathLibrary.h"

const FName NAME_CameraBehavior(TEXT("CameraBehavior"));
const FName NAME_CameraOffset_X(TEXT("CameraOffset_X"));
const FName NAME_CameraOffset_Y(TEXT("CameraOffset_Y"));
const FName NAME_CameraOffset_Z(TEXT("CameraOffset_Z"));
const FName NAME_PivotLagSpeed_X(TEXT("PivotLagSpeed_X"));
const FName NAME_PivotLagSpeed_Y(TEXT("PivotLagSpeed_Y"));
const FName NAME_PivotLagSpeed_Z(TEXT("PivotLagSpeed_Z"));
const FName NAME_PivotOffset_X(TEXT("PivotOffset_X"));
const FName NAME_PivotOffset_Y(TEXT("PivotOffset_Y"));
const FName NAME_PivotOffset_Z(TEXT("PivotOffset_Z"));
const FName NAME_RotationLagSpeed(TEXT("RotationLagSpeed"));

ABOTWPlayerCameraManager::ABOTWPlayerCameraManager()
	: RushBlurWeight(0.f)
	, RushBlurStartTime(0.f)
	, RushBlurEndTime(0.f)
	, LockOnTargetRotationSpeed(10.f)
{
	CameraBehavior = CreateDefaultSubobject<USkeletalMeshComponent>(NAME_CameraBehavior);
	CameraBehavior->SetupAttachment(GetRootComponent());
	CameraBehavior->bHiddenInGame = true;
}

void ABOTWPlayerCameraManager::UpdateViewTargetInternal(FTViewTarget& _OutVT, float _DeltaTime)
{
	if (_OutVT.Target)
	{
		FVector outLocation;
		FRotator outRotation;
		float _outFOV;

		if (_OutVT.Target->IsA<ACharacterBase>())
		{
			if (CustomCameraBehavior(_DeltaTime, outLocation, outRotation, _outFOV))
			{
				_OutVT.POV.Location = outLocation;
				_OutVT.POV.Rotation = outRotation;
				_OutVT.POV.FOV = _outFOV;
			}
			else
			{
				_OutVT.Target->CalcCamera(_DeltaTime, _OutVT.POV);
			}
		}
		else
		{
			_OutVT.Target->CalcCamera(_DeltaTime, _OutVT.POV);
		}
	}
}

void ABOTWPlayerCameraManager::ApplyCameraModifiers(float DeltaTime, FMinimalViewInfo& InOutPOV)
{
	Super::ApplyCameraModifiers(DeltaTime, InOutPOV);

	AddCachedPPBlend(PostProcessSettings, 0.f < RushBlurWeight ? 1.f : 0.f);
}

void ABOTWPlayerCameraManager::OnPossess(APlayerCharacter* _Player)
{
	check(_Player);
	Player = _Player;

	UBOTWCameraAnimInstance* cameraAnimInstance = Cast<UBOTWCameraAnimInstance>(CameraBehavior->GetAnimInstance());
	ULocomotionComponent* locomotionComponent = Cast<ULocomotionComponent>(Player->GetComponentByClass(ULocomotionComponent::StaticClass()));
	UPlayerCombatComponent* playerCombatComponent = Cast<UPlayerCombatComponent>(Player->GetComponentByClass(UPlayerCombatComponent::StaticClass()));
	if (cameraAnimInstance && locomotionComponent && playerCombatComponent)
	{
		locomotionComponent->SetCameraAnimInstance(cameraAnimInstance);
		playerCombatComponent->SetCameraAnimInstance(cameraAnimInstance);
		playerCombatComponent->SetCameraManager(this);
		cameraAnimInstance->SetMovementState(locomotionComponent->GetMovementState());
		cameraAnimInstance->SetMovementAction(locomotionComponent->GetMovementAction());
		cameraAnimInstance->SetGait(locomotionComponent->GetGait());
		cameraAnimInstance->SetRotationMode(locomotionComponent->GetRotationMode());
		cameraAnimInstance->SetStance(locomotionComponent->GetStance());
	}

	const FVector& playerLoc = Player->GetActorLocation();
	SetActorLocation(playerLoc);
	SmoothedPivotTarget.SetLocation(playerLoc);

	if (BlurMaterial)
	{
		BlurMaterialInstance = UMaterialInstanceDynamic::Create(BlurMaterial, this);
		if (BlurMaterialInstance)
		{
			FWeightedBlendable weightBlendable;
			weightBlendable.Object = BlurMaterialInstance;
			weightBlendable.Weight = 1.f;
			PostProcessSettings.WeightedBlendables.Array.Add(weightBlendable);
		}
	}
}

float ABOTWPlayerCameraManager::GetCameraAnimInstanceParam(FName _CurveName) const
{
	if (CameraAnimInstance)
	{
		return CameraAnimInstance->GetCurveValue(_CurveName);
	}

	return 0.f;
}

FVector ABOTWPlayerCameraManager::CalculateAxisIndependentLag(FVector _CurrentLocation, FVector _TargetLocation, FRotator _CameraRotation, FVector _LagSpeeds, float _DeltaTime)
{
	_CameraRotation.Roll = 0.f;
	_CameraRotation.Pitch = 0.f;

	const FVector unrotatedCurLoc = _CameraRotation.UnrotateVector(_CurrentLocation);
	const FVector unrotatedTargetLoc = _CameraRotation.UnrotateVector(_TargetLocation);

	const FVector resultVector(
		FMath::FInterpTo(unrotatedCurLoc.X, unrotatedTargetLoc.X, _DeltaTime, _LagSpeeds.X),
		FMath::FInterpTo(unrotatedCurLoc.Y, unrotatedTargetLoc.Y, _DeltaTime, _LagSpeeds.Y),
		FMath::FInterpTo(unrotatedCurLoc.Z, unrotatedTargetLoc.Z, _DeltaTime, _LagSpeeds.Z));

	return _CameraRotation.RotateVector(resultVector);
}

bool ABOTWPlayerCameraManager::CustomCameraBehavior(float _DeltaTime, FVector& _Location, FRotator& _Rotation, float& _FOV)
{
	if (Player == nullptr)
	{
		return false;
	}

	CameraAnimInstance = CameraAnimInstance == nullptr ? Cast<UBOTWCameraAnimInstance>(CameraBehavior->GetAnimInstance()) : CameraAnimInstance;
	if (CameraAnimInstance == nullptr)
	{
		return false;
	}

	LocomotionComponent = LocomotionComponent == nullptr ? Player->GetLocomotionComponent() : LocomotionComponent;
	if (LocomotionComponent == nullptr)
	{
		return false;
	}

	if (LockOnTarget)
	{
		UpdateLockOnCamera(_DeltaTime);
	}

	const FVector pivotTargetLoc = (Player->GetMesh()->GetSocketLocation(TEXT("Head")) + Player->GetMesh()->GetSocketLocation(TEXT("root"))) * 0.5f;
	const FRotator pivotTargetRot = Player->GetActorRotation();

	TargetCameraRotation = FMath::RInterpTo(GetCameraRotation(),
		GetOwningPlayerController()->GetControlRotation(), _DeltaTime,
		GetCameraAnimInstanceParam(NAME_RotationLagSpeed));

	const FVector lagSpd(GetCameraAnimInstanceParam(NAME_PivotLagSpeed_X),
		GetCameraAnimInstanceParam(NAME_PivotLagSpeed_Y),
		GetCameraAnimInstanceParam(NAME_PivotLagSpeed_Z));

	const FVector& axisIndpLag = CalculateAxisIndependentLag(SmoothedPivotTarget.GetLocation(),
		pivotTargetLoc, TargetCameraRotation, lagSpd, _DeltaTime);

	SmoothedPivotTarget.SetRotation(pivotTargetRot.Quaternion());
	SmoothedPivotTarget.SetLocation(axisIndpLag);
	SmoothedPivotTarget.SetScale3D(FVector::OneVector);

	PivotLocation =
		SmoothedPivotTarget.GetLocation() +
		UKismetMathLibrary::GetForwardVector(SmoothedPivotTarget.Rotator()) * GetCameraAnimInstanceParam(NAME_PivotOffset_X) +
		UKismetMathLibrary::GetRightVector(SmoothedPivotTarget.Rotator()) * GetCameraAnimInstanceParam(NAME_PivotOffset_Y) +
		UKismetMathLibrary::GetUpVector(SmoothedPivotTarget.Rotator()) * GetCameraAnimInstanceParam(NAME_PivotOffset_Z);

	TargetCameraLocation = PivotLocation +
		UKismetMathLibrary::GetForwardVector(TargetCameraRotation) * GetCameraAnimInstanceParam(NAME_CameraOffset_X) +
		UKismetMathLibrary::GetRightVector(TargetCameraRotation) * GetCameraAnimInstanceParam(NAME_CameraOffset_Y) +
		UKismetMathLibrary::GetUpVector(TargetCameraRotation) * GetCameraAnimInstanceParam(NAME_CameraOffset_Z);

	FVector traceOrigin = Player->GetActorLocation();
	float traceRadius = 10.f;
	ECollisionChannel traceChannel = ECC_Visibility;

	UWorld* world = GetWorld();
	check(world);

	FCollisionQueryParams params;
	params.AddIgnoredActor(this);
	params.AddIgnoredActor(Player);

	FHitResult hitResult;
	const FCollisionShape sphereCollisionShape = FCollisionShape::MakeSphere(traceRadius);
	const bool isHit = world->SweepSingleByChannel(hitResult, traceOrigin, TargetCameraLocation, FQuat::Identity, traceChannel, sphereCollisionShape, params);

	if (hitResult.IsValidBlockingHit())
	{
		TargetCameraLocation += hitResult.Location - hitResult.TraceEnd;
	}

	_Location = TargetCameraLocation;
	_Rotation = TargetCameraRotation;
	_FOV = 90.f;

	return true;
}

void ABOTWPlayerCameraManager::UpdateLockOnCamera(float _DeltaTime)
{
	APlayerController* pc = GetOwningPlayerController();

	const FRotator controlRotation = pc->GetControlRotation();
	FRotator targetRotation = (LockOnTarget->GetActorLocation() - Player->GetActorLocation()).GetSafeNormal().Rotation();
	targetRotation.Pitch = controlRotation.Pitch;
	targetRotation.Roll = controlRotation.Roll;

	const FRotator targetControlRotation = FMath::RInterpTo(controlRotation, targetRotation, _DeltaTime, LockOnTargetRotationSpeed);

	pc->SetControlRotation(targetControlRotation);
}

void ABOTWPlayerCameraManager::UpdateRushBlur()
{
	float currentTime = GetWorld()->GetRealTimeSeconds() - RushBlurStartTime;
	if (currentTime < RushBlurEndTime)
	{
		RushBlurWeight = CurrentRushBlurCurve->GetFloatValue(currentTime);
		BlurMaterialInstance->SetScalarParameterValue(FName("Weight"), RushBlurWeight);

		GetWorld()->GetTimerManager().SetTimerForNextTick(this, &ABOTWPlayerCameraManager::UpdateRushBlur);
	}
	else
	{
		if (CurrentRushBlurCurve == RushBlurOutCurve)
		{
			RushBlurWeight = 0.f;
			BlurMaterialInstance->SetScalarParameterValue(FName("Weight"), 0.f);
		}
	}
}

void ABOTWPlayerCameraManager::SetIsApplyRushBlur(bool _Set)
{
	if (RushBlurInCurve == nullptr || RushBlurOutCurve == nullptr || BlurMaterialInstance == nullptr)
	{
		return;
	}

	RushBlurStartTime = GetWorld()->GetRealTimeSeconds();
	CurrentRushBlurCurve = _Set ? RushBlurInCurve : RushBlurOutCurve;

	float minTime = 0.f;
	CurrentRushBlurCurve->GetTimeRange(minTime, RushBlurEndTime);

	GetWorldTimerManager().SetTimerForNextTick(this, &ABOTWPlayerCameraManager::UpdateRushBlur);
}