#include "PlayerCombatComponent.h"

#include "Character/Player/PlayerCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "Curves/CurveFloat.h"

UPlayerCombatComponent::UPlayerCombatComponent()
	: TimeDilationStartTime(0.f)
	, TimeDilationEndTime(0.f)
	, KnockBackPower(100.f)
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UPlayerCombatComponent::BeginPlay()
{
	Super::BeginPlay();
	
	Player = Cast<APlayerCharacter>(GetOwner());
	check(Player);

	Player->GetOnLockOnDelegate().AddUObject(this, &UPlayerCombatComponent::LockOn);
}

void UPlayerCombatComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UPlayerCombatComponent::StartTimeDilation(UCurveFloat* _Curve, bool _IsForceOverride)
{
	if (_Curve == nullptr)
	{
		return;
	}

	if (TimeDilationTimerHandle.IsValid() && _IsForceOverride == false)
	{
		return;
	}

	TimeDilationCurve = _Curve;
	TimeDilationStartTime = GetWorld()->GetRealTimeSeconds();

	float minTime = 0.f;
	TimeDilationCurve->GetTimeRange(minTime, TimeDilationEndTime);

	UpdateTimeDilation();

	if (TimeDilationTimerHandle.IsValid())
	{
		GetWorld()->GetTimerManager().ClearTimer(TimeDilationTimerHandle);
	}

	TimeDilationTimerHandle = GetWorld()->GetTimerManager().SetTimerForNextTick(this, &UPlayerCombatComponent::UpdateTimeDilation);
}

void UPlayerCombatComponent::UpdateTimeDilation()
{
	float currentTime = GetWorld()->GetRealTimeSeconds() - TimeDilationStartTime;
	if (currentTime < TimeDilationEndTime)
	{
		float curveValue = TimeDilationCurve->GetFloatValue(currentTime);
		UGameplayStatics::SetGlobalTimeDilation(Player, curveValue);

		TimeDilationTimerHandle = GetWorld()->GetTimerManager().SetTimerForNextTick(this, &UPlayerCombatComponent::UpdateTimeDilation);
	}
	else
	{
		TimeDilationEndDelegate.Broadcast();

		TimeDilationTimerHandle.Invalidate();
		UGameplayStatics::SetGlobalTimeDilation(Player, 1.f);
	}
}

void UPlayerCombatComponent::LockOn(ACharacterBase* _LockOnTarget)
{
	LockOnTarget = _LockOnTarget;

	if (CameraAnimInstance)
	{
		CameraAnimInstance->SetIsLockOn(LockOnTarget != nullptr);
	}

	if (CameraManager)
	{
		CameraManager->SetLockOnTarget(LockOnTarget);
	}
}

const FWeaponAnim* UPlayerCombatComponent::GetWeaponAnimData(const EOverlayType& _OverlayType)
{
	if (MeleeAnimData && MeleeAnimData->Data.Contains(_OverlayType))
	{
		return &MeleeAnimData->Data[_OverlayType];
	}

	return nullptr;
}

UAnimMontage* UPlayerCombatComponent::GetShieldGuardImpactAnimMontage() const
{
	return ShieldGuardImpactAnimMontage;
}
