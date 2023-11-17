#include "GASLibrary.h"

FActiveGameplayEffectHandle UBOTWGASFunctionLibrary::ApplyDamage(UAbilitySystemComponent* _SourceASC, UAbilitySystemComponent* _TargetASC, float _Damage, const UObject* _SourceObject /*= nullptr*/, const FGameplayTagContainer& _DynamicAssetTag /*= FGameplayTagContainer()*/)
{
	FActiveGameplayEffectHandle handle;

	if (_SourceASC && _TargetASC)
	{
		const TSubclassOf<UGameplayEffect> applyDamageEffect = UBOTWAssetManager::GetSubclass(UGameplayEffectData::Get().ApplyDamageEffect_SetByCaller);
		if (applyDamageEffect)
		{
			FGameplayEffectContextHandle contextHandle = _SourceASC->MakeEffectContext();
			contextHandle.AddSourceObject(_SourceObject);

			FGameplayEffectSpecHandle specHandle = _SourceASC->MakeOutgoingSpec(applyDamageEffect, 1.f, contextHandle);
			FGameplayEffectSpec* spec = specHandle.Data.Get();

			if (spec)
			{
				if (_DynamicAssetTag.IsValid())
				{
					spec->AppendDynamicAssetTags(_DynamicAssetTag);
				}

				spec->SetSetByCallerMagnitude(GAMEPLAYTAG.GameplayEffect_SetByCaller_Damage, _Damage);

				handle = _SourceASC->ApplyGameplayEffectSpecToTarget(*spec, _TargetASC);
			}
		}
	}

	return handle;
}