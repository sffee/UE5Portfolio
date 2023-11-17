#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "BOTWAbilitySystemComponent.generated.h"

UCLASS()
class BOTW_API UBOTWAbilitySystemComponent : public UAbilitySystemComponent
{
	GENERATED_BODY()
	
public:
	bool IsAddAbilityComplete;

private:
	TArray<FGameplayAbilitySpecHandle> InputPressedSpecHandles;
	TArray<FGameplayAbilitySpecHandle> InputReleasedSpecHandles;
	TArray<FGameplayAbilitySpecHandle> InputHeldSpecHandles;
	TArray<int32> InputKeyPressedList;

public:
	UBOTWAbilitySystemComponent();

public:
	FGameplayAbilitySpecHandle FindAbilitySpecHandleForClass(TSubclassOf<UGameplayAbility> _AbilityClass, UObject* _OptionalSourceObject);

public:
	void AbilityInputTagPressed(const FGameplayTag& _InputTag);
	void AbilityInputTagReleased(const FGameplayTag& _InputTag);

	void ProcessAbilityInput(float _DeltaTime);

	bool IsKeyPreesed(int32 _InputID);

public:
	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	virtual bool GetShouldTick() const override;

	virtual void AbilityLocalInputPressed(int32 InputID);
	virtual void AbilityLocalInputReleased(int32 InputID);
};
