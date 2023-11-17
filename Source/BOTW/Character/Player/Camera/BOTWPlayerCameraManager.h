#pragma once

#include "CoreMinimal.h"
#include "Camera/PlayerCameraManager.h"
#include "BOTWPlayerCameraManager.generated.h"

class ACharacterBase;
class APlayerCharacter;
class UBOTWCameraAnimInstance;
class ULocomotionComponent;
class UCurveFloat;
class UMaterial;
class UMaterialInstanceDynamic;

UCLASS()
class BOTW_API ABOTWPlayerCameraManager : public APlayerCameraManager
{
	GENERATED_BODY()
	
public:
	UPROPERTY()
	APlayerCharacter* Player;

	UPROPERTY()
	UBOTWCameraAnimInstance* CameraAnimInstance;

	UPROPERTY()
	ULocomotionComponent* LocomotionComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "BOTW|Camera")
	USkeletalMeshComponent* CameraBehavior;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "BOTW|Camera")
	FVector RootLocation;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "BOTW|Camera")
	FTransform SmoothedPivotTarget;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "BOTW|Camera")
	FVector PivotLocation;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "BOTW|Camera")
	FVector TargetCameraLocation;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "BOTW|Camera")
	FRotator TargetCameraRotation;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "BOTW|RushBlur")
	UMaterial* BlurMaterial;

	UMaterialInstanceDynamic* BlurMaterialInstance;
	FPostProcessSettings PostProcessSettings;

	float RushBlurWeight;

	float RushBlurStartTime;
	float RushBlurEndTime;
	UCurveFloat* CurrentRushBlurCurve;

	UPROPERTY(EditDefaultsOnly, Category = "BOTW|RushBlur")
	UCurveFloat* RushBlurInCurve;

	UPROPERTY(EditDefaultsOnly, Category = "BOTW|RushBlur")
	UCurveFloat* RushBlurOutCurve;

protected:
	UPROPERTY()
	ACharacterBase* LockOnTarget;

	UPROPERTY(EditDefaultsOnly, Category = "BOTW|Camera")
	float LockOnTargetRotationSpeed;

public:
	ABOTWPlayerCameraManager();

	void OnPossess(APlayerCharacter* _Player);

public:
	virtual void UpdateViewTargetInternal(FTViewTarget& _OutVT, float _DeltaTime) override;
	virtual void ApplyCameraModifiers(float DeltaTime, FMinimalViewInfo& InOutPOV) override;

private:
	float GetCameraAnimInstanceParam(FName _CurveName) const;

protected:
	FVector CalculateAxisIndependentLag(FVector _CurrentLocation, FVector _TargetLocation, FRotator _CameraRotation, FVector _LagSpeeds, float _DeltaTime);
	bool CustomCameraBehavior(float _DeltaTime, FVector& _Location, FRotator& _Rotation, float& _FOV);

	void UpdateLockOnCamera(float _DeltaTime);

	void UpdateRushBlur();

public:
	FORCEINLINE void SetLockOnTarget(ACharacterBase* _LockOnTarget) { LockOnTarget = _LockOnTarget; }
	void SetIsApplyRushBlur(bool _Set);
};