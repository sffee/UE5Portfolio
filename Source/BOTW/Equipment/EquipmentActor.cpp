#include "EquipmentActor.h"

AEquipmentActor::AEquipmentActor()
{
	PrimaryActorTick.bCanEverTick = false;
	
	SkelMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("SkeletalMeshComponent"));
	SkelMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	StaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMeshComponent"));
	StaticMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

FORCEINLINE UMeshComponent* AEquipmentActor::GetMesh() const
{
	UMeshComponent* meshComponent = nullptr;
	if (SkelMesh)
	{
		meshComponent = SkelMesh;
	}
	else if (StaticMesh)
	{
		meshComponent = StaticMesh;
	}

	return meshComponent;
}
