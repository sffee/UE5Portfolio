#include "AnimNotifyState_ApplyGameplayTag.h"

void UAnimNotifyState_ApplyGameplayTag::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	if (GameplayTag.IsValid() == false)
	{
		return;
	}

	UAbilitySystemComponent* abilitySystemComponent = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(MeshComp->GetOwner());
	if (abilitySystemComponent)
	{
		abilitySystemComponent->AddLooseGameplayTag(GameplayTag);
	}
}

void UAnimNotifyState_ApplyGameplayTag::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	if (GameplayTag.IsValid() == false)
	{
		return;
	}

	UAbilitySystemComponent* abilitySystemComponent = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(MeshComp->GetOwner());
	if (abilitySystemComponent)
	{
		abilitySystemComponent->RemoveLooseGameplayTag(GameplayTag);
	}
}

FString UAnimNotifyState_ApplyGameplayTag::GetNotifyName_Implementation() const
{
	return TEXT("GameplayTag : ") + GameplayTag.ToString();
}