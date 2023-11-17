#include "CarryableObject.h"

#include "Components/StaticMeshComponent.h"

ACarryableObject::ACarryableObject()
{
	PrimaryActorTick.bCanEverTick = false;

	StaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMeshComponent"));
	StaticMeshComponent->SetCollisionProfileName(TEXT("BlockAll"));
}

void ACarryableObject::BeginPlay()
{
	Super::BeginPlay();
	
}

FTransform ACarryableObject::GetLeftHandSocketTransform()
{
	return StaticMeshComponent->GetSocketTransform(FName("LeftHandSocket"));
}

FTransform ACarryableObject::GetRightHandSocketTransform()
{
	return StaticMeshComponent->GetSocketTransform(FName("RightHandSocket"));
}

FVector ACarryableObject::GetLocationOffset()
{
	return LocationOffset;
}
