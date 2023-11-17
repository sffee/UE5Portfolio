#include "BOTWInputConfig.h"

UBOTWInputConfig::UBOTWInputConfig(const FObjectInitializer& ObjectInitializer)
{
}

const UInputAction* UBOTWInputConfig::FindNativeInputActionForTag(const FGameplayTag& _InputTag) const
{
	for (const FBOTWInputAction& action : NativeInputActions)
	{
		if (action.InputAction && (action.InputTag == _InputTag))
		{
			return action.InputAction;
		}
	}

	return nullptr;
}

const UInputAction* UBOTWInputConfig::FindAbilityInputActionForTag(const FGameplayTag& _InputTag) const
{
	for (const FBOTWInputAction& action : AbilityInputActions)
	{
		if (action.InputAction && (action.InputTag == _InputTag))
		{
			return action.InputAction;
		}
	}

	return nullptr;
}
