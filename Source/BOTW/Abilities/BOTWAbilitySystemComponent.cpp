#include "BOTWAbilitySystemComponent.h"

#include "Ability/BOTWGameplayAbilityBase.h"
#include "Library/BOTWGameplayTags.h"

UBOTWAbilitySystemComponent::UBOTWAbilitySystemComponent()
	: IsAddAbilityComplete(false)
{

}

FGameplayAbilitySpecHandle UBOTWAbilitySystemComponent::FindAbilitySpecHandleForClass(TSubclassOf<UGameplayAbility> _AbilityClass, UObject* _OptionalSourceObject)
{
	TSubclassOf<UGameplayAbility> abilityClass = _AbilityClass;
	if (abilityClass->IsInBlueprint())
	{
		abilityClass = abilityClass->GetSuperClass();
	}

	ABILITYLIST_SCOPE_LOCK();
	for (FGameplayAbilitySpec& spec : ActivatableAbilities.Items)
	{
		TSubclassOf<UGameplayAbility> specAbilityClass = spec.Ability->GetClass();
		if (specAbilityClass->IsInBlueprint())
		{
			specAbilityClass = specAbilityClass->GetSuperClass();
		}

		if (specAbilityClass == _AbilityClass)
		{
			if (_OptionalSourceObject == false || (_OptionalSourceObject && spec.SourceObject == _OptionalSourceObject))
			{
				return spec.Handle;
			}
		}
	}

	return FGameplayAbilitySpecHandle();
}

void UBOTWAbilitySystemComponent::AbilityInputTagPressed(const FGameplayTag& _InputTag)
{
	if (_InputTag.IsValid())
	{
		for (const FGameplayAbilitySpec& abilitySpec : ActivatableAbilities.Items)
		{
			if (abilitySpec.Ability && (abilitySpec.DynamicAbilityTags.HasTagExact(_InputTag)))
			{
				InputPressedSpecHandles.AddUnique(abilitySpec.Handle);
				InputHeldSpecHandles.AddUnique(abilitySpec.Handle);
			}
		}
	}
}

void UBOTWAbilitySystemComponent::AbilityInputTagReleased(const FGameplayTag& _InputTag)
{
	if (_InputTag.IsValid())
	{
		for (const FGameplayAbilitySpec& abilitySpec : ActivatableAbilities.Items)
		{
			if (abilitySpec.Ability && (abilitySpec.DynamicAbilityTags.HasTagExact(_InputTag)))
			{
				InputReleasedSpecHandles.AddUnique(abilitySpec.Handle);
				InputHeldSpecHandles.Remove(abilitySpec.Handle);
			}
		}
	}
}

void UBOTWAbilitySystemComponent::ProcessAbilityInput(float _DeltaTime)
{
	static TArray<FGameplayAbilitySpecHandle> abilitiesToActivate;
	abilitiesToActivate.Reset();

	for (const FGameplayAbilitySpecHandle& specHandle : InputHeldSpecHandles)
	{
		if (const FGameplayAbilitySpec* abilitySpec = FindAbilitySpecFromHandle(specHandle))
		{
			if (abilitySpec->Ability && abilitySpec->IsActive() == false)
			{
				const UBOTWGameplayAbilityBase* abilityCDO = CastChecked<UBOTWGameplayAbilityBase>(abilitySpec->Ability);

				if (abilityCDO->GetAbilityActivationPolicy() == EAbilityActivationPolicy::WhileInputActive)
				{
					abilitiesToActivate.AddUnique(abilitySpec->Handle);
				}
			}
		}
	}

	for (const FGameplayAbilitySpecHandle& specHandle : InputPressedSpecHandles)
	{
		if (FGameplayAbilitySpec* abilitySpec = FindAbilitySpecFromHandle(specHandle))
		{
			if (abilitySpec->Ability)
			{
				if (abilitySpec->IsActive())
				{
					AbilitySpecInputPressed(*abilitySpec);
				}
				else if (abilitySpec->InputPressed == false)
				{
					const UBOTWGameplayAbilityBase* abilityCDO = CastChecked<UBOTWGameplayAbilityBase>(abilitySpec->Ability);

					if (abilityCDO->GetAbilityActivationPolicy() == EAbilityActivationPolicy::OnInputTriggered)
					{
						abilitiesToActivate.AddUnique(abilitySpec->Handle);
					}
				}

				abilitySpec->InputPressed = true;
			}
		}
	}

	for (const FGameplayAbilitySpecHandle& abilitySpecHandle : abilitiesToActivate)
	{
		TryActivateAbility(abilitySpecHandle);
	}

	for (const FGameplayAbilitySpecHandle& specHandle : InputReleasedSpecHandles)
	{
		if (FGameplayAbilitySpec* AbilitySpec = FindAbilitySpecFromHandle(specHandle))
		{
			if (AbilitySpec->Ability)
			{
				AbilitySpec->InputPressed = false;

				if (AbilitySpec->IsActive())
				{
					AbilitySpecInputReleased(*AbilitySpec);
				}
			}
		}
	}

	InputPressedSpecHandles.Reset();
	InputReleasedSpecHandles.Reset();
}

bool UBOTWAbilitySystemComponent::IsKeyPreesed(int32 _InputID)
{
	return InputKeyPressedList.Contains(_InputID);
}

void UBOTWAbilitySystemComponent::TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

bool UBOTWAbilitySystemComponent::GetShouldTick() const
{
	return Super::GetShouldTick();
}

void UBOTWAbilitySystemComponent::AbilityLocalInputPressed(int32 InputID)
{
	InputKeyPressedList.AddUnique(InputID);

	if (IsGenericConfirmInputBound(InputID))
	{
		LocalInputConfirm();
		return;
	}

	if (IsGenericCancelInputBound(InputID))
	{
		LocalInputCancel();
		return;
	}

	ABILITYLIST_SCOPE_LOCK();
	for (FGameplayAbilitySpec& Spec : ActivatableAbilities.Items)
	{
		if (Spec.InputID == InputID)
		{
			if (Spec.Ability)
			{
				if (Spec.Ability->AbilityTags.HasTag(FBOTWGameplayTags::Get().Ability_BlockAbilityInput))
				{
					continue;
				}

				InputHeldSpecHandles.AddUnique(Spec.Handle);

				Spec.InputPressed = true;
				if (Spec.IsActive())
				{
					AbilitySpecInputPressed(Spec);

					InvokeReplicatedEvent(EAbilityGenericReplicatedEvent::InputPressed, Spec.Handle, Spec.ActivationInfo.GetActivationPredictionKey());
				}
				else
				{
					TryActivateAbility(Spec.Handle);
				}
			}
		}
	}
}

void UBOTWAbilitySystemComponent::AbilityLocalInputReleased(int32 InputID)
{
	Super::AbilityLocalInputReleased(InputID);

	InputKeyPressedList.Remove(InputID);

	for (FGameplayAbilitySpec& spec : ActivatableAbilities.Items)
	{
		if (spec.InputID == InputID && spec.Ability)
		{
			InputHeldSpecHandles.Remove(spec.Handle);
		}
	}
}
