#pragma once

#include "GameplayTagContainer.h"

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "AnimNotifyState_SendGameplayEventToActor.generated.h"

class ACharacterBase;

UCLASS(meta = (DisplayName = "Send GameplayEvent To Actor(State)"))
class BOTW_API UAnimNotifyState_SendGameplayEventToActor : public UAnimNotifyState
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, Category = "Ability", meta = (Categories = "Event"))
	FGameplayTag BeginEventTag;

	UPROPERTY(EditAnywhere, Category = "Ability", meta = (Categories = "Event"))
	FGameplayTag EndEventTag;

public:
	virtual void NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference) override;
	virtual void NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;

	virtual FString GetNotifyName_Implementation() const override;

private:
	void SendEventTag(ACharacterBase* _Character, FGameplayTag& _Tag);
};
