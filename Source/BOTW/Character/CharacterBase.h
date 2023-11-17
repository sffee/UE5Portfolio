#pragma once

#include "Library/GASLibrary.h"
#include "Library/LocomotionEnumLibrary.h"
#include "Library/LocomotionStructLibrary.h"
#include "Abilities/BOTWAbilitySet.h"

#include "Components/BOTWCharacterMovementComponent.h"

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "CharacterBase.generated.h"

class ULocomotionComponent;
class UCharacterAttributeSetBase;
class UEquipmentManagerComponent;
class UEquipmentDefinition;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnJumped);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnLockOn, ACharacterBase*);

UCLASS()
class BOTW_API ACharacterBase : public ACharacter, public IAbilitySystemInterface
{
	GENERATED_BODY()
	
protected:
	UPROPERTY()
	UBOTWAbilitySystemComponent* AbilitySystemComponent;

	UPROPERTY(EditDefaultsOnly)
	ULocomotionComponent* LocomotionComponent;

protected:
	UPROPERTY(EditDefaultsOnly)
	UEquipmentManagerComponent* EquipmentManager;

	UPROPERTY(EditDefaultsOnly, Category = "BOTW|Abilities")
	UBOTWAbilitySet* AbilitySet;

	FBOTWAbilitySet_GrantedHandles GrantedAbilityHandles;

private:
	UPROPERTY(EditDefaultsOnly, Category = "BOTW|Abilities")
	TSubclassOf<UGameplayEffect> InAirGameplayEffect;

	FActiveGameplayEffectHandle InAirGameplayEffectHandle;

public:
	FOnJumped OnJumpedDelegate;

private:
	FOnLockOn OnLockOn;

public:
	ACharacterBase(const FObjectInitializer& ObjectInitializer);

protected:
	virtual void BeginPlay() override;
	virtual void PostInitializeComponents() override;

public:	
	virtual void Tick(float DeltaTime) override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

protected:
	void AddAbilities();
	void RemoveAbilities();

public:
	virtual void OnMovementModeChanged(EMovementMode _PrevMovementMode, uint8 _PreviousCustomMode = 0) override;
	virtual void OnStartCrouch(float _HalfHeightAdjust, float _ScaledHalfHeightAdjust) override;
	virtual void OnEndCrouch(float _HalfHeightAdjust, float _ScaledHalfHeightAdjust) override;
	virtual void OnJumped_Implementation() override;
	virtual void Landed(const FHitResult& Hit) override;

public:
	bool CanSprint();
	void Sprint(bool _IsActivate);

public:
	void LockOn(ACharacterBase* _LockOnTarget);
	void LockOff();

public:
	FVector GetLockOnLocation();

public:
	FORCEINLINE virtual UBOTWAbilitySystemComponent* GetAbilitySystemComponent() const override { return AbilitySystemComponent; }
	FORCEINLINE ULocomotionComponent* GetLocomotionComponent() const { return LocomotionComponent; }

public:
	FORCEINLINE FOnLockOn& GetOnLockOnDelegate() { return OnLockOn; }
};
