#include "BOTWGA_Interaction.h"

#include "Character/Player/PlayerCharacter.h"
#include "Character/Components/InteractionComponent.h"

UBOTWGA_Interaction::UBOTWGA_Interaction()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::NonInstanced;

	UGameplayTagsManager::Get().CallOrRegister_OnDoneAddingNativeTagsDelegate(FSimpleDelegate::CreateLambda([&]
	{
		AbilityInputTag = GAMEPLAYTAG.InputTag_Interaction;
	}));
}

void UBOTWGA_Interaction::OnAvatarSet(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec)
{
	Super::OnAvatarSet(ActorInfo, Spec);

	APlayerCharacter* player = Cast<APlayerCharacter>(ActorInfo->AvatarActor.Get());
	if (player == nullptr)
	{
		return ;
	}

	InteractionComp = Cast<UInteractionComponent>(player->GetComponentByClass(UInteractionComponent::StaticClass()));
}

bool UBOTWGA_Interaction::CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags /*= nullptr*/, const FGameplayTagContainer* TargetTags /*= nullptr*/, OUT FGameplayTagContainer* OptionalRelevantTags /*= nullptr*/) const
{
	if (Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags) == false)
		return false;

	if (InteractionComp.IsValid() == false)
		return false;

	return InteractionComp->CanInteraction();
}

void UBOTWGA_Interaction::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	InteractionComp->DoInteraction();
	
	EndAbility(Handle, ActorInfo, ActivationInfo, false, true);
}
