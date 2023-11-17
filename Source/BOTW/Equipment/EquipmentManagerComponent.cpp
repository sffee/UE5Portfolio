#include "EquipmentManagerComponent.h"

#include "AbilitySystemGlobals.h"
#include "EquipmentDefinition.h"
#include "EquipmentInstance.h"
#include "Weapon/WeaponInstance.h"

FEquipmentList::FEquipmentList()
	: OwnerComponent(nullptr)
{

}

FEquipmentList::FEquipmentList(UActorComponent* _OwnerComponent)
	: OwnerComponent(_OwnerComponent)
{

}

//UEquipmentInstance* FEquipmentList::AddEntry(TSubclassOf<UEquipmentDefinition> _EquipmentDefinition)
UEquipmentInstance* FEquipmentList::AddEntry(TSubclassOf<UEquipmentDefinition> _EquipmentDefinition, UActorComponent* _OwnerComponent)
{
	OwnerComponent = _OwnerComponent;
	if (_EquipmentDefinition == nullptr || OwnerComponent == nullptr)
	{
		return nullptr;
	}

	UEquipmentInstance* result = nullptr;

	const UEquipmentDefinition* equipmentCDO = GetDefault<UEquipmentDefinition>(_EquipmentDefinition);

	TSubclassOf<UEquipmentInstance> instanceType = equipmentCDO->InstanceType;
	if (instanceType == nullptr)
	{
		instanceType = UEquipmentInstance::StaticClass();
	}

	FEquipmentEntry& newEntry = Entries.AddDefaulted_GetRef();
	newEntry.EquipmentDefinition = _EquipmentDefinition;
	newEntry.Instance = NewObject<UEquipmentInstance>(OwnerComponent->GetOwner(), instanceType);
	result = newEntry.Instance;

	result->SpawnEquipmentActor(equipmentCDO->ActorToSpawn);

	return result;
}

void FEquipmentList::RemoveEntry(UEquipmentInstance* _Instance)
{
	for (auto iter = Entries.CreateIterator(); iter; ++iter)
	{
		FEquipmentEntry entry = *iter;
		if (entry.Instance == _Instance)
		{
			UAbilitySystemComponent* abilitySystemComponent = GetAbilitySystemComponent();
			if (abilitySystemComponent)
			{
				entry.GrantedHandles.TakeFromAbilitySystem(abilitySystemComponent);
			}

			_Instance->DestroyEquipmentActor();

			iter.RemoveCurrent();
		}
	}
}

void FEquipmentList::GrantAbility(UEquipmentInstance* _Instance)
{
	const UEquipmentDefinition* equipmentCDO = nullptr;
	for (auto iter = Entries.CreateIterator(); iter; ++iter)
	{
		FEquipmentEntry& entry = *iter;
		if (entry.Instance == _Instance)
		{
			equipmentCDO = GetDefault<UEquipmentDefinition>(entry.EquipmentDefinition);
			if (IsValid(equipmentCDO))
			{
				UBOTWAbilitySystemComponent* abilitySystemComponent = GetAbilitySystemComponent();
				if (abilitySystemComponent)
				{
					for (const UBOTWAbilitySet* abilitySet : equipmentCDO->AbilitySetsToGrant)
					{
						abilitySet->GiveToAbilitySystem(abilitySystemComponent, &entry.GrantedHandles, entry.Instance);
					}
				}
			}

			break;
		}
	}
}

void FEquipmentList::RemoveAbility(UEquipmentInstance* _Instance)
{
	const UEquipmentDefinition* equipmentCDO = nullptr;
	for (auto iter = Entries.CreateIterator(); iter; ++iter)
	{
		FEquipmentEntry& entry = *iter;
		if (entry.Instance == _Instance)
		{
			equipmentCDO = GetDefault<UEquipmentDefinition>(entry.EquipmentDefinition);
			if (IsValid(equipmentCDO))
			{
				UBOTWAbilitySystemComponent* abilitySystemComponent = GetAbilitySystemComponent();
				if (abilitySystemComponent)
				{
					entry.GrantedHandles.TakeFromAbilitySystem(abilitySystemComponent);
				}
			}

			break;
		}
	}
}

UBOTWAbilitySystemComponent* FEquipmentList::GetAbilitySystemComponent() const
{
	AActor* owingActor = OwnerComponent ? OwnerComponent->GetOwner() : nullptr;
	return Cast<UBOTWAbilitySystemComponent>(UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(owingActor));
}

// -------------------------------------------------------------------------------------------------------------------------------------------------
// UEquipmentManagerComponent

UEquipmentManagerComponent::UEquipmentManagerComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UEquipmentManagerComponent::BeginPlay()
{
	Super::BeginPlay();

	DefaultWeapon = Cast<UWeaponInstance>(SetEquipItem(DefaultWeaponClass));
	EquipWeapon();

	for (const TSubclassOf<UEquipmentDefinition>& equipmentDefinition : DefaultEquipmentClass)
	{
		SetEquipItem(equipmentDefinition);
	}
}

