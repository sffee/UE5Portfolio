#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "CarryableInterface.generated.h"

UINTERFACE(MinimalAPI)
class UCarryableInterface : public UInterface
{
	GENERATED_BODY()
};

class BOTW_API ICarryableInterface
{
	GENERATED_BODY()

public:
	virtual FTransform GetLeftHandSocketTransform() PURE_VIRTUAL(ICarryableInterface::GetLeftHandSocketTransform, return FTransform::Identity;)
	virtual FTransform GetRightHandSocketTransform() PURE_VIRTUAL(ICarryableInterface::GetRightHandSocketTransform, return FTransform::Identity;)
	virtual FVector GetLocationOffset() { return FVector::ZeroVector; }
};
