#include "BOTWGA_Carrying.h"

#include "Character/Player/Components/CarryComponent.h"

UBOTWGA_Carrying::UBOTWGA_Carrying()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;

	UGameplayTagsManager::Get().CallOrRegister_OnDoneAddingNativeTagsDelegate(FSimpleDelegate::CreateLambda([&]
		{
			AbilityInputTag = GAMEPLAYTAG.InputTag_Carry;
			AbilityTags.AddTag(GAMEPLAYTAG.Character_State_Carrying);
			ActivationOwnedTags.AddTag(GAMEPLAYTAG.Character_State_Carrying);
		}));
}

void UBOTWGA_Carrying::OnAvatarSet(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec)
{
	Super::OnAvatarSet(ActorInfo, Spec);

	CarryComponent = ActorInfo->AvatarActor->FindComponentByClass<UCarryComponent>();
}

void UBOTWGA_Carrying::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	CarryComponent->Carry();

	DelegateHandle = GetBOTWAbilitySystemComponent()->AddGameplayEventTagContainerDelegate(FGameplayTagContainer(GAMEPLAYTAG.Event_Character_Carry_CarryEnd),
		FGameplayEventTagMulticastDelegate::FDelegate::CreateUObject(this, &UBOTWGA_Carrying::OnCarryEndTag));
}

bool UBOTWGA_Carrying::CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags /*= nullptr*/, const FGameplayTagContainer* TargetTags /*= nullptr*/, OUT FGameplayTagContainer* OptionalRelevantTags /*= nullptr*/) const
{
	if (IsValid(CarryComponent) == false || CarryComponent->IsCarryable() == false)
	{
		return false;
	}

	return Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags);
}

void UBOTWGA_Carrying::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UBOTWGA_Carrying::InputPressed(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo)
{
	CarryComponent->DropToGround();
}

void UBOTWGA_Carrying::OnCarryEndTag(FGameplayTag _EventTag, const FGameplayEventData* _Payload)
{
	if (_EventTag == GAMEPLAYTAG.Event_Character_Carry_CarryEnd)
	{
		GetBOTWAbilitySystemComponent()->RemoveGameplayEventTagContainerDelegate(FGameplayTagContainer(GAMEPLAYTAG.Event_Character_Carry_CarryEnd), DelegateHandle);
		DelegateHandle.Reset();

		CarryComponent->EndCarry();

		ExternalEndAbility();
	}
}