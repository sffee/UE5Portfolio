#include "BOTWAbilitySet.h"
#include "Abilities/Ability/BOTWGameplayAbilityBase.h"

void FBOTWAbilitySet_GrantedHandles::AddAbilitySpecHandle(const FGameplayAbilitySpecHandle& _Handle)
{
	if (_Handle.IsValid())
	{
		AbilitySpecHandles.Add(_Handle);
	}
}

void FBOTWAbilitySet_GrantedHandles::AddGameplayEffectHandle(const FActiveGameplayEffectHandle& _Handle)
{
	if (_Handle.IsValid())
	{
		GameplayEffectHandles.Add(_Handle);
	}
}

void FBOTWAbilitySet_GrantedHandles::AddAttributeSet(UAttributeSet* _AttributeSet)
{
	GrantedAttributeSets.Add(_AttributeSet);
}

void FBOTWAbilitySet_GrantedHandles::TakeFromAbilitySystem(UAbilitySystemComponent* _AbilitySystemComponent)
{
	if (_AbilitySystemComponent == nullptr)
	{
		return;
	}

	for (const FGameplayAbilitySpecHandle& handle : AbilitySpecHandles)
	{
		if (handle.IsValid())
		{
			_AbilitySystemComponent->ClearAbility(handle);
		}
	}

	for (const FActiveGameplayEffectHandle& handle : GameplayEffectHandles)
	{
		if (handle.IsValid())
		{
			_AbilitySystemComponent->RemoveActiveGameplayEffect(handle);
		}
	}

	for (UAttributeSet* attributeSet : GrantedAttributeSets)
	{
		_AbilitySystemComponent->RemoveSpawnedAttribute(attributeSet);
		//_AbilitySystemComponent->GetSpawnedAttributes_Mutable().Remove(attributeSet);
	}

	AbilitySpecHandles.Reset();
	GameplayEffectHandles.Reset();
	GrantedAttributeSets.Reset();
}

void UBOTWAbilitySet::GiveToAbilitySystem(UBOTWAbilitySystemComponent* _AbilitySystemComponent, FBOTWAbilitySet_GrantedHandles* _OutGrantedHandles, UObject* _SourceObject /*= nullptr*/) const
{
	if (_AbilitySystemComponent == nullptr)
	{
		return;
	}

	bool isPlayer = Cast<APlayerController>(_AbilitySystemComponent->GetAvatarActor()->GetInstigatorController()) ? true : false;

	for (int32 abilityIndex = 0; abilityIndex < GrantedGameplayAbilities.Num(); ++abilityIndex)
	{
		const FBOTWAbilitySet_GameplayAbility& abilityToGrant = GrantedGameplayAbilities[abilityIndex];
		if (IsValid(abilityToGrant.Ability) == false)
		{
			continue;
		}

		if (GrantedGameplayAbilities[abilityIndex].IsOnlyPlayer && isPlayer == false)
		{
			continue;
		}

		UBOTWGameplayAbilityBase* abilityCDO = abilityToGrant.Ability->GetDefaultObject<UBOTWGameplayAbilityBase>();

		FGameplayAbilitySpec abilitySpec(abilityCDO, 1);
		abilitySpec.SourceObject = _SourceObject;
		abilitySpec.DynamicAbilityTags.AddTag(abilityToGrant.InputTag);

		const FGameplayAbilitySpecHandle abilitySpecHandle = _AbilitySystemComponent->GiveAbility(abilitySpec);

		if (_OutGrantedHandles)
		{
			_OutGrantedHandles->AddAbilitySpecHandle(abilitySpecHandle);
		}
	}

	for (int32 effectIndex = 0; effectIndex < GrantedGameplayEffects.Num(); ++effectIndex)
	{
		const TSubclassOf<UGameplayEffect>& effectToGrant = GrantedGameplayEffects[effectIndex];

		if (IsValid(effectToGrant) == false)
		{
			continue;
		}

		const UGameplayEffect* gameplayEffect = effectToGrant->GetDefaultObject<UGameplayEffect>();
		const FActiveGameplayEffectHandle gameplayEffectHandle = _AbilitySystemComponent->ApplyGameplayEffectToSelf(gameplayEffect, 1, _AbilitySystemComponent->MakeEffectContext());

		if (_OutGrantedHandles)
		{
			_OutGrantedHandles->AddGameplayEffectHandle(gameplayEffectHandle);
		}
	}

	for (int32 setIndex = 0; setIndex < GrantedAttributes.Num(); ++setIndex)
	{
		const TSubclassOf<UAttributeSet>& setToGrantAttributeSet = GrantedAttributes[setIndex];

		if (IsValid(setToGrantAttributeSet) == false)
		{
			continue;
		}

		UAttributeSet* newSet = NewObject<UAttributeSet>(_AbilitySystemComponent->GetOwner(), setToGrantAttributeSet);
		_AbilitySystemComponent->AddAttributeSetSubobject(newSet);

		if (_OutGrantedHandles)
		{
			_OutGrantedHandles->AddAttributeSet(newSet);
		}
	}
}
