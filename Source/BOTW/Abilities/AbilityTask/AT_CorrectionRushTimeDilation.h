#pragma once

#include "CoreMinimal.h"
#include "Abilities/Tasks/AbilityTask.h"
#include "AT_CorrectionRushTimeDilation.generated.h"

class ACharacter;
class USkeletalMeshComponent;

UCLASS()
class BOTW_API UAT_CorrectionRushTimeDilation : public UAbilityTask
{
	GENERATED_UCLASS_BODY()
	
private:
	UPROPERTY()
	ACharacter* Character;

	UPROPERTY()
	USkeletalMeshComponent* SkeletalMeshComponent;

public:
	static UAT_CorrectionRushTimeDilation* CorrectionRushTimeDilation(UGameplayAbility* _OwningAbility, ACharacter* _TargetCharacter);

	virtual void Activate() override;
	virtual void TickTask(float DeltaTime);
	virtual void OnDestroy(bool AbilityEnded) override;
};
