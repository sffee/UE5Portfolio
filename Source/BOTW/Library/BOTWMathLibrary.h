#pragma once

#include "LocomotionEnumLibrary.h"

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "BOTWMathLibrary.generated.h"

UCLASS()
class BOTW_API UBOTWMathLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
public:
	static bool AngleInRange(float _Angle, float _MinAngle, float _MaxAngle, float _Buffer, bool _IncreaseBuffer);

	static EMovementDirection CalculateQuadrant(EMovementDirection _Current, float _FRThreshold, float _FLThreshold, float _BRThreshold, float _BLThreshold, float _Buffer, float _Angle);
};
