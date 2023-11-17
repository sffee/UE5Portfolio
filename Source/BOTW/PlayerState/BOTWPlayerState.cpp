#include "BOTWPlayerState.h"

#include "Abilities/BOTWAbilitySystemComponent.h"
#include "Abilities/AttributeSets/CharacterAttributeSetBase.h"

ABOTWPlayerState::ABOTWPlayerState()
{
	AbilitySystemComponent = CreateDefaultSubobject<UBOTWAbilitySystemComponent>(TEXT("AbilitySystemComponent"));

	AttributeSet = CreateDefaultSubobject<UCharacterAttributeSetBase>(TEXT("AttributeSet"));
}
