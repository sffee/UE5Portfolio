#include "PlayerAnimInstance.h"

void UPlayerAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();
}

void UPlayerAnimInstance::SetCarryingObjectSocketTransform(const FTransform& _LeftHand, const FTransform& _RightHand)
{
	CarryingObjectLeftHandSocketTransform = _LeftHand;
	CarryingObjectRightHandSocketTransform = _RightHand;
}

void UPlayerAnimInstance::SetCarryingObjectLocationOffset(const FVector& _Offset)
{
	CarryingObjectLocationOffset = _Offset;
}
