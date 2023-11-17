#include "AT_CorrectionRushTimeDilation.h"

#include "GameFramework/Character.h"
#include "Components/SkeletalMeshComponent.h"

UAT_CorrectionRushTimeDilation::UAT_CorrectionRushTimeDilation(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	bTickingTask = true;
}

UAT_CorrectionRushTimeDilation* UAT_CorrectionRushTimeDilation::CorrectionRushTimeDilation(UGameplayAbility* _OwningAbility, ACharacter* _TargetCharacter)
{
	UAT_CorrectionRushTimeDilation* task = NewAbilityTask<UAT_CorrectionRushTimeDilation>(_OwningAbility);

	task->Character = _TargetCharacter;
	task->SkeletalMeshComponent = _TargetCharacter ? _TargetCharacter->GetMesh() : nullptr;

	return task;
}

void UAT_CorrectionRushTimeDilation::Activate()
{
	if (Character == nullptr || SkeletalMeshComponent == nullptr || GetWorld() == nullptr)
	{
		EndTask();
		return;
	}
}

void UAT_CorrectionRushTimeDilation::TickTask(float DeltaTime)
{
	float globalTimeDilation = GetWorld()->GetWorldSettings()->TimeDilation;

	float characterTimeDilationRate = 1.f / globalTimeDilation;
	SkeletalMeshComponent->GlobalAnimRateScale = characterTimeDilationRate;
}

void UAT_CorrectionRushTimeDilation::OnDestroy(bool AbilityEnded)
{
	Super::OnDestroy(AbilityEnded);

	SkeletalMeshComponent->GlobalAnimRateScale = 1.f;
}