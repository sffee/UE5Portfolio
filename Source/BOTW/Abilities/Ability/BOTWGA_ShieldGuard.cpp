#include "BOTWGA_ShieldGuard.h"

#include "Abilities/BOTWAbilitySystemComponent.h"
#include "Abilities/AbilityTask/AT_PlayMontageAndWaitForEvent.h"
#include "GameFramework/Character.h"
#include "Interface/CombatInterface.h"
#include "MotionWarpingComponent.h"

UBOTWGA_ShieldGuard::UBOTWGA_ShieldGuard()
	: IsShieldImpactPlaying(false)
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;

	UGameplayTagsManager::Get().CallOrRegister_OnDoneAddingNativeTagsDelegate(FSimpleDelegate::CreateLambda([&]
	{
		AbilityInputTag = GAMEPLAYTAG.InputTag_ShieldGuard;
		AbilityTags.AddTag(GAMEPLAYTAG.Character_State_ShieldGuard);
		ActivationOwnedTags.AddTag(GAMEPLAYTAG.Character_State_ShieldGuard);
	}));
}

void UBOTWGA_ShieldGuard::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	AbilitySystemComponent = AbilitySystemComponent == nullptr ? GetBOTWAbilitySystemComponent() : AbilitySystemComponent;
	if (AbilitySystemComponent)
	{
		GameplayEventTagDelegateHandle = AbilitySystemComponent->AddGameplayEventTagContainerDelegate(FGameplayTagContainer(GAMEPLAYTAG.Event_Shield_GuardImpact),
										FGameplayEventTagMulticastDelegate::FDelegate::CreateUObject(this, &UBOTWGA_ShieldGuard::OnGameplayEvent));
	}
	else
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, false, false);
	}
}

void UBOTWGA_ShieldGuard::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	if (AbilitySystemComponent)
	{
		AbilitySystemComponent->RemoveGameplayEventTagContainerDelegate(FGameplayTagContainer(GAMEPLAYTAG.Event_Shield_GuardImpact), GameplayEventTagDelegateHandle);
	}

	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UBOTWGA_ShieldGuard::InputReleased(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo)
{
	if (IsShieldImpactPlaying == false)
	{
		CancelAbility(Handle, ActorInfo, ActivationInfo, true);
	}
}

void UBOTWGA_ShieldGuard::OnGameplayEvent(FGameplayTag _EventTag, const FGameplayEventData* _Payload)
{
	ACharacter* character = Cast<ACharacter>(CurrentActorInfo->AvatarActor.Get());
	if (character)
	{
		ICombatInterface* combatInterface = Cast<ICombatInterface>(character->FindComponentByInterface<UCombatInterface>());
		if (combatInterface)
		{
			MotionWarpingComponent = character->FindComponentByClass<UMotionWarpingComponent>();
			if (MotionWarpingComponent)
			{
				float knockBackPower = combatInterface->GetKnockBackPower();

				FVector knockBackLocation = (character->GetActorForwardVector() * -1.f * knockBackPower) + character->GetActorLocation();
				MotionWarpingComponent->AddOrUpdateWarpTargetFromLocation(FName("KnockBack"), knockBackLocation);
			}

			if (combatInterface->GetShieldGuardImpactAnimMontage())
			{
				PlayMontageAndWaitForEvent = UAT_PlayMontageAndWaitForEvent::PlayMontageAndWaitForEvent(this, combatInterface->GetShieldGuardImpactAnimMontage());
				if (PlayMontageAndWaitForEvent)
				{
					PlayMontageAndWaitForEvent->OnCompleted.AddDynamic(this, &UBOTWGA_ShieldGuard::OnMontageEnd);
					PlayMontageAndWaitForEvent->OnInterrupted.AddDynamic(this, &UBOTWGA_ShieldGuard::OnMontageEnd);

					PlayMontageAndWaitForEvent->ReadyForActivation();
				}

				AbilitySystemComponent->AddLooseGameplayTag(GAMEPLAYTAG.Character_State_KnockBack);
			}
		}
	}

	IsShieldImpactPlaying = true;
}

void UBOTWGA_ShieldGuard::OnMontageEnd(FGameplayTag _EventTag, FGameplayEventData _EventData)
{
	IsShieldImpactPlaying = false;

	if (MotionWarpingComponent)
	{
		MotionWarpingComponent->RemoveWarpTarget(FName("KnockBack"));
	}

	AbilitySystemComponent->RemoveLooseGameplayTag(GAMEPLAYTAG.Character_State_KnockBack);

	FGameplayAbilitySpec* spec = GetCurrentAbilitySpec();
	if (spec && spec->InputPressed == false)
	{
		ExternalEndAbility();
	}
}