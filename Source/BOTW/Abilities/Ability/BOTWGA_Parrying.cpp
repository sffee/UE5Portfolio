#include "BOTWGA_Parrying.h"

#include "Abilities/AbilityTask/AT_PlayMontageAndWaitForEvent.h"

UBOTWGA_Parrying::UBOTWGA_Parrying()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;

	UGameplayTagsManager::Get().CallOrRegister_OnDoneAddingNativeTagsDelegate(FSimpleDelegate::CreateLambda([&]
	{
		AbilityInputTag = GAMEPLAYTAG.InputTag_Parrying;
		AbilityTags.AddTag(GAMEPLAYTAG.Character_State_Parrying);
		ActivationOwnedTags.AddTag(GAMEPLAYTAG.Character_State_Parrying);
		ActivationRequiredTags.AddTag(GAMEPLAYTAG.Character_State_ShieldGuard);
	}));
}

void UBOTWGA_Parrying::OnAvatarSet(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec)
{
	Super::OnAvatarSet(ActorInfo, Spec);

	AbilitySystemComponent = GetBOTWAbilitySystemComponent();
}

void UBOTWGA_Parrying::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	PlayMontageAndWaitForEvent = UAT_PlayMontageAndWaitForEvent::PlayMontageAndWaitForEvent(this, AnimMontage, FGameplayTagContainer(GAMEPLAYTAG.Character_Special_Parrying));
	if (PlayMontageAndWaitForEvent)
	{
		PlayMontageAndWaitForEvent->OnCompleted.AddDynamic(this, &UBOTWGA_Parrying::OnGameplayEvent);

		PlayMontageAndWaitForEvent->ReadyForActivation();
	}
}

bool UBOTWGA_Parrying::CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags /*= nullptr*/, const FGameplayTagContainer* TargetTags /*= nullptr*/, OUT FGameplayTagContainer* OptionalRelevantTags /*= nullptr*/) const
{
	if (AbilitySystemComponent == nullptr || AnimMontage == nullptr)
	{
		return false;
	}

	return Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags);
}

void UBOTWGA_Parrying::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UBOTWGA_Parrying::OnGameplayEvent(FGameplayTag _EventTag, FGameplayEventData _EventData)
{
	if (_EventTag == GAMEPLAYTAG.Character_Special_Parrying)
	{
		int a = 0;
	}
	else if (_EventTag == FGameplayTag::EmptyTag)
	{
		ExternalEndAbility();
	}
}