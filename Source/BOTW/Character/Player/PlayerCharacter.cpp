#include "PlayerCharacter.h"

#include "Blueprint/UserWidget.h"
#include "PlayerState/BOTWPlayerState.h"
#include "Character/Components/InteractionComponent.h"
#include "Character/Components/LocomotionComponent.h"
#include "Character/Components/BOTWStatusComponent.h"
#include "Character/Components/QuestComponent.h"
#include "Components/PawnNoiseEmitterComponent.h"
#include "Controllers/BOTWPlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "SheikahRune/RuneAbilityComponent.h"
#include "SheikahRune/RuneAbilityStateMachine.h"
#include "Controllers/BOTWPlayerController.h"
#include "Character/Components/BOTWCharacterMovementComponent.h"
#include "Components/ClimbingComponent.h"

APlayerCharacter::APlayerCharacter(const class FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer.SetDefaultSubobjectClass<UBOTWCharacterMovementComponent>(CharacterMovementComponentName))
	, LookUpDownRate(0.45f)
	, LookAroundRate(0.45f)
	, IsBlockMoveInput(false)
{
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	UCharacterMovementComponent* movementComponent = GetCharacterMovement();
	movementComponent->NavAgentProps.bCanCrouch = true;

	InteractionComponent = CreateDefaultSubobject<UInteractionComponent>(TEXT("InteractionComponent"));
	StatusComponent = CreateDefaultSubobject<UBOTWStatusComponent>(TEXT("StatusComponent"));

	ClimbingComponent = CreateDefaultSubobject<UClimbingComponent>(TEXT("ClimbingComponent"));
}

void APlayerCharacter::BeginPlay()
{
	Super::BeginPlay();
}

void APlayerCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	InitAbilitySystem();
}

void APlayerCharacter::UnPossessed()
{
	Super::UnPossessed();
}

void APlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void APlayerCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();
}

void APlayerCharacter::InitAbilitySystem()
{
	ABOTWPlayerState* playerState = GetPlayerState<ABOTWPlayerState>();
	if (playerState == nullptr)
	{
		return;
	}

	AbilitySystemComponent = Cast<UBOTWAbilitySystemComponent>(playerState->GetAbilitySystemComponent());
	check(AbilitySystemComponent);

	AbilitySystemComponent->InitAbilityActorInfo(playerState, this);

	AddAbilities();

	AbilitySystemComponent->RegisterGameplayTagEvent(GAMEPLAYTAG.Ability_BlockMoveInput).AddUObject(this, &APlayerCharacter::OnChangeBlockIMovenputTag);
}

void APlayerCharacter::MoveForward(float _Value)
{
	if (IsBlockMoveInput)
	{
		return;
	}

	if (LocomotionComponent && _Value != 0.f)
	{
		LocomotionComponent->MoveForward(_Value);
	}
}

void APlayerCharacter::MoveSide(float _Value)
{
	if (IsBlockMoveInput)
	{
		return;
	}

	if (LocomotionComponent && _Value != 0.f)
	{
		LocomotionComponent->MoveSide(_Value);
	}
}

void APlayerCharacter::LookUp(float _Value)
{
	if (_Value == 0.f)
	{
		return;
	}

	AddControllerPitchInput(_Value * LookAroundRate);
}

void APlayerCharacter::LookAround(float _Value)
{
	if (_Value == 0.f)
	{
		return;
	}

	AddControllerYawInput(_Value * LookUpDownRate);
}

bool APlayerCharacter::Sprint(bool _IsSprinting)
{
	if (LocomotionComponent->CanSprint())
	{
		LocomotionComponent->Sprint(_IsSprinting);
		return true;
	}

	LocomotionComponent->Sprint(false);

	return false;
}

void APlayerCharacter::OnChangeBlockIMovenputTag(const FGameplayTag _CallbackTag, int32 _NewCount)
{
	if (_CallbackTag != GAMEPLAYTAG.Ability_BlockMoveInput)
	{
		return;
	}

	IsBlockMoveInput = _NewCount ? true : false;
}