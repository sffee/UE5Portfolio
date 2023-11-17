#include "BOTWMathLibrary.h"

bool UBOTWMathLibrary::AngleInRange(float _Angle, float _MinAngle, float _MaxAngle, float _Buffer, bool _IncreaseBuffer)
{
	if (_IncreaseBuffer)
	{
		return _MinAngle - _Buffer <= _Angle && _Angle <= _MaxAngle + _Buffer;
	}
	return _MinAngle + _Buffer <= _Angle && _Angle <= _MaxAngle - _Buffer;
}

EMovementDirection UBOTWMathLibrary::CalculateQuadrant(EMovementDirection _Current, float _FRThreshold, float _FLThreshold, float _BRThreshold, float _BLThreshold, float _Buffer, float _Angle)
{
	if (AngleInRange(_Angle, _FLThreshold, _FRThreshold, _Buffer,
		_Current != EMovementDirection::Forward && _Current != EMovementDirection::Backward))
	{
		return EMovementDirection::Forward;
	}

	if (AngleInRange(_Angle, _FRThreshold, _BRThreshold, _Buffer,
		_Current != EMovementDirection::Right && _Current != EMovementDirection::Left))
	{
		return EMovementDirection::Right;
	}

	if (AngleInRange(_Angle, _BLThreshold, _FLThreshold, _Buffer,
		_Current != EMovementDirection::Right && _Current != EMovementDirection::Left))
	{
		return EMovementDirection::Left;
	}

	return EMovementDirection::Backward;
}