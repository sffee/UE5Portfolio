#include "BOTWGameplayAbilityBase.h"

#include "Abilities/AttributeSets/CharacterAttributeSetBase.h"
#include "Character/Components/BOTWStatusComponent.h"

UBOTWGameplayAbilityBase::UBOTWGameplayAbilityBase()
	: AbilityActivationPolicy(EAbilityActivationPolicy::OnInputTriggered)
	, StaminaRequired(0.f)
	, StaminaCost(0.f)
	, IsStaminaCostOnce(false)
{
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalOnly;
}

void UBOTWGameplayAbilityBase::OnGiveAbility(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec)
{
	Super::OnGiveAbility(ActorInfo, Spec);

	TryActivatePassiveAbility(ActorInfo, Spec);
}

void UBOTWGameplayAbilityBase::OnAvatarSet(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec)
{
	Super::OnAvatarSet(ActorInfo, Spec);

	if (AbilityActivationPolicy == EAbilityActivationPolicy::OnSpawn)
	{
		ActorInfo->AbilitySystemComponent->TryActivateAbility(Spec.Handle, false);
	}
}

void UBOTWGameplayAbilityBase::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	if (StaminaCost != 0.f)
	{
		FGameplayEffectSpecHandle specHandle;
		if (IsStaminaCostOnce)
		{
			TSubclassOf<UGameplayEffect> ConsumeStaminaInstance = UBOTWAssetManager::Get().GetSubclass(UGameplayEffectData::Get().ConsumeStaminaEffect_Instance);
			if (ConsumeStaminaInstance)
			{
				specHandle = ActorInfo->AbilitySystemComponent->MakeOutgoingSpec(ConsumeStaminaInstance, 1.f, ActorInfo->AbilitySystemComponent->MakeEffectContext());
			}
		}
		else
		{
			TSubclassOf<UGameplayEffect> ConsumeStaminaContinuous = UBOTWAssetManager::Get().GetSubclass(UGameplayEffectData::Get().ConsumeStaminaEffect_Continuous);
			if (ConsumeStaminaContinuous)
			{
				specHandle = ActorInfo->AbilitySystemComponent->MakeOutgoingSpec(ConsumeStaminaContinuous, 1.f, ActorInfo->AbilitySystemComponent->MakeEffectContext());
			}
		}

		FGameplayEffectSpec* spec = specHandle.Data.Get();
		if (spec == nullptr)
		{
			return;
		}

		spec->SetSetByCallerMagnitude(GAMEPLAYTAG.GameplayEffect_SetByCaller_StaminaCost, -StaminaCost);
		StaminaSpecHandle = ActorInfo->AbilitySystemComponent->ApplyGameplayEffectSpecToSelf(*spec);

		UBOTWStatusComponent* statusComponent = Cast<UBOTWStatusComponent>(ActorInfo->AvatarActor->GetComponentByClass(UBOTWStatusComponent::StaticClass()));
		if (statusComponent)
		{
			statusComponent->GetOnStaminaEmpty().AddUObject(this, &ThisClass::StaminaEmpty);
		}
	}

	if (AbilitySet)
	{
		if (AbilitySet)
		{
			AbilitySet->GiveToAbilitySystem(GetBOTWAbilitySystemComponent(), &GrantedAbilityHandles);
		}
	}
}

bool UBOTWGameplayAbilityBase::CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags /*= nullptr*/, const FGameplayTagContainer* TargetTags /*= nullptr*/, OUT FGameplayTagContainer* OptionalRelevantTags /*= nullptr*/) const
{
	if (AbilityTags.HasTag(FBOTWGameplayTags::Get().Ability_BlockAbilityInput))
	{
		return false;
	}

	if (0.f < StaminaRequired)
	{
		const UCharacterAttributeSetBase* attribute = ActorInfo->AbilitySystemComponent->GetSet<UCharacterAttributeSetBase>();
		if (attribute)
		{
			float stamina = attribute->GetStamina();
			if (stamina < StaminaRequired)
			{
				return false;
			}
		}
	}

	return Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags);
}

void UBOTWGameplayAbilityBase::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	if (StaminaSpecHandle.IsValid())
	{
		ActorInfo->AbilitySystemComponent->RemoveActiveGameplayEffect(StaminaSpecHandle);
		StaminaSpecHandle.Invalidate();
	}

	GrantedAbilityHandles.TakeFromAbilitySystem(GetBOTWAbilitySystemComponent());

	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UBOTWGameplayAbilityBase::TryActivatePassiveAbility(const FGameplayAbilityActorInfo* _ActorInfo, const FGameplayAbilitySpec& _Spec)
{
	if (_ActorInfo && _Spec.IsActive() == false && AbilityActivationPolicy == EAbilityActivationPolicy::OnSpawn)
	{
		UAbilitySystemComponent* abilitySystemComponent = _ActorInfo->AbilitySystemComponent.Get();
		const AActor* avatarActor = _ActorInfo->AvatarActor.Get();

		if (abilitySystemComponent && avatarActor && avatarActor->GetTearOff() == false && avatarActor->GetLifeSpan() <= 0.0f)
		{
			abilitySystemComponent->TryActivateAbility(_Spec.Handle);
		}
	}
}

void UBOTWGameplayAbilityBase::StaminaEmpty()
{
	CancelAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, false);
}

FActiveGameplayEffectHandle UBOTWGameplayAbilityBase::ApplyDamage(AActor* _TargetActor, const UObject* _SourceObject, float _Damage, const FGameplayTagContainer& _DynamicAssetTag /*= FGameplayTagContainer()*/)
{
	return UBOTWGASFunctionLibrary::ApplyDamage(GetAbilitySystemComponentFromActorInfo(), UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(_TargetActor), _Damage, _SourceObject, _DynamicAssetTag);
}

FActiveGameplayEffectHandle UBOTWGameplayAbilityBase::ApplyDamage(UAbilitySystemComponent* _TargetASC, const UObject* _SourceObject, float _Damage, const FGameplayTagContainer& _DynamicAssetTag /*= FGameplayTagContainer()*/)
{
	return UBOTWGASFunctionLibrary::ApplyDamage(GetAbilitySystemComponentFromActorInfo(), _TargetASC, _Damage, _SourceObject, _DynamicAssetTag);
}

int32 UBOTWGameplayAbilityBase::HandleGameplayEvent(FGameplayTag _EventTag)
{
	UAbilitySystemComponent* abilitySystemComponent = GetAbilitySystemComponentFromActorInfo();
	if (abilitySystemComponent == nullptr)
	{
		return 0;
	}

	FGameplayEventData eventData;
	return HandleGameplayEvent(_EventTag, &eventData);
}

int32 UBOTWGameplayAbilityBase::HandleGameplayEvent(FGameplayTag _EventTag, const FGameplayEventData* _Payload)
{
	UAbilitySystemComponent* abilitySystemComponent = GetAbilitySystemComponentFromActorInfo();
	if (abilitySystemComponent == nullptr)
	{
		return 0;
	}

	return abilitySystemComponent->HandleGameplayEvent(_EventTag, _Payload);
}

void UBOTWGameplayAbilityBase::ExternalEndAbility()
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}