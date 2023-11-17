#pragma once

#include "Library/LocomotionEnumLibrary.h"
#include "Library/LocomotionStructLibrary.h"
#include "Library/EquipmentEnumLibrary.h"
#include "Library/GASLibrary.h"

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "CharacterAnimInstanceBase.generated.h"

class ACharacterBase;
class UCurveFloat;
class UCurveVector;
class ULocomotionComponent;
class UEquipmentInstance;
class UCharacterAttributeSetBase;

UCLASS()
class BOTW_API UCharacterAnimInstanceBase : public UAnimInstance
{
	GENERATED_BODY()
	
protected:
	UPROPERTY()
	ACharacterBase* Character;

	UPROPERTY()
	ULocomotionComponent* LocomotionComponent;

	UPROPERTY()
	UAbilitySystemComponent* AbilitySystemComponent;

	UPROPERTY()
	const UCharacterAttributeSetBase* CharacterAttributeSet;

protected:
	UPROPERTY(BlueprintReadOnly, Category = "BOTW|Character Information")
	FAnimCharacterInformation CharacterInformation;

	UPROPERTY(BlueprintReadOnly, Category = "BOTW|Character Information")
	FMovementState MovementState;

	UPROPERTY(BlueprintReadOnly, Category = "BOTW|Character Information")
	FMovementAction MovementAction;

	UPROPERTY(BlueprintReadOnly, Category = "BOTW|Character Information")
	FRotationMode RotationMode;

	UPROPERTY(BlueprintReadOnly, Category = "BOTW|Character Information")
	FGait Gait;

	UPROPERTY(BlueprintReadOnly, Category = "BOTW|Character Information")
	FStance Stance;

protected:
	UPROPERTY(BlueprintReadOnly, Category = "BOTW|Anim Graph - Grounded")
	FAnimGraphGrounded Grounded;

	UPROPERTY(BlueprintReadOnly, Category = "BOTW|Anim Graph - Grounded")
	FVelocityBlend VelocityBlend;

	UPROPERTY(BlueprintReadOnly, Category = "BOTW|Anim Graph - Grounded")
	FLeanAmount LeanAmount;

	UPROPERTY(BlueprintReadOnly, Category = "BOTW|Anim Graph - Grounded")
	FVector RelativeAccelerationAmount;

	UPROPERTY(BlueprintReadOnly, Category = "BOTW|Anim Graph - Grounded")
	FGroundedEntryState GroundedEntryState;

	UPROPERTY(BlueprintReadOnly, Category = "BOTW|Anim Graph - Grounded")
	FMovementDirection MovementDirection;

protected:
	UPROPERTY(BlueprintReadOnly, Category = "BOTW|Anim Graph - In Air")
	FAnimGraphInAir InAir;

protected:
	UPROPERTY(BlueprintReadOnly, Category = "BOTW|Anim Graph - Aiming Values")
	FAnimGraphAimingValues AimingValues;

	UPROPERTY(BlueprintReadOnly, Category = "BOTW|Anim Graph - Aiming Values")
	FVector2D SmoothedAimingAngle;

	UPROPERTY(BlueprintReadOnly, Category = "BOTW|Anim Graph - Aiming Values")
	float FlailRate;

protected:
	UPROPERTY(BlueprintReadOnly, Category = "BOTW|Anim Graph - Layer Blending")
	FAnimGraphLayerBlending LayerBlendingValues;

protected:
	UPROPERTY(BlueprintReadOnly, Category = "BOTW|Anim Graph - Foot IK")
	FAnimGraphFootIK FootIKValues;

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BOTW|Rotate In Place")
	FAnimRotateInPlace RotateInPlace;

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BOTW|Main Configuration")
	FAnimConfiguration Config;

private:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BOTW|Blend Curves", meta = (AllowPrivateAccess = true))
	UCurveFloat* DiagonalScaleAmountCurve;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BOTW|Blend Curves", meta = (AllowPrivateAccess = true))
	UCurveFloat* StrideBlend_N_Walk;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BOTW|Blend Curves", meta = (AllowPrivateAccess = true))
	UCurveFloat* StrideBlend_N_Run;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BOTW|Blend Curves", meta = (AllowPrivateAccess = true))
	UCurveFloat* StrideBlend_C_Walk;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BOTW|Blend Curves", meta = (AllowPrivateAccess = true))
	UCurveFloat* LandPredictionCurve;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BOTW|Blend Curves", meta = (AllowPrivateAccess = true))
	UCurveFloat* LeanInAirCurve;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BOTW|Blend Curves", meta = (AllowPrivateAccess = true))
	UCurveVector* YawOffset_FB;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BOTW|Blend Curves", meta = (AllowPrivateAccess = true))
	UCurveVector* YawOffset_LR;

private:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BOTW|Dynamic Transition", meta = (AllowPrivateAccess = true))
	UAnimSequenceBase* TransitionAnim_R;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BOTW|Dynamic Transition", meta = (AllowPrivateAccess = true))
	UAnimSequenceBase* TransitionAnim_L;

private:
	/** IK Bone Names */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BOTW|Anim Graph - Foot IK", meta = (AllowPrivateAccess = true))
	FName IkFootL_BoneName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BOTW|Anim Graph - Foot IK", meta = (AllowPrivateAccess = true))
	FName IkFootR_BoneName;

protected:
	UPROPERTY(BlueprintReadOnly)
	EOverlayType OverlayType;

