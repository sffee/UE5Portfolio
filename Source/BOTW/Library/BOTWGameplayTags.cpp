#include "BOTWGameplayTags.h"

FBOTWGameplayTags FBOTWGameplayTags::GameplayTags;

#define TAG(tagName) FGameplayTag::RequestGameplayTag(FName(tagName))

void FBOTWGameplayTags::InitializeTags()
{
	GameplayTags.InitTags();
}

void FBOTWGameplayTags::InitTags()
{
	// Event
	Event = TAG("Event");
	Event_Weapon = TAG("Event.Weapon");
	Event_Weapon_Attack = TAG("Event.Weapon.Attack");
	Event_Weapon_Attack_Attackable = TAG("Event.Weapon.Attack.Attackable");
	Event_Weapon_Attack_CollisionEnable = TAG("Event.Weapon.Attack.CollisionEnable");
	Event_Weapon_Attack_CollisionDisable = TAG("Event.Weapon.Attack.CollisionDisable");
	Event_Weapon_Attack_RushAttack = TAG("Event.Weapon.Attack.RushAttack");
	Event_Weapon_Attack_RushAttackable = TAG("Event.Weapon.Attack.RushAttackable");
	Event_Weapon_Attack_RushEnd = TAG("Event.Weapon.Attack.RushEnd");

	Event_Shield_GuardImpact = TAG("Event.Shield.GuardImpact");

	Event_Character_Carry = TAG("Event.Character.Carry");
	Event_Character_Carry_CarryObject = TAG("Event.Character.Carry.CarryObject");
	Event_Character_Carry_DropObject = TAG("Event.Character.Carry.DropObject");
	Event_Character_Carry_CarryEnd = TAG("Event.Character.Carry.CarryEnd");

	// Ability
	Ability = TAG("Ability");
	Ability_BlockAbilityInput = TAG("Ability.BlockAbilityInput");
	Ability_BlockMoveInput = TAG("Ability.BlockMoveInput");

	// GameplayEffect
	GameplayEffect = TAG("GameplayEffect");
	GameplayEffect_SetByCaller = TAG("GameplayEffect.SetByCaller");
	GameplayEffect_SetByCaller_StaminaCost = TAG("GameplayEffect.SetByCaller.StaminaCost");
	GameplayEffect_SetByCaller_Damage = TAG("GameplayEffect.SetByCaller.Damage");

	// InputTag
	InputTag_MoveForward = TAG("InputTag.MoveForward");
	InputTag_MoveSide = TAG("InputTag.MoveSide");
	InputTag_LookUp = TAG("InputTag.LookUp");
	InputTag_LookAround = TAG("InputTag.LookAround");
	InputTag_Sprint = TAG("InputTag.Sprint");
	InputTag_Crouch = TAG("InputTag.Crouch");
	InputTag_Jump = TAG("InputTag.Jump");
	InputTag_Attack = TAG("InputTag.Attack");
	InputTag_LockOn = TAG("InputTag.LockOn");
	InputTag_Interaction = TAG("InputTag.Interaction");
	InputTag_KnockBack = TAG("InputTag.KnockBack");
	InputTag_ShieldGuard = TAG("InputTag.ShieldGuard");
	InputTag_Rush = TAG("InputTag.Rush");
	InputTag_Parrying = TAG("InputTag.Parrying");
	InputTag_Climbing = TAG("InputTag.Climbing");
	InputTag_TestKey = TAG("InputTag.TestKey");
	InputTag_ShowRune = TAG("InputTag.ShowRune");
	InputTag_ConfirmRune = TAG("InputTag.ConfirmRune");
	InputTag_CancelRune = TAG("InputTag.CancelRune");
	InputTag_TakeRune = TAG("InputTag.TakeRune");
	InputTag_Carry = TAG("InputTag.Carry");

	// Character
	Character_Attribute_StaminaFull = TAG("Character.Attribute.StaminaFull");

	Character_State = TAG("Character.State");
	Character_State_Crouch = TAG("Character.State.Crouch");
	Character_State_Sprint = TAG("Character.State.Sprint");
	Character_State_Jump = TAG("Character.State.Jump");
	Character_State_InAir = TAG("Character.State.InAir");
	Character_State_MeleeAttack = TAG("Character.State.MeleeAttack");
	Character_State_KnockBack = TAG("Character.State.KnockBack");
	Character_State_ShieldGuard = TAG("Character.State.ShieldGuard");
	Character_State_Parrying = TAG("Character.State.Parrying");
	Character_State_Climbing = TAG("Character.State.Climbing");
	Character_State_Carrying = TAG("Character.State.Carrying");

	Character_Special_LockOn = TAG("Character.Special.LockOn");
	Character_Special_Rush = TAG("Character.Special.Rush");
	Character_Special_RushAttackEnable = TAG("Character.Special.RushAttackEnable");
	Character_Special_Parrying = TAG("Character.Special.Parrying");	
}