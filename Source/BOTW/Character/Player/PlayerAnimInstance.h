#pragma once

#include "CoreMinimal.h"
#include "../CharacterAnimInstanceBase.h"
#include "PlayerAnimInstance.generated.h"

UCLASS()
class BOTW_API UPlayerAnimInstance : public UCharacterAnimInstanceBase
{
	GENERATED_BODY()
	
protected:
	UPROPERTY(BlueprintReadOnly)
	bool IsDropCarryingObject;

	UPROPERTY(BlueprintReadOnly)
	FTransform CarryingObjectLeftHandSocketTransform;

	UPROPERTY(BlueprintReadOnly)
	FTransform CarryingObjectRightHandSocketTransform;

	UPROPERTY(BlueprintReadOnly)
	FVector CarryingObjectLocationOffset;

public:
	virtual void NativeInitializeAnimation() override;

public:
	FORCEINLINE void SetIsDropCarryingObject(bool _Set) { IsDropCarryingObject = _Set; }
	void SetCarryingObjectSocketTransform(const FTransform& _LeftHand, const FTransform& _RightHand);
	void SetCarryingObjectLocationOffset(const FVector& _Offset);
};
