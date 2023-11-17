#pragma once

#include "Curves/CurveFloat.h"
#include "Library/GASLibrary.h"

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "CarryComponent.generated.h"

class UBoxComponent;
class APlayerCharacter;
class UPlayerAnimInstance;
class UAnimMontage;
class UBOTWAbilitySystemComponent;

USTRUCT(BlueprintType)
struct FCarryingObjectCachedData
{
	GENERATED_BODY()

	ECollisionEnabled::Type CollisionType;
	bool IsSimulatedPhysics;

	FCarryingObjectCachedData()
		: CollisionType(ECollisionEnabled::NoCollision)
		, IsSimulatedPhysics(false)
	{
	}
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class BOTW_API UCarryComponent : public UActorComponent
{
	GENERATED_BODY()

private:
	UPROPERTY(EditAnywhere)
	UBoxComponent* CarryBoxComponent;

private:
	UPROPERTY()
	APlayerCharacter* OwingCharacter;

	UPROPERTY()
	UPlayerAnimInstance* PlayerAnimInstance;

	UPROPERTY()
	UBOTWAbilitySystemComponent* AbilitySystemComponent;

	FDelegateHandle GameplayEventTagDelegateHandle;

private:
	TSet<AActor*> OverlappedActors;

	UPROPERTY()
	AActor* CarryingObject;

	TMap<UPrimitiveComponent*, FCarryingObjectCachedData> CachedCarryingObjectData;

	bool IsCarrying;
	
private:
	UPROPERTY(EditAnywhere, Category = "Curves")
	FRuntimeFloatCurve DropToGroundCurve;

	bool IsDropStart;
	float DropStartTime;

private:
	UPROPERTY()
	UAnimMontage* LaunchCarryObject;

public:	
	UCarryComponent();

protected:
	virtual void OnRegister() override;
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

public:
	void Carry();
	void EndCarry();

	bool IsCarryable() const;

	void DropToGround();

private:
	void CarryObject();
	void DropObject();

private:
	void OnGameplayEvent(FGameplayTag _EventTag, const FGameplayEventData* _Payload);

private:
	UFUNCTION()
	void OnBeginOverlap(UPrimitiveComponent* _OverlappedComponent, AActor* _OtherActor, UPrimitiveComponent* _OtherComp, int32 _OtherBodyIndex, bool _bFromSweep, const FHitResult& _SweepResult);

	UFUNCTION()
	void OnEndOverlap(UPrimitiveComponent* _OverlappedComponent, AActor* _OtherActor, UPrimitiveComponent* _OtherComp, int32 _OtherBodyIndex);
};
