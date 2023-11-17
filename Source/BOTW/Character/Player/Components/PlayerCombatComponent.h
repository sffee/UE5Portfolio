#pragma once

#include "Interface/CombatInterface.h"

#include "Character/Player/Camera/BOTWCameraAnimInstance.h"
#include "Character/Player/Camera/BOTWPlayerCameraManager.h"

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "PlayerCombatComponent.generated.h"

class UCurveFloat;
class ACharacterBase;
class APlayerCharacter;
class UAnimMontage;

UCLASS(Blueprintable, ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class BOTW_API UPlayerCombatComponent : public UActorComponent, public ICombatInterface
{
	GENERATED_BODY()

private:
	UPROPERTY()
	APlayerCharacter* Player;

	UPROPERTY()
	UBOTWCameraAnimInstance* CameraAnimInstance;

	UPROPERTY()
	ABOTWPlayerCameraManager* CameraManager;

private:
	UPROPERTY()
	ACharacterBase* LockOnTarget;

private:
	UPROPERTY(EditDefaultsOnly, Category = "BOTW|Animation")
	UWeaponAnimationData* MeleeAnimData;

	UPROPERTY(EditDefaultsOnly, Category = "BOTW|Animation")
	UAnimMontage* ShieldGuardImpactAnimMontage;

private:
	UPROPERTY()
	UCurveFloat* TimeDilationCurve;

	float TimeDilationStartTime;
	float TimeDilationEndTime;

	FTimerHandle TimeDilationTimerHandle;

	FSimpleMulticastDelegate TimeDilationEndDelegate;

private:
	UPROPERTY(EditDefaultsOnly, Category = "BOTW|KnockBack")
	float KnockBackPower;

public:	
	UPlayerCombatComponent();

protected:
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

public:
	void StartTimeDilation(UCurveFloat* _Curve, bool _IsForceOverride = false);
	void UpdateTimeDilation();

private:
	void LockOn(ACharacterBase* _LockOnTarget);

public:
	void SetCameraAnimInstance(UBOTWCameraAnimInstance* _CameraAnimInstance) { CameraAnimInstance = _CameraAnimInstance; }
	void SetCameraManager(ABOTWPlayerCameraManager* _CameraManager) { CameraManager = _CameraManager; }

public:
	virtual const FWeaponAnim* GetWeaponAnimData(const EOverlayType& _OverlayType) override;
	virtual UAnimMontage* GetShieldGuardImpactAnimMontage() const override;
	virtual float GetKnockBackPower() const { return KnockBackPower; }

	FORCEINLINE ACharacterBase* GetLockOnTarget() const { return LockOnTarget; }

public:
	FORCEINLINE FSimpleMulticastDelegate& GetTimeDilationEndDelegate() { return TimeDilationEndDelegate; }
};
