#pragma once

#include "Abilities/BOTWAbilitySet.h"
#include "Library/EquipmentEnumLibrary.h"

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "EquipmentManagerComponent.generated.h"

class UBOTWAbilitySystemComponent;
class UEquipmentDefinition;
class UEquipmentInstance;
class UWeaponInstance;

DECLARE_MULTICAST_DELEGATE_TwoParams(FOnEquipmentChangeDelegate, UEquipmentInstance*, UEquipmentInstance*);

USTRUCT(BlueprintType)
struct FEquipmentEntry
{
	GENERATED_BODY()

private:
	friend FEquipmentList;
	friend UEquipmentManagerComponent;

private:
	UPROPERTY()
	TSubclassOf<UEquipmentDefinition> EquipmentDefinition;

	UPROPERTY()
	UEquipmentInstance* Instance;

	UPROPERTY()
	FBOTWAbilitySet_GrantedHandles GrantedHandles;
};

USTRUCT(BlueprintType)
struct FEquipmentList
{
	GENERATED_BODY()

private:
	friend UEquipmentManagerComponent;

private:
	UPROPERTY()
	UActorComponent* OwnerComponent;

	UPROPERTY()
	TArray<FEquipmentEntry> Entries;
	
public:
	FEquipmentList();
	FEquipmentList(UActorComponent* _OwnerComponent);

public:
	//UEquipmentInstance* AddEntry(TSubclassOf<UEquipmentDefinition> _EquipmentDefinition);
	UEquipmentInstance* AddEntry(TSubclassOf<UEquipmentDefinition> _EquipmentDefinition, UActorComponent* _OwnerComponent);
	void RemoveEntry(UEquipmentInstance* _Instance);

	void GrantAbility(UEquipmentInstance* _Instance);
	void RemoveAbility(UEquipmentInstance* _Instance);

private:
	UBOTWAbilitySystemComponent* GetAbilitySystemComponent() const;
};

// -------------------------------------------------------------------------------------------------------------------------------------------------
// UEquipmentManagerComponent

UCLASS(BlueprintType, Const)
class BOTW_API UEquipmentManagerComponent : public UActorComponent
{
	GENERATED_BODY()

private:
	UPROPERTY()
	FEquipmentList EquipmentList;

	FOnEquipmentChangeDelegate OnEquippedWeaponChangeDelegate;
	FOnEquipmentChangeDelegate OnEquippedShieldChangeDelegate;
	FOnEquipmentChangeDelegate OnEquippedArmorChangeDelegate;

private:
	UPROPERTY()
	UWeaponInstance* DefaultWeapon;

	UPROPERTY()
	UWeaponInstance* SelectedWeapon;

	UPROPERTY()
	UWeaponInstance* SelectedShield;
	
	UPROPERTY()
	UWeaponInstance* EquippedWeapon;

	UPROPERTY()
	UWeaponInstance* EquippedShield;

	UPROPERTY()
	TMap<EEquipmentType, UEquipmentInstance*> EquippedArmorMap;

	UPROPERTY(EditAnywhere, Category = "Default")
	TSubclassOf<UEquipmentDefinition> DefaultWeaponClass;	// ¸Ç¼Õ

	UPROPERTY(EditAnywhere, Category = "Default")
	TArray<TSubclassOf<UEquipmentDefinition>> DefaultEquipmentClass;

public:	
	UEquipmentManagerComponent();

	virtual void BeginPlay() override;
	virtual void UninitializeComponent() override;

public:
	UEquipmentInstance* SetEquipItem(TSubclassOf<UEquipmentDefinition> _EquipmentClass);
	void RemoveEquipItem(UEquipmentInstance* _ItemInstance);
	
	void EquipWeapon();
	void UnEquipWeapon();

public:
	UEquipmentInstance* GetFirstInstanceOfType(TSubclassOf<UEquipmentInstance> _instanceType);
	TArray<UEquipmentInstance*> GetEquipmentInstancesOfType(TSubclassOf<UEquipmentInstance> _InstanceType) const;

	template<typename T>
	T* GetFirstInstanceOfType()
	{
		return Cast<T>(GetFirstInstanceOfType(T::StaticClass()));
	}

public:
	FORCEINLINE FOnEquipmentChangeDelegate& GetOnEquippedWeaponChangeDelegate() { return OnEquippedWeaponChangeDelegate; }
	FORCEINLINE FOnEquipmentChangeDelegate& GetOnEquippedShieldChangeDelegate() { return OnEquippedShieldChangeDelegate; }
	FORCEINLINE FOnEquipmentChangeDelegate& GetOnEquippedArmorChangeDelegate() { return OnEquippedArmorChangeDelegate; }
};
