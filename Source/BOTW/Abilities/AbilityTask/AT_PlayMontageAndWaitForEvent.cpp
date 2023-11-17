#include "AT_PlayMontageAndWaitForEvent.h"

#include "Abilities/BOTWAbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"
#include "Character/CharacterBase.h"
#include "Animation/AnimInstance.h"

UAT_PlayMontageAndWaitForEvent::UAT_PlayMontageAndWaitForEvent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	Rate = 1.f;
	IsStopWhenAbilityEnds = true;
}

UAT_PlayMontageAndWaitForEvent* UAT_PlayMontageAndWaitForEvent::PlayMontageAndWaitForEvent(UGameplayAbility* _OwningAbility, UAnimMontage* _MontageToPlay, FGameplayTagContainer _EventTags /*= FGameplayTagContainer()*/, float _Rate /*= 1.f*/, FName _StartSection /*= NAME_None*/, bool _IsStopWhenAbilityEnds /*= true*/, float _AnimRootMotionTranslationScale /*= 1.f*/)
{
	UAbilitySystemGlobals::NonShipping_ApplyGlobalAbilityScaler_Rate(_Rate);

	UAT_PlayMontageAndWaitForEvent* task = NewAbilityTask<UAT_PlayMontageAndWaitForEvent>(_OwningAbility);
	task->MontageToPlay = _MontageToPlay;
	task->EventTags = _EventTags;
	task->Rate = _Rate;
	task->StartSection = _StartSection;
	task->AnimRootMotionTranslationScale = _AnimRootMotionTranslationScale;
	task->IsStopWhenAbilityEnds = _IsStopWhenAbilityEnds;

	return task;
}

void UAT_PlayMontageAndWaitForEvent::Activate()
{
	if (Ability == nullptr)
	{
		return;
	}

	bool isPlayedMontage = false;
	UBOTWAbilitySystemComponent* abilitySystemComponent = GetTargetASC();

	if (abilitySystemComponent)
	{
		const FGameplayAbilityActorInfo* actorInfo = Ability->GetCurrentActorInfo();
		UAnimInstance* animInstance = actorInfo->GetAnimInstance();
		if (animInstance != nullptr)
		{
			EventHandle = abilitySystemComponent->AddGameplayEventTagContainerDelegate(EventTags, FGameplayEventTagMulticastDelegate::FDelegate::CreateUObject(this, &UAT_PlayMontageAndWaitForEvent::OnGameplayEvent));

			if (0.f < abilitySystemComponent->PlayMontage(Ability, Ability->GetCurrentActivationInfo(), MontageToPlay, Rate, StartSection))
			{
				if (ShouldBroadcastAbilityTaskDelegates() == false)
				{
					return;
				}

				CancelledHandle = Ability->OnGameplayAbilityCancelled.AddUObject(this, &UAT_PlayMontageAndWaitForEvent::OnAbilityCancelled);

				BlendingOutDelegate.BindUObject(this, &UAT_PlayMontageAndWaitForEvent::OnMontageBlendingOut);
				animInstance->Montage_SetBlendingOutDelegate(BlendingOutDelegate, MontageToPlay);

				MontageEndedDelegate.BindUObject(this, &UAT_PlayMontageAndWaitForEvent::OnMontageEnded);
				animInstance->Montage_SetEndDelegate(MontageEndedDelegate, MontageToPlay);

				ACharacterBase* character = Cast<ACharacterBase>(GetAvatarActor());
				if (character)
				{
					character->SetAnimRootMotionTranslationScale(AnimRootMotionTranslationScale);
				}

				isPlayedMontage = true;
			}
		}
	}

	if (isPlayedMontage == false)
	{
		if (ShouldBroadcastAbilityTaskDelegates())
		{
			OnCancelled.Broadcast(FGameplayTag(), FGameplayEventData());
		}
	}

	SetWaitingOnAvatar();
}

void UAT_PlayMontageAndWaitForEvent::ExternalCancel()
{
	if (AbilitySystemComponent == nullptr)
	{
		return;
	}

	OnAbilityCancelled();

	Super::ExternalCancel();
}

