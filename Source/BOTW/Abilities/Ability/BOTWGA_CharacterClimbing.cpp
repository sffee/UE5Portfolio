#include "BOTWGA_CharacterClimbing.h"

#include "Character/Player/PlayerCharacter.h"
#include "Character/Player/Components/ClimbingComponent.h"
#include "Character/Components/LocomotionComponent.h"
#include "Character/Components/BOTWCharacterMovementComponent.h"

UBOTWGA_CharacterClimbing::UBOTWGA_CharacterClimbing()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;

	UGameplayTagsManager::Get().CallOrRegister_OnDoneAddingNativeTagsDelegate(FSimpleDelegate::CreateLambda([&]
	{
		AbilityInputTag = GAMEPLAYTAG.InputTag_Climbing;
		AbilityTags.AddTag(GAMEPLAYTAG.Character_State_Climbing);
		ActivationOwnedTags.AddTag(GAMEPLAYTAG.Character_State_Climbing);
	}));
}

void UBOTWGA_CharacterClimbing::OnAvatarSet(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec)
{
	Player = Cast<APlayerCharacter>(ActorInfo->AvatarActor.Get());
	ClimbingComponent = Player ? Player->FindComponentByClass<UClimbingComponent>() : nullptr;
	LocomotionComponent = Player ? Player->FindComponentByClass<ULocomotionComponent>() : nullptr;
}

void UBOTWGA_CharacterClimbing::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Player->GetCharacterMovement()->SetMovementMode(MOVE_Custom, ECustomMovementMode::MOVE_Climb);
	OnEndClimbingDelegateHandle = ClimbingComponent->OnEndClimbing.AddLambda([&] { ExternalEndAbility(); });
	LocomotionComponent->SetMovementState(EMovementState::Climbing);

	ClimbingComponent->StartClimbing();
}

bool UBOTWGA_CharacterClimbing::CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags /*= nullptr*/, const FGameplayTagContainer* TargetTags /*= nullptr*/, OUT FGameplayTagContainer* OptionalRelevantTags /*= nullptr*/) const
{
	if (Player == nullptr || ClimbingComponent == nullptr || LocomotionComponent == nullptr)
	{
		return false;
	}

	if (ClimbingComponent->GetCanStartClimbing() == false)
	{
		return false;
	}

	return Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags);
}

void UBOTWGA_CharacterClimbing::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	Player->GetCharacterMovement()->SetMovementMode(MOVE_Falling);
	ClimbingComponent->OnEndClimbing.Remove(OnEndClimbingDelegateHandle);
	LocomotionComponent->SetMovementState(EMovementState::InAir);

	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}