#include "AnimNotify_SendGameplayEventToActor.h"

#include "GameplayTagsManager.h"

#include "AbilitySystemComponent.h"
#include "Character/CharacterBase.h"

void UAnimNotify_SendGameplayEventToActor::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	ACharacterBase* character = Cast<ACharacterBase>(MeshComp->GetOwner());
	if (character == nullptr)
	{
		return;
	}

	UAbilitySystemComponent* abilitySystemComponent = character->GetAbilitySystemComponent();
	if (abilitySystemComponent == nullptr)
	{
		return;
	}

	FGameplayEventData eventData = FGameplayEventData();
	abilitySystemComponent->HandleGameplayEvent(EventTag, &eventData);
}

FString UAnimNotify_SendGameplayEventToActor::GetNotifyName_Implementation() const
{
	TArray<FName> names;
	UGameplayTagsManager::Get().SplitGameplayTagFName(EventTag, names);

	FString name(TEXT("Event : "));

	if (names.Num())
	{
		name.Append(names.Last().ToString());
	}
	else
	{
		name.Append(TEXT("Empty"));
	}

	return name;
}