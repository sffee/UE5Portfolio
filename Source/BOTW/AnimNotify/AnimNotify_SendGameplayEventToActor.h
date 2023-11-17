#pragma once

#include "GameplayTagContainer.h"

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "AnimNotify_SendGameplayEventToActor.generated.h"

UCLASS(meta = (DisplayName = "Send GameplayEvent To Actor"))
class BOTW_API UAnimNotify_SendGameplayEventToActor : public UAnimNotify
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, Category = "Ability", meta = (Categories = "Event"))
	FGameplayTag EventTag;

public:
	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation) override;

	virtual FString GetNotifyName_Implementation() const override;
};
