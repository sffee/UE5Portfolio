#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "EquipmentActor.generated.h"

UCLASS()
class BOTW_API AEquipmentActor : public AActor
{
	GENERATED_BODY()
	
private:
	UPROPERTY(EditAnywhere)
	USkeletalMeshComponent* SkelMesh;

	UPROPERTY(EditAnywhere)
	UStaticMeshComponent* StaticMesh;

public:	
	AEquipmentActor();

public:
	UMeshComponent* GetMesh() const;
};