void UAT_PlayMontageAndWaitForEvent::OnDestroy(bool AbilityEnded)
{
	if (Ability)
	{
		Ability->OnGameplayAbilityCancelled.Remove(CancelledHandle);
		if (AbilityEnded && IsStopWhenAbilityEnds)
		{
			StopPlayingMontage(OverrideBlendOutTimeForStopWhenEndAbility);
		}
	}

	UBOTWAbilitySystemComponent* abilitySystemComponent = GetTargetASC();
	if (abilitySystemComponent)
	{
		abilitySystemComponent->RemoveGameplayEventTagContainerDelegate(EventTags, EventHandle);
	}

	Super::OnDestroy(AbilityEnded);
}

void UAT_PlayMontageAndWaitForEvent::OnMontageBlendingOut(UAnimMontage* _Montage, bool _IsInterrupted)
{
	if (Ability && Ability->GetCurrentMontage() == MontageToPlay)
	{
		if (_Montage == MontageToPlay)
		{
			AbilitySystemComponent->ClearAnimatingAbility(Ability);

			ACharacterBase* character = Cast<ACharacterBase>(GetAvatarActor());
			if (character)
			{
				character->SetAnimRootMotionTranslationScale(1.f);
			}

		}
	}

	if (_IsInterrupted)
	{
		if (ShouldBroadcastAbilityTaskDelegates())
		{
			OnInterrupted.Broadcast(FGameplayTag(), FGameplayEventData());
		}
	}
	else
	{
		if (ShouldBroadcastAbilityTaskDelegates())
		{
			OnBlendOut.Broadcast(FGameplayTag(), FGameplayEventData());
		}
	}
}

void UAT_PlayMontageAndWaitForEvent::OnAbilityCancelled()
{
	if (StopPlayingMontage(OverrideBlendOutTimeForCancelAbility))
	{
		if (ShouldBroadcastAbilityTaskDelegates())
		{
			OnCancelled.Broadcast(FGameplayTag(), FGameplayEventData());
		}
	}
}

void UAT_PlayMontageAndWaitForEvent::OnMontageEnded(UAnimMontage* _Montage, bool _IsInterrupted)
{
	if (_IsInterrupted == false)
	{
		if (ShouldBroadcastAbilityTaskDelegates())
		{
			OnCompleted.Broadcast(FGameplayTag(), FGameplayEventData());
		}
	}

	EndTask();
}

void UAT_PlayMontageAndWaitForEvent::OnGameplayEvent(FGameplayTag _EventTag, const FGameplayEventData* _Payload)
{
	if (ShouldBroadcastAbilityTaskDelegates())
	{
		FGameplayEventData tempData = *_Payload;
		tempData.EventTag = _EventTag;

		EventReceived.Broadcast(_EventTag, tempData);
	}
}

bool UAT_PlayMontageAndWaitForEvent::StopPlayingMontage(float _OverrideBlendOutTime /*= -1.f*/)
{
	UBOTWAbilitySystemComponent* abilitySystemComponent = GetTargetASC();
	if (abilitySystemComponent == nullptr)
	{
		return false;
	}

	const FGameplayAbilityActorInfo* actorInfo = Ability->GetCurrentActorInfo();
	UAnimInstance* animInstance = actorInfo->GetAnimInstance();
	if (actorInfo == nullptr || animInstance == nullptr)
	{
		return false;
	}

	if (abilitySystemComponent && Ability)
	{
		if (abilitySystemComponent->GetAnimatingAbility() == Ability
			&& abilitySystemComponent->GetCurrentMontage() == MontageToPlay)
		{
			FAnimMontageInstance* montageInstance = animInstance->GetActiveInstanceForMontage(MontageToPlay);
			if (montageInstance)
			{
				montageInstance->OnMontageBlendingOutStarted.Unbind();
				montageInstance->OnMontageEnded.Unbind();
			}

			abilitySystemComponent->CurrentMontageStop(_OverrideBlendOutTime);
			return true;
		}
	}

	return false;
}

UBOTWAbilitySystemComponent* UAT_PlayMontageAndWaitForEvent::GetTargetASC()
{
	return Cast<UBOTWAbilitySystemComponent>(AbilitySystemComponent);
}