void UEquipmentManagerComponent::UninitializeComponent()
{
	TArray<UEquipmentInstance*> allEquipmentInstances;

	for (const FEquipmentEntry& entry : EquipmentList.Entries)
	{
		allEquipmentInstances.Add(entry.Instance);
	}

	for (UEquipmentInstance* equipInstance : allEquipmentInstances)
	{
		RemoveEquipItem(equipInstance);
	}

	Super::UninitializeComponent();
}

UEquipmentInstance* UEquipmentManagerComponent::SetEquipItem(TSubclassOf<UEquipmentDefinition> _EquipmentClass)
{
	UEquipmentInstance* equipmentInstance = nullptr;

	if (_EquipmentClass)
	{
		equipmentInstance = EquipmentList.AddEntry(_EquipmentClass, this);

		EEquipmentType equipmentType = equipmentInstance->GetEquipmentType();
		if (EEquipmentType::Head <= equipmentType && equipmentType < EEquipmentType::MAX)
		{
			EquippedArmorMap.FindOrAdd(equipmentInstance->GetEquipmentType()) = equipmentInstance;
		}
		else if (equipmentType == EEquipmentType::Weapon)
		{
			SelectedWeapon = Cast<UWeaponInstance>(equipmentInstance);
		}
		else if (equipmentType == EEquipmentType::Shield)
		{
			SelectedShield = Cast<UWeaponInstance>(equipmentInstance);
		}
	}

	return equipmentInstance;
}

void UEquipmentManagerComponent::RemoveEquipItem(UEquipmentInstance* _ItemInstance)
{
	if (_ItemInstance)
	{
		EquipmentList.RemoveEntry(_ItemInstance);
	}
}

void UEquipmentManagerComponent::EquipWeapon()
{
	UWeaponInstance* prevWeaponInstance = EquippedWeapon;
	EquippedWeapon = SelectedWeapon;
	OnEquippedWeaponChangeDelegate.Broadcast(prevWeaponInstance, EquippedWeapon);

	if (prevWeaponInstance)
	{
		EquipmentList.RemoveAbility(prevWeaponInstance);
	}

	if (EquippedWeapon)
	{
		EquippedWeapon->OnEquipped();
		EquipmentList.GrantAbility(EquippedWeapon);

		if (EquippedWeapon->GetOverlayType() == EOverlayType::OneHandedWeapon && SelectedShield)
		{
			UWeaponInstance* prevShieldInstance = EquippedShield;
			EquippedShield = SelectedShield;
			OnEquippedShieldChangeDelegate.Broadcast(nullptr, EquippedShield);

			if (prevShieldInstance)
			{
				EquipmentList.RemoveAbility(prevShieldInstance);
			}

			EquippedShield->OnEquipped();
			EquipmentList.GrantAbility(EquippedShield);
		}
	}
}

void UEquipmentManagerComponent::UnEquipWeapon()
{
	if (EquippedWeapon && EquippedWeapon != DefaultWeapon)
	{
		EquippedWeapon->OnUnEquipped();
		EquipmentList.RemoveAbility(EquippedWeapon);

		OnEquippedWeaponChangeDelegate.Broadcast(EquippedWeapon, DefaultWeapon);
		EquippedWeapon = DefaultWeapon;
		EquipmentList.GrantAbility(DefaultWeapon);
	}

	if (EquippedShield)
	{
		EquippedShield->OnUnEquipped();
		EquipmentList.RemoveAbility(EquippedShield);

		OnEquippedShieldChangeDelegate.Broadcast(EquippedShield, nullptr);
		EquippedShield = nullptr;
	}
}

UEquipmentInstance* UEquipmentManagerComponent::GetFirstInstanceOfType(TSubclassOf<UEquipmentInstance> _instanceType)
{
	for (FEquipmentEntry& entry : EquipmentList.Entries)
	{
		UEquipmentInstance* equipmentInstance = entry.Instance;
		if (equipmentInstance)
		{
			if (equipmentInstance->IsA(_instanceType))
			{
				return equipmentInstance;
			}
		}
	}

	return nullptr;
}

TArray<UEquipmentInstance*> UEquipmentManagerComponent::GetEquipmentInstancesOfType(TSubclassOf<UEquipmentInstance> _InstanceType) const
{
	TArray<UEquipmentInstance*> equipmentInstances;
	for (const FEquipmentEntry& entry : EquipmentList.Entries)
	{
		UEquipmentInstance* equipmentInstance = entry.Instance;
		if (equipmentInstance)
		{
			if (equipmentInstance->IsA(_InstanceType))
			{
				equipmentInstances.Add(equipmentInstance);
			}
		}
	}

	return equipmentInstances;
}
