#include "BOTWDamageExecutionCalculation.h"

struct BOTWDamageStatics
{
	DECLARE_ATTRIBUTE_CAPTUREDEF(Damage);

	BOTWDamageStatics()
	{
		DEFINE_ATTRIBUTE_CAPTUREDEF(UCharacterAttributeSetBase, Damage, Source, true);
	}
};

static const BOTWDamageStatics& DamageStatics()
{
	static BOTWDamageStatics damageStatics;

	return damageStatics;
}

UBOTWDamageExecutionCalculation::UBOTWDamageExecutionCalculation()
{
	RelevantAttributesToCapture.Add(DamageStatics().DamageDef);
}

bool UBOTWDamageExecutionCalculation::IsGuardSuccess(const AActor* _SourceActor, const AActor* _TargetActor, const FGameplayTagContainer* _TargetTags) const
{
	if (_SourceActor == nullptr || _TargetActor == nullptr || _TargetTags->HasTag(GAMEPLAYTAG.Character_State_ShieldGuard) == false)
	{
		return false;
	}

	FVector sourceLocation = _SourceActor->GetActorLocation();
	FVector targetLocation = _TargetActor->GetActorLocation();

	FVector dirVec = sourceLocation - targetLocation;
	dirVec.Normalize();

	float dot = FVector::DotProduct(_TargetActor->GetActorForwardVector(), dirVec);
	if (0.f < dot)
	{
		// Source와 Target이 서로 마주보고 있는 상태일 경우 가드 성공 판정
		return true;
	}

	return false;
}

void UBOTWDamageExecutionCalculation::Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams, OUT FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const
{
	UAbilitySystemComponent* sourceAbilitySystemComponent = ExecutionParams.GetSourceAbilitySystemComponent();
	UAbilitySystemComponent* targetAbilitySystemComponent = ExecutionParams.GetTargetAbilitySystemComponent();

	AActor* sourceActor = sourceAbilitySystemComponent ? sourceAbilitySystemComponent->GetAvatarActor() : nullptr;
	AActor* targetActor = targetAbilitySystemComponent ? targetAbilitySystemComponent->GetAvatarActor() : nullptr;

	const FGameplayEffectSpec& spec = ExecutionParams.GetOwningSpec();
	FGameplayTagContainer assetTags;
	spec.GetAllAssetTags(assetTags);

	const FGameplayTagContainer* sourceTags = spec.CapturedSourceTags.GetAggregatedTags();
	const FGameplayTagContainer* targetTags = spec.CapturedTargetTags.GetAggregatedTags();

	FAggregatorEvaluateParameters evaluationParameters;
	evaluationParameters.SourceTags = sourceTags;
	evaluationParameters.TargetTags = targetTags;

	if (IsGuardSuccess(sourceActor, targetActor, targetTags))
	{
		FGameplayEventData eventData;
		targetAbilitySystemComponent->HandleGameplayEvent(GAMEPLAYTAG.Event_Shield_GuardImpact, &eventData);
		return;
	}

	float damage = 0.f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().DamageDef, evaluationParameters, damage);

	OutExecutionOutput.AddOutputModifier(FGameplayModifierEvaluatedData(UCharacterAttributeSetBase::GetHealthAttribute(), EGameplayModOp::Additive, -damage));
}
