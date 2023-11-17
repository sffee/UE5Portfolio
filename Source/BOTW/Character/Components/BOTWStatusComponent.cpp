#include "BOTWStatusComponent.h"

#include "Abilities/BOTWAbilitySystemComponent.h"
#include "Abilities/AttributeSets/CharacterAttributeSetBase.h"
#include "Character/CharacterBase.h"

UBOTWStatusComponent::UBOTWStatusComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UBOTWStatusComponent::BeginPlay()
{
	Super::BeginPlay();

	OwnerCharacter = Cast<ACharacterBase>(GetOwner());
	check(OwnerCharacter);

	AbilitySystemComponent = OwnerCharacter->GetAbilitySystemComponent();
	check(AbilitySystemComponent);

	CharacterAttributeSet = AbilitySystemComponent->GetSet<UCharacterAttributeSetBase>();
	check(CharacterAttributeSet);
	
	GetWorld()->GetTimerManager().SetTimerForNextTick(this, &ThisClass::InitAbilitySystem);
}

void UBOTWStatusComponent::InitAbilitySystem()
{
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(UCharacterAttributeSetBase::GetHealthAttribute()).AddUObject(this, &ThisClass::OnChangeHealth);
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(UCharacterAttributeSetBase::GetMaxHealthAttribute()).AddUObject(this, &ThisClass::OnChangeMaxHealth);
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(UCharacterAttributeSetBase::GetStaminaAttribute()).AddUObject(this, &ThisClass::OnChangeStamina);
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(UCharacterAttributeSetBase::GetMaxStaminaAttribute()).AddUObject(this, &ThisClass::OnChangeMaxStamina);

	OnMaxHealthAttributeChanged.Broadcast(CharacterAttributeSet->GetMaxHealth(), CharacterAttributeSet->GetMaxHealth(), nullptr);
	OnMaxStaminaAttributeChanged.Broadcast(CharacterAttributeSet->GetMaxStamina(), CharacterAttributeSet->GetMaxStamina(), nullptr);

	AbilitySystemComponent->SetNumericAttributeBase(UCharacterAttributeSetBase::GetHealthAttribute(), CharacterAttributeSet->GetMaxHealth());
	AbilitySystemComponent->SetNumericAttributeBase(UCharacterAttributeSetBase::GetStaminaAttribute(), CharacterAttributeSet->GetMaxStamina());
}

AActor* UBOTWStatusComponent::GetInstigator(const FOnAttributeChangeData& _ChangeData)
{
	if (_ChangeData.GEModData != nullptr)
	{
		const FGameplayEffectContextHandle& effectContext = _ChangeData.GEModData->EffectSpec.GetEffectContext();
		return effectContext.GetOriginalInstigator();
	}

	return nullptr;
}

void UBOTWStatusComponent::OnChangeHealth(const FOnAttributeChangeData& _ChangeData)
{
	if (_ChangeData.NewValue <= 0.f)
	{
		OnHealthEmpty.Broadcast();
	}

	OnHealthAttributeChanged.Broadcast(_ChangeData.OldValue, _ChangeData.NewValue, GetInstigator(_ChangeData));
}

void UBOTWStatusComponent::OnChangeMaxHealth(const FOnAttributeChangeData& _ChangeData)
{
	OnMaxHealthAttributeChanged.Broadcast(_ChangeData.OldValue, _ChangeData.NewValue, GetInstigator(_ChangeData));
}

void UBOTWStatusComponent::OnChangeStamina(const FOnAttributeChangeData& _ChangeData)
{
	if (_ChangeData.NewValue <= 0.f)
	{
		OnStaminaEmpty.Broadcast();
	}

	if (_ChangeData.NewValue == CharacterAttributeSet->GetMaxStamina() && AbilitySystemComponent->HasMatchingGameplayTag(GAMEPLAYTAG.Character_Attribute_StaminaFull) == false)
	{
		AbilitySystemComponent->AddLooseGameplayTag(GAMEPLAYTAG.Character_Attribute_StaminaFull, 1);
	}
	else if (AbilitySystemComponent->HasMatchingGameplayTag(GAMEPLAYTAG.Character_Attribute_StaminaFull))
	{
		AbilitySystemComponent->RemoveLooseGameplayTag(GAMEPLAYTAG.Character_Attribute_StaminaFull);
	}

	OnStaminaAttributeChanged.Broadcast(_ChangeData.OldValue, _ChangeData.NewValue, GetInstigator(_ChangeData));
}

void UBOTWStatusComponent::OnChangeMaxStamina(const FOnAttributeChangeData& _ChangeData)
{
	OnMaxStaminaAttributeChanged.Broadcast(_ChangeData.OldValue, _ChangeData.NewValue, GetInstigator(_ChangeData));
}