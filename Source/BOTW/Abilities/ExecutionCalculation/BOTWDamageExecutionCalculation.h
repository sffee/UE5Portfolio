#pragma once

#include "Library/GASLibrary.h"
#include "Abilities/AttributeSets/CharacterAttributeSetBase.h"

#include "CoreMinimal.h"
#include "GameplayEffectExecutionCalculation.h"
#include "BOTWDamageExecutionCalculation.generated.h"

UCLASS()
class BOTW_API UBOTWDamageExecutionCalculation : public UGameplayEffectExecutionCalculation
{
	GENERATED_BODY()
	
public:
	UBOTWDamageExecutionCalculation();

public:
	virtual void Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams, OUT FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const override;

private:
	bool IsGuardSuccess(const AActor* _SourceActor, const AActor* _TargetActor, const FGameplayTagContainer* _TargetTags) const;
};
