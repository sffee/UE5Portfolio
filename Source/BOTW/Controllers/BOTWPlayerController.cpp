#include "BOTWPlayerController.h"

#include "Library/GASLibrary.h"

#include "Abilities/BOTWAbilitySystemComponent.h"
#include "Abilities/AttributeSets/CharacterAttributeSetBase.h"
#include "Character/Player/Camera/BOTWPlayerCameraManager.h"
#include "Character/Player/PlayerCharacter.h"
#include "Character/Components/BOTWInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Kismet/GameplayStatics.h"
#include "InputMappingContext.h"
#include "InputActionValue.h"
#include "Library/LogLibrary.h"
#include "Library/WidgetEnumLibrary.h"
#include "Library/WidgetNamespaceLibrary.h"
#include "SheikahRune/RuneAbilityComponent.h"
#include "SheikahRune/RuneAbilityStateActor.h"
#include "SheikahRune/RuneAbilityStateMachine.h"
#include "SheikahRune/RuneAbilityState_Normal.h"
#include "SheikahRune/RuneAbilityState_RemoteBomb.h"
#include "Widget/HUD/Player/PlayerHUDLayoutWidget.h"

#include "System/BOTWGameInstance.h"
#include "System/BOTWWidgetFunctionLibrary.h"
#include "Widget/HUD/SheikahRune/RuneSelectionWidget.h"

ABOTWPlayerController::ABOTWPlayerController(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	Inventory = CreateDefaultSubobject<UInventoryComponent>(TEXT("INVENTORY"));

}

void ABOTWPlayerController::OnPossess(APawn* NewPawn)
{
	Super::OnPossess(NewPawn);
	PossessedPlayer = Cast<APlayerCharacter>(NewPawn);
	AbilitySystemComponent = PossessedPlayer->GetAbilitySystemComponent();

	SetupCamera();
	SetupInputs();
	SetupWidget();
}

void ABOTWPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	BindActions();
}

void ABOTWPlayerController::PostProcessInput(const float DeltaTime, const bool bGamePaused)
{
	Super::PostProcessInput(DeltaTime, bGamePaused);

	PossessedPlayer = PossessedPlayer == nullptr ? Cast<APlayerCharacter>(GetPawn()) : PossessedPlayer;
	if (PossessedPlayer == nullptr)
	{
		return;
	}

	UBOTWAbilitySystemComponent* abilitySystemComponent = PossessedPlayer->GetAbilitySystemComponent();
	if (abilitySystemComponent)
	{
		abilitySystemComponent->ProcessAbilityInput(DeltaTime);
	}
}

void ABOTWPlayerController::BindActions()
{
	UBOTWInputComponent* enhancedInputComponent = Cast<UBOTWInputComponent>(InputComponent);
	
	if (enhancedInputComponent)
	{
		enhancedInputComponent->ClearActionEventBindings();
		enhancedInputComponent->ClearActionValueBindings();
		enhancedInputComponent->ClearDebugKeyBindings();

		enhancedInputComponent->BindAbilityActions(InputConfig, this, &ThisClass::AbilityInputTagPressed, &ThisClass::AbilityInputTagReleased, BindHandles);
		
		enhancedInputComponent->BindNativeAction(InputConfig, GAMEPLAYTAG.InputTag_MoveForward, ETriggerEvent::Triggered, this, &ThisClass::MoveForward);
		enhancedInputComponent->BindNativeAction(InputConfig, GAMEPLAYTAG.InputTag_MoveSide, ETriggerEvent::Triggered, this, &ThisClass::MoveSide);
		enhancedInputComponent->BindNativeAction(InputConfig, GAMEPLAYTAG.InputTag_LookUp, ETriggerEvent::Triggered, this, &ThisClass::LookUp);
		enhancedInputComponent->BindNativeAction(InputConfig, GAMEPLAYTAG.InputTag_LookAround, ETriggerEvent::Triggered, this, &ThisClass::LookAround);

		enhancedInputComponent->BindNativeAction(InputConfig, GAMEPLAYTAG.InputTag_TestKey, ETriggerEvent::Triggered, this, &ThisClass::TestKey);
	}
}

void ABOTWPlayerController::SetupInputs()
{
	if (PossessedPlayer)
	{
		if (UEnhancedInputLocalPlayerSubsystem* subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
		{
			subsystem->AddMappingContext(InputMappingContext, 1);
		}
	}
}

void ABOTWPlayerController::SetupWidget()
{
	for (const FName& WidgetName : BaseWidgetNames)
	{
		UBOTWWidgetFunctionLibrary::PushWidget(GetWorld(), EWidgetLayoutType::Main, WidgetName, EWidgetAnchorType::Fill);
	}
}

void ABOTWPlayerController::SetupCamera()
{
	ABOTWPlayerCameraManager* cameraManager = Cast<ABOTWPlayerCameraManager>(PlayerCameraManager);
	if (PossessedPlayer && cameraManager)
	{
		cameraManager->OnPossess(PossessedPlayer);
	}
}

void ABOTWPlayerController::AbilityInputTagPressed(FGameplayTag _InputTag)
{
	if (PossessedPlayer)
	{
		UBOTWAbilitySystemComponent* abilitySystemComponent = PossessedPlayer->GetAbilitySystemComponent();
		if (abilitySystemComponent)
		{
			abilitySystemComponent->AbilityInputTagPressed(_InputTag);
		}
	}
}

void ABOTWPlayerController::AbilityInputTagReleased(FGameplayTag _InputTag)
{
	if (PossessedPlayer)
	{
		UBOTWAbilitySystemComponent* abilitySystemComponent = PossessedPlayer->GetAbilitySystemComponent();
		if (abilitySystemComponent)
		{
			abilitySystemComponent->AbilityInputTagReleased(_InputTag);
		}
	}
}

void ABOTWPlayerController::MoveForward(const FInputActionValue& _Value)
{	
	if (PossessedPlayer)
	{
		PossessedPlayer->MoveForward(_Value.GetMagnitude());
	}
}

void ABOTWPlayerController::MoveSide(const FInputActionValue& _Value)
{
	if (PossessedPlayer)
	{
		PossessedPlayer->MoveSide(_Value.GetMagnitude());
	}
}

void ABOTWPlayerController::LookUp(const FInputActionValue& _Value)
{
	if (PossessedPlayer)
	{
		PossessedPlayer->LookUp(_Value.GetMagnitude());
	}
}

void ABOTWPlayerController::LookAround(const FInputActionValue& _Value)
{
	if (PossessedPlayer)
	{
		PossessedPlayer->LookAround(_Value.GetMagnitude());
	}
}

void ABOTWPlayerController::TestKey(const FInputActionValue& _Value)
{
	if (PossessedPlayer)
	{
		UBOTWAbilitySystemComponent* abilitySystemComponent = PossessedPlayer->GetAbilitySystemComponent();
		if (abilitySystemComponent)
		{
			UCharacterAttributeSetBase* attribute = const_cast<UCharacterAttributeSetBase*>(abilitySystemComponent->GetSet<UCharacterAttributeSetBase>());
			if (attribute)
			{
				attribute->SetHealth(attribute->GetHealth() - 1.f);
			}
		}
	}
}