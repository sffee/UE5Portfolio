#pragma once

#include "Library/GASLibrary.h"

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "BOTWStatusComponent.generated.h"

DECLARE_MULTICAST_DELEGATE_ThreeParams(FOnAttributeValueChanged, float, float, AActor*);

class ACharacterBase;
class UCharacterAttributeSetBase;

UCLASS(Blueprintable, ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class BOTW_API UBOTWStatusComponent : public UActorComponent
{
	GENERATED_BODY()

private:
	UPROPERTY()
	UBOTWAbilitySystemComponent* AbilitySystemComponent;

	UPROPERTY()
	ACharacterBase* OwnerCharacter;

	UPROPERTY()
	const UCharacterAttributeSetBase* CharacterAttributeSet;
	
private:
	FOnAttributeValueChanged OnHealthAttributeChanged;
	FOnAttributeValueChanged OnMaxHealthAttributeChanged;
	FOnAttributeValueChanged OnStaminaAttributeChanged;
	FOnAttributeValueChanged OnMaxStaminaAttributeChanged;

	FSimpleMulticastDelegate OnHealthEmpty;
	FSimpleMulticastDelegate OnStaminaEmpty;
	
public:	
	UBOTWStatusComponent();

protected:
	virtual void BeginPlay() override;

private:
	void InitAbilitySystem();

private:
	AActor* GetInstigator(const FOnAttributeChangeData& _ChangeData);

	void OnChangeHealth(const FOnAttributeChangeData& _ChangeData);
	void OnChangeMaxHealth(const FOnAttributeChangeData& _ChangeData);

	void OnChangeStamina(const FOnAttributeChangeData& _ChangeData);
	void OnChangeMaxStamina(const FOnAttributeChangeData& _ChangeData);

public:
	FORCEINLINE FOnAttributeValueChanged& GetOnHealthAttributeChanged() { return OnHealthAttributeChanged; }
	FORCEINLINE FOnAttributeValueChanged& GetOnMaxHealthAttributeChanged() { return OnMaxHealthAttributeChanged; }
	FORCEINLINE FOnAttributeValueChanged& GetOnStaminaAttributeChanged() { return OnStaminaAttributeChanged; }
	FORCEINLINE FOnAttributeValueChanged& GetOnMaxStaminaAttributeChanged() { return OnMaxStaminaAttributeChanged; }

	FORCEINLINE FSimpleMulticastDelegate& GetOnHealthEmpty() { return OnHealthEmpty; }
	FORCEINLINE FSimpleMulticastDelegate& GetOnStaminaEmpty() { return OnStaminaEmpty; }
};
