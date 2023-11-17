#include "CharacterBase.h"

#include "Components/LocomotionComponent.h"

#include "Abilities/Ability/BOTWGameplayAbilityBase.h"
#include "Abilities/AttributeSets/CharacterAttributeSetBase.h"
#include "Equipment/EquipmentManagerComponent.h"
#include "Equipment/EquipmentDefinition.h"

ACharacterBase::ACharacterBase(const FObjectInitializer& ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = true;

	LocomotionComponent = CreateDefaultSubobject<ULocomotionComponent>(TEXT("LocomotionComponent"));
	EquipmentManager = CreateDefaultSubobject<UEquipmentManagerComponent>(TEXT("EquipmentManagerComponent"));

	bUseControllerRotationYaw = false;
}

void ACharacterBase::BeginPlay()
{
	Super::BeginPlay();
	
	GetMesh()->AddTickPrerequisiteActor(this);
}

void ACharacterBase::PostInitializeComponents()
{
	Super::PostInitializeComponents();
}

void ACharacterBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ACharacterBase::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void ACharacterBase::AddAbilities()
{
	if (AbilitySystemComponent == nullptr || AbilitySystemComponent->IsAddAbilityComplete)
	{
		return;
	}

	if (AbilitySet)
	{
		AbilitySet->GiveToAbilitySystem(AbilitySystemComponent, &GrantedAbilityHandles);
	}

	AbilitySystemComponent->IsAddAbilityComplete = true;
}

void ACharacterBase::RemoveAbilities()
{
	if (AbilitySystemComponent == nullptr)
	{
		return;
	}

	GrantedAbilityHandles.TakeFromAbilitySystem(AbilitySystemComponent);
}

void ACharacterBase::OnMovementModeChanged(EMovementMode _PrevMovementMode, uint8 _PreviousCustomMode /*= 0*/)
{
	Super::OnMovementModeChanged(_PrevMovementMode, _PreviousCustomMode);

	if (LocomotionComponent == nullptr)
	{
		return;
	}

	if (GetCharacterMovement()->MovementMode == MOVE_Walking ||
		GetCharacterMovement()->MovementMode == MOVE_NavWalking)
	{
		LocomotionComponent->SetMovementState(EMovementState::Grounded);
	}
	
	else if (GetCharacterMovement()->MovementMode == MOVE_Falling)
	{
		if (AbilitySystemComponent && InAirGameplayEffect && InAirGameplayEffectHandle.IsValid() == false)
		{
			InAirGameplayEffectHandle = AbilitySystemComponent->ApplyGameplayEffectToSelf(InAirGameplayEffect->GetDefaultObject<UGameplayEffect>(), 1.f, AbilitySystemComponent->MakeEffectContext());
		}

		LocomotionComponent->SetMovementState(EMovementState::InAir);
	}

	if (_PrevMovementMode == MOVE_Falling)
	{
		if (AbilitySystemComponent && InAirGameplayEffectHandle.IsValid())
		{
			AbilitySystemComponent->RemoveActiveGameplayEffect(InAirGameplayEffectHandle);
			InAirGameplayEffectHandle.Invalidate();
		}
	}
	
}

void ACharacterBase::OnStartCrouch(float _HalfHeightAdjust, float _ScaledHalfHeightAdjust)
{
	Super::OnStartCrouch(_HalfHeightAdjust, _ScaledHalfHeightAdjust);

	if (LocomotionComponent)
	{
		LocomotionComponent->SetStance(EStance::Crouching);
	}
}

void ACharacterBase::OnEndCrouch(float _HalfHeightAdjust, float _ScaledHalfHeightAdjust)
{
	Super::OnEndCrouch(_HalfHeightAdjust, _ScaledHalfHeightAdjust);

	if (LocomotionComponent)
	{
		LocomotionComponent->SetStance(EStance::Standing);
	}
}

void ACharacterBase::OnJumped_Implementation()
{
	Super::OnJumped_Implementation();

	if (LocomotionComponent)
	{
		LocomotionComponent->OnJumped();
	}

	OnJumpedDelegate.Broadcast();
}

void ACharacterBase::Landed(const FHitResult& Hit)
{
	Super::Landed(Hit);

	if (LocomotionComponent)
	{
		LocomotionComponent->OnLanded(Hit);
	}
}

bool ACharacterBase::CanSprint()
{
	if (LocomotionComponent)
	{
		return LocomotionComponent->CanSprint();
	}

	return false;
}

void ACharacterBase::Sprint(bool _IsActivate)
{
	if (LocomotionComponent)
	{
		LocomotionComponent->Sprint(_IsActivate);
	}
}

void ACharacterBase::LockOn(ACharacterBase* _LockOnTarget)
{
	OnLockOn.Broadcast(_LockOnTarget);
}

void ACharacterBase::LockOff()
{
	OnLockOn.Broadcast(nullptr);
}

FVector ACharacterBase::GetLockOnLocation()
{
	if (GetMesh() == nullptr)
	{
		return FVector::ZeroVector;
	}

	return GetMesh()->GetSocketLocation(FName("LockOnSocket"));
}
