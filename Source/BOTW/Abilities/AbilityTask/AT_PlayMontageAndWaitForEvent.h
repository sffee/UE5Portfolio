#pragma once

#include "CoreMinimal.h"
#include "Abilities/Tasks/AbilityTask.h"
#include "AT_PlayMontageAndWaitForEvent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FPlayMontageAndWaitForEventDelegate, FGameplayTag, EventTag, FGameplayEventData, EventData);

class UBOTWAbilitySystemComponent;

UCLASS()
class BOTW_API UAT_PlayMontageAndWaitForEvent : public UAbilityTask
{
	GENERATED_BODY()
	
public:
	FPlayMontageAndWaitForEventDelegate OnCompleted;
	FPlayMontageAndWaitForEventDelegate OnBlendOut;
	FPlayMontageAndWaitForEventDelegate OnInterrupted;
	FPlayMontageAndWaitForEventDelegate OnCancelled;
	FPlayMontageAndWaitForEventDelegate EventReceived;

	FOnMontageBlendingOutStarted BlendingOutDelegate;
	FOnMontageEnded MontageEndedDelegate;
	FDelegateHandle CancelledHandle;
	FDelegateHandle EventHandle;

private:
	UPROPERTY()
	UAnimMontage* MontageToPlay;

	FGameplayTagContainer EventTags;

	float Rate;
	FName StartSection;
	float AnimRootMotionTranslationScale;
	bool IsStopWhenAbilityEnds;
	bool IsReplicateMontage;
	float OverrideBlendOutTimeForCancelAbility;
	float OverrideBlendOutTimeForStopWhenEndAbility;

public:
	UFUNCTION()
	static UAT_PlayMontageAndWaitForEvent* PlayMontageAndWaitForEvent(
		UGameplayAbility* _OwningAbility,
		UAnimMontage* _MontageToPlay,
		FGameplayTagContainer _EventTags = FGameplayTagContainer(),
		float _Rate = 1.f,
		FName _StartSection = NAME_None,
		bool _IsStopWhenAbilityEnds = true,
		float _AnimRootMotionTranslationScale = 1.f);

public:
	UAT_PlayMontageAndWaitForEvent(const FObjectInitializer& ObjectInitializer);

	virtual void Activate() override;
	virtual void ExternalCancel() override;
	virtual void OnDestroy(bool AbilityEnded) override;

public:
	void OnMontageBlendingOut(UAnimMontage* _Montage, bool _IsInterrupted);
	void OnAbilityCancelled();
	void OnMontageEnded(UAnimMontage* _Montage, bool _IsInterrupted);
	void OnGameplayEvent(FGameplayTag _EventTag, const FGameplayEventData* _Payload);

public:
	bool StopPlayingMontage(float _OverrideBlendOutTime = -1.f);

public:
	UBOTWAbilitySystemComponent* GetTargetASC();
};