	UPROPERTY(BlueprintReadOnly)
	bool IsEquippedShield;

	UPROPERTY(BlueprintReadOnly)
	bool IsShieldGuard;

private:
	FTimerHandle OnPivotTimer;
	FTimerHandle PlayDynamicTransitionTimer;
	FTimerHandle OnJumpedTimer;

	bool CanPlayDynamicTransition;

private:
	FDelegateHandle ShieldGuardGameplayTagDelegateHandle;
	
public:
	UCharacterAnimInstanceBase();

	virtual void NativeInitializeAnimation() override;
	virtual void NativeBeginPlay() override;
	virtual void NativeUninitializeAnimation() override;
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

	void UpdateLocomotion(float _DeltaSeconds);

public:
	UFUNCTION()
	void OnJumped();

	UFUNCTION(BlueprintCallable, Category = "BOTW")
	void OnPivot();

protected:
	bool ShouldMoveCheck() const;
	bool CanRotateInPlace() const;
	bool CanDynamicTransition() const;

protected:
	UFUNCTION(BlueprintCallable, Category = "BOTW|Animation")
	void PlayTransition(const FDynamicMontageParams& _Parameters);

	UFUNCTION(BlueprintCallable, Category = "BOTW|Animation")
	void PlayTransitionChecked(const FDynamicMontageParams& _Parameters);

	UFUNCTION(BlueprintCallable, Category = "BOTW|Animation")
	void PlayDynamicTransition(float _ReTriggerDelay, FDynamicMontageParams _Parameters);

	UFUNCTION(BlueprintCallable, Category = "BOTW|Grounded")
	void SetTrackedHipsDirection(EHipsDirection _HipsDirection);

	UFUNCTION(BlueprintCallable, Category = "BOTW|Grounded")
	void SetGroundedEntryState(EGroundedEntryState _NewState);

private:
	void PlayDynamicTransitionDelay();
	void OnJumpedDelay();
	void OnPivotDelay();

private:
	FVector CalculateRelativeAccelerationAmount() const;
	float CalculateStrideBlend() const;
	float CalculateWalkRunBlend() const;
	float CalculateStandingPlayRate() const;
	float CalculateDiagonalScaleAmount() const;
	float CalculateCrouchingPlayRate() const;
	float CalculateLandPrediction() const;
	FLeanAmount CalculateAirLeanAmount() const;
	EMovementDirection CalculateMovementDirection() const;

private:
	void UpdateAimingValues(float _DeltaSeconds);
	void UpdateLayerValues();
	void UpdateFootIK(float _DeltaSeconds);
	void UpdateMovementValues(float _DeltaSeconds);
	void UpdateRotationValues();
	void UpdateInAirValues(float _DeltaSeconds);

private:
	void SetFootLocking(float _DeltaSeconds, FName _EnableFootIKCurve, FName _FootLockCurve, FName _IKFootBone, float& _CurFootLockAlpha, bool& UseFootLockCurve, FVector& _CurFootLockLoc, FRotator& _CurFootLockRot);
	void SetFootLockOffsets(float _DeltaSeconds, FVector& _LocalLoc, FRotator& _LocalRot);
	void SetPelvisIKOffset(float _DeltaSeconds, FVector _FootOffsetLTarget, FVector _FootOffsetRTarget);
	void ResetIKOffsets(float _DeltaSeconds);
	void SetFootOffsets(float _DeltaSeconds, FName _EnableFootIKCurve, FName _IKFootBone, FName _RootBone, FVector& _CurLocationTarget, FVector& _CurLocationOffset, FRotator& _CurRotationOffset);

public:
	void SetOverlayType(EOverlayType _OverlayType);

private:
	void RotateInPlaceCheck();
	void DynamicTransitionCheck();
	FVelocityBlend CalculateVelocityBlend() const;

private:
	UFUNCTION()
	void OnEquippedWeaponChange(UEquipmentInstance* _PrevEquipInstance, UEquipmentInstance* _NewEquipInstance);

	UFUNCTION()
	void OnEquippedShieldChange(UEquipmentInstance* _PrevEquipInstance, UEquipmentInstance* _NewEquipInstance);

	void OnGameplayTagChange(const FGameplayTag _Tag, int32 _Count);

private:
	float GetAnimCurveClamped(const FName& _Name, float _Bias, float _ClampMin, float _ClampMax) const;
};
