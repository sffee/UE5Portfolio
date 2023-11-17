#include "AnimNotifyState_SendGameplayEventToActor.h"

#include "GameplayTagsManager.h"

#include "AbilitySystemComponent.h"
#include "Character/CharacterBase.h"

void UAnimNotifyState_SendGameplayEventToActor::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	SendEventTag(Cast<ACharacterBase>(MeshComp->GetOwner()), BeginEventTag);
}

void UAnimNotifyState_SendGameplayEventToActor::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	SendEventTag(Cast<ACharacterBase>(MeshComp->GetOwner()), EndEventTag);
}

FString UAnimNotifyState_SendGameplayEventToActor::GetNotifyName_Implementation() const
{
	TArray<FName> names;
	UGameplayTagsManager::Get().SplitGameplayTagFName(BeginEventTag, names);

	FString name(TEXT("Begin : "));

	if (names.Num())
	{
		name.Append(names.Last().ToString());
	}
	else
	{
		name.Append(TEXT("Empty"));
	}

	names.Empty();
	UGameplayTagsManager::Get().SplitGameplayTagFName(EndEventTag, names);

	name.Append(" / End : ");

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

void UAnimNotifyState_SendGameplayEventToActor::SendEventTag(ACharacterBase* _Character, FGameplayTag& _Tag)
{
	if (_Character == nullptr)
	{
		return;
	}

	UAbilitySystemComponent* abilitySystemComponent = _Character->GetAbilitySystemComponent();
	if (abilitySystemComponent == nullptr)
	{
		return;
	}

	FGameplayEventData eventData = FGameplayEventData();
	abilitySystemComponent->HandleGameplayEvent(_Tag, &eventData);
}
