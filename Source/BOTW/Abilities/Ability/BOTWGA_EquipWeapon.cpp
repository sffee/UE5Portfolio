#include "BOTWGA_EquipWeapon.h"

#include "Equipment/EquipmentManagerComponent.h"

UBOTWGA_EquipWeapon::UBOTWGA_EquipWeapon()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;

	UGameplayTagsManager::Get().CallOrRegister_OnDoneAddingNativeTagsDelegate(FSimpleDelegate::CreateLambda([&]
		{
			AbilityInputTag = GAMEPLAYTAG.InputTag_Attack;
			AbilityTags.AddTag(GAMEPLAYTAG.Character_State_MeleeAttack);
			ActivationOwnedTags.AddTag(GAMEPLAYTAG.Character_State_MeleeAttack);
		}));
}

void UBOTWGA_EquipWeapon::OnAvatarSet(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec)
{
	Super::OnAvatarSet(ActorInfo, Spec);

	EquipmentManagerComponent = ActorInfo->AvatarActor->FindComponentByClass<UEquipmentManagerComponent>();
}

void UBOTWGA_EquipWeapon::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	EquipmentManagerComponent->EquipWeapon();

	ExternalEndAbility();
}

bool UBOTWGA_EquipWeapon::CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags /*= nullptr*/, const FGameplayTagContainer* TargetTags /*= nullptr*/, OUT FGameplayTagContainer* OptionalRelevantTags /*= nullptr*/) const
{
	if (IsValid(EquipmentManagerComponent) == false)
	{
		return false;
	}

	return Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags);
}

void UBOTWGA_EquipWeapon::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}
