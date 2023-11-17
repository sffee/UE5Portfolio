#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"

class UGameplayTagsManager;

struct FBOTWGameplayTags
{
private:
	static FBOTWGameplayTags GameplayTags;

public:
	// Event
	FGameplayTag Event;

	FGameplayTag Event_Weapon;
	FGameplayTag Event_Weapon_Attack;
	FGameplayTag Event_Weapon_Attack_Attackable;
	FGameplayTag Event_Weapon_Attack_CollisionEnable;
	FGameplayTag Event_Weapon_Attack_CollisionDisable;
	FGameplayTag Event_Weapon_Attack_AttackEnd;
	FGameplayTag Event_Weapon_Attack_RushAttack;
	FGameplayTag Event_Weapon_Attack_RushAttackable;
	FGameplayTag Event_Weapon_Attack_RushEnd;

	FGameplayTag Event_Shield_GuardImpact;

	FGameplayTag Event_Character_Carry;
	FGameplayTag Event_Character_Carry_CarryObject;
	FGameplayTag Event_Character_Carry_DropObject;
	FGameplayTag Event_Character_Carry_CarryEnd;

	// Ability
	FGameplayTag Ability;
	FGameplayTag Ability_BlockAbilityInput;
	FGameplayTag Ability_BlockMoveInput;

	// GameplayEffect
	FGameplayTag GameplayEffect;
	FGameplayTag GameplayEffect_SetByCaller;
	FGameplayTag GameplayEffect_SetByCaller_StaminaCost;
	FGameplayTag GameplayEffect_SetByCaller_Damage;

	// InputTag
	FGameplayTag InputTag_MoveForward;
	FGameplayTag InputTag_MoveSide;
	FGameplayTag InputTag_LookUp;
	FGameplayTag InputTag_LookAround;
	FGameplayTag InputTag_Sprint;
	FGameplayTag InputTag_Crouch;
	FGameplayTag InputTag_Jump;
	FGameplayTag InputTag_Attack;
	FGameplayTag InputTag_LockOn;
	FGameplayTag InputTag_Interaction;
	FGameplayTag InputTag_KnockBack;
	FGameplayTag InputTag_ShieldGuard;
	FGameplayTag InputTag_Rush;
	FGameplayTag InputTag_Parrying;
	FGameplayTag InputTag_Climbing;
	FGameplayTag InputTag_TestKey;
	FGameplayTag InputTag_ShowRune;
	FGameplayTag InputTag_ConfirmRune;
	FGameplayTag InputTag_CancelRune;
	FGameplayTag InputTag_TakeRune;
	FGameplayTag InputTag_Carry;

	// Character
	FGameplayTag Character_Attribute_StaminaFull;

	FGameplayTag Character_State;
	FGameplayTag Character_State_Crouch;
	FGameplayTag Character_State_Sprint;
	FGameplayTag Character_State_Jump;
	FGameplayTag Character_State_InAir;
	FGameplayTag Character_State_MeleeAttack;
	FGameplayTag Character_State_KnockBack;
	FGameplayTag Character_State_ShieldGuard;
	FGameplayTag Character_State_Parrying;
	FGameplayTag Character_State_Climbing;
	FGameplayTag Character_State_Carrying;

	FGameplayTag Character_Special_LockOn;
	FGameplayTag Character_Special_Rush;
	FGameplayTag Character_Special_RushAttackEnable;
	FGameplayTag Character_Special_Parrying;

	// EnemyState
	FGameplayTag EnemyState;
	FGameplayTag EnemyState_Combat;
	FGameplayTag EnemyState_Combat_Offense;
	FGameplayTag EnemyState_Combat_Whirlwind;
	FGameplayTag EnemyState_Combat_JumpAttack;
	FGameplayTag EnemyState_Combat_Hit;
	FGameplayTag EnemyState_Combat_ShieldGuard;
	
	FGameplayTag EnemyState_Combat_Cooldown;
	FGameplayTag EnemyState_Combat_GotHit;
	FGameplayTag EnemyState_Combat_Knockdown;

	FGameplayTag EnemyState_NormalAttack;
	FGameplayTag EnemyState_PriorityAttack;
	
	// EnemyAbility
	FGameplayTag InputTag_EnemyAbility;
	FGameplayTag InputTag_EnemyAbility_Combat;
	FGameplayTag InputTag_EnemyAbility_Combat_Offense;
	FGameplayTag InputTag_EnemyAbility_Combat_Whirlwind;
	FGameplayTag InputTag_EnemyAbility_Combat_JumpAttack;
	FGameplayTag InputTag_EnemyAbility_Combat_ShieldGuard;
	
	// EnemyEvent
	FGameplayTag EnemyEvent;
	FGameplayTag EnemyEvent_Combat;
	FGameplayTag EnemyEvent_Combat_Hit;
	FGameplayTag EnemyEvent_Combat_Knockdown;

public:
	static void InitializeTags();

private:
	void InitTags();

public:
	static const FBOTWGameplayTags& Get() { return GameplayTags; }
};