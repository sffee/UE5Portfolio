#pragma once

#include "Interface/CarryableInterface.h"

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CarryableObject.generated.h"

class UStaticMeshComponent;

UCLASS()
class BOTW_API ACarryableObject : public AActor, public ICarryableInterface
{
	GENERATED_BODY()

private:
	UPROPERTY(EditanyWhere)
	UStaticMeshComponent* StaticMeshComponent;

protected:
	UPROPERTY(EditanyWhere, Category = "Offset")
	FVector LocationOffset;
	
public:	
	ACarryableObject();

protected:
	virtual void BeginPlay() override;

public:
	virtual FTransform GetLeftHandSocketTransform() override;
	virtual FTransform GetRightHandSocketTransform() override;
	virtual FVector GetLocationOffset() override;
};
