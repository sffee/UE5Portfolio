#pragma once

#include "GameplayTagContainer.h"
#include "DataAsset/BOTWInputConfig.h"

#include "CoreMinimal.h"
#include "EnhancedInputComponent.h"
#include "BOTWInputComponent.generated.h"

class UBOTWInputConfig;

UCLASS()
class BOTW_API UBOTWInputComponent : public UEnhancedInputComponent
{
	GENERATED_BODY()
	
public:
	template<class UserClass, typename FuncType>
	void BindNativeAction(const UBOTWInputConfig* _InputConfig, const FGameplayTag& _InputTag, ETriggerEvent _TriggerEvent, UserClass* _Object, FuncType _Func);

	template<class UserClass, typename PressedFuncType, typename ReleasedFuncType>
	void BindAbilityActions(const UBOTWInputConfig* _InputConfig, UserClass* _Object, PressedFuncType _PressedFunc, ReleasedFuncType _ReleasedFunc, TArray<uint32>& _BindHandles);
};

template<class UserClass, typename FuncType>
void UBOTWInputComponent::BindNativeAction(const UBOTWInputConfig* _InputConfig, const FGameplayTag& _InputTag, ETriggerEvent _TriggerEvent, UserClass* _Object, FuncType _Func)
{
	check(_InputConfig);

	if (const UInputAction* IA = _InputConfig->FindNativeInputActionForTag(_InputTag))
	{
		BindAction(IA, _TriggerEvent, _Object, _Func);
	}
}

template<class UserClass, typename PressedFuncType, typename ReleasedFuncType>
void UBOTWInputComponent::BindAbilityActions(const UBOTWInputConfig* _InputConfig, UserClass* _Object, PressedFuncType _PressedFunc, ReleasedFuncType _ReleasedFunc, TArray<uint32>& _BindHandles)
{
	check(_InputConfig);

	for (const FBOTWInputAction& action : _InputConfig->AbilityInputActions)
	{
		if (action.InputAction && action.InputTag.IsValid())
		{
			if (_PressedFunc)
			{
				_BindHandles.Add(BindAction(action.InputAction, ETriggerEvent::Triggered, _Object, _PressedFunc, action.InputTag).GetHandle());
			}

			if (_ReleasedFunc)
			{
				_BindHandles.Add(BindAction(action.InputAction, ETriggerEvent::Completed, _Object, _ReleasedFunc, action.InputTag).GetHandle());
			}
		}
	}
}