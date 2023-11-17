#include "EquipmentInstance.h"

#include "EquipmentActor.h"
#include "Character/CharacterBase.h"
#include "EquipmentDefinition.h"

UEquipmentInstance::UEquipmentInstance(const FObjectInitializer& ObjectInitializer /*= FObjectInitializer::Get()*/)
	: Super(ObjectInitializer)
{
}

void UEquipmentInstance::SpawnEquipmentActor(const FEquipmentActorToSpawn& _ActorToSpawn)
{
	if (IsValid(_ActorToSpawn.ActorToSpawn) == false)
	{
		return;
	}

	ACharacterBase* owingCharacter = Cast<ACharacterBase>(GetOuter());
	if (owingCharacter)
	{
		USceneComponent* attachTarget = owingCharacter->GetMesh();

		SpawnedActor = owingCharacter->GetWorld()->SpawnActorDeferred<AEquipmentActor>(_ActorToSpawn.ActorToSpawn, FTransform::Identity, owingCharacter);
		if (SpawnedActor)
		{
			SpawnedActor->FinishSpawning(FTransform::Identity, true);
			SpawnedActor->SetActorRelativeTransform(_ActorToSpawn.AttachTransform);
			SpawnedActor->AttachToComponent(attachTarget, FAttachmentTransformRules::KeepRelativeTransform, _ActorToSpawn.AttachSocket);
		}
	}
}

void UEquipmentInstance::DestroyEquipmentActor()
{
	if (SpawnedActor)
	{
		SpawnedActor->Destroy();
	}
}

void UEquipmentInstance::OnEquipped()
{

}

void UEquipmentInstance::OnUnEquipped()
{

}

void UEquipmentInstance::SetInstigator(UObject* _Instigator)
{
	Instigator = _Instigator;
}
