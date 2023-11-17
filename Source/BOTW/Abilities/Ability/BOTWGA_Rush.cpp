#include "BOTWGA_Rush.h"

#include "Character/Player/PlayerCharacter.h"
#include "Character/Player/Components/PlayerCombatComponent.h"
#include "Character/Player/Camera/BOTWPlayerCameraManager.h"
#include "Abilities/AbilityTask/AT_WaitDelay.h"
#include "Abilities/AbilityTask/AT_CorrectionRushTimeDilation.h"

UBOTWGA_Rush::UBOTWGA_Rush()
	: RushAttackEnableTime(0.f)
	, IsRushAttackStart(false)
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;

	UGameplayTagsManager::Get().CallOrRegister_OnDoneAddingNativeTagsDelegate(FSimpleDelegate::CreateLambda([&]
	{
		AbilityInputTag = GAMEPLAYTAG.InputTag_Rush;
		AbilityTags.AddTag(GAMEPLAYTAG.Character_Special_Rush);
		ActivationOwnedTags.AddTag(GAMEPLAYTAG.Character_Special_Rush);

		FAbilityTriggerData AbilityTriggerData;
		AbilityTriggerData.TriggerTag = GAMEPLAYTAG.InputTag_Rush;
		AbilityTriggers.Add(AbilityTriggerData);
	}));
}

void UBOTWGA_Rush::OnAvatarSet(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec)
{
	Super::OnAvatarSet(ActorInfo, Spec);

	Player = Cast<APlayerCharacter>(ActorInfo->AvatarActor.Get());
	PlayerCombatComponent = Player ? Player->FindComponentByClass<UPlayerCombatComponent>() : nullptr;
	CameraManager = Cast<ABOTWPlayerCameraManager>(ActorInfo->PlayerController->PlayerCameraManager);
	AbilitySystemComponent = GetBOTWAbilitySystemComponent();
}

void UBOTWGA_Rush::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	WaitDelay = UAT_WaitDelay::WaitDelay(this, RushAttackEnableTime, FSimpleDelegate::CreateUObject(this, &UBOTWGA_Rush::RushAttackEnable));
	if (WaitDelay)
	{
		WaitDelay->ReadyForActivation();
	}

	GameplayEventTagDelegateHandle = AbilitySystemComponent->AddGameplayEventTagContainerDelegate(FGameplayTagContainer(GAMEPLAYTAG.Event_Weapon_Attack_RushAttack),
		FGameplayEventTagMulticastDelegate::FDelegate::CreateUObject(this, &UBOTWGA_Rush::OnGameplayEvent));

	CameraManager->SetIsApplyRushBlur(true);

	PlayerCombatComponent->StartTimeDilation(RushTimeDilationCurve);
	TimeDilationEndDelegateHandle = PlayerCombatComponent->GetTimeDilationEndDelegate().AddLambda([&] { ExternalEndAbility(); });

	IsRushAttackStart = false;
}

bool UBOTWGA_Rush::CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags /*= nullptr*/, const FGameplayTagContainer* TargetTags /*= nullptr*/, OUT FGameplayTagContainer* OptionalRelevantTags /*= nullptr*/) const
{
	if (Player == nullptr || PlayerCombatComponent == nullptr || AbilitySystemComponent == nullptr ||
		CameraManager == nullptr || RushTimeDilationCurve == nullptr || RushAttackTimeDilationCurve == nullptr)
	{
		return false;
	}

	return Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags);
}

void UBOTWGA_Rush::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	if (TimeDilationEndDelegateHandle.IsValid())
	{
		PlayerCombatComponent->GetTimeDilationEndDelegate().Remove(TimeDilationEndDelegateHandle);
	}

	if (GameplayEventTagDelegateHandle.IsValid())
	{
		AbilitySystemComponent->RemoveGameplayEventTagContainerDelegate(FGameplayTagContainer(GAMEPLAYTAG.Event_Weapon_Attack_RushAttack), GameplayEventTagDelegateHandle);
	}

	if (WaitDelay)
	{
		WaitDelay->EndTask();
	}

	if (CorrectionRushTimeDilation)
	{
		CorrectionRushTimeDilation->EndTask();
		CorrectionRushTimeDilation = nullptr;
	}

	CameraManager->SetIsApplyRushBlur(false);

	AbilitySystemComponent->RemoveLooseGameplayTag(GAMEPLAYTAG.Character_Special_RushAttackEnable);
	HandleGameplayEvent(GAMEPLAYTAG.Event_Weapon_Attack_RushEnd);

	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UBOTWGA_Rush::OnGameplayEvent(FGameplayTag _EventTag, const FGameplayEventData* _Payload)
{
	if (IsRushAttackStart == false)
	{
		CorrectionRushTimeDilation = UAT_CorrectionRushTimeDilation::CorrectionRushTimeDilation(this, Player);
		if (CorrectionRushTimeDilation)
		{
			CorrectionRushTimeDilation->ReadyForActivation();
		}

		PlayerCombatComponent->StartTimeDilation(RushAttackTimeDilationCurve, true);
	}

	IsRushAttackStart = true;
}

void UBOTWGA_Rush::RushAttackEnable()
{
	AbilitySystemComponent->AddLooseGameplayTag(GAMEPLAYTAG.Character_Special_RushAttackEnable);
}