#pragma once

#include "InputActionValue.h"
#include "Library/GASLibrary.h"

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ClimbingComponent.generated.h"

class APlayerCharacter;
class UBOTWCharacterMovementComponent;
class UAbilitySystemComponent;
class UInputMappingContext;
class UBOTWInputConfig;
class UAnimMontage;
class UCapsuleComponent;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class BOTW_API UClimbingComponent : public UActorComponent
{
	GENERATED_BODY()

private:
	UPROPERTY()
	APlayerCharacter* Player;

	UPROPERTY()
	UBOTWCharacterMovementComponent* CharacterMovementComponent;

	UPROPERTY()
	UBOTWAbilitySystemComponent* AbilitySystemComponent;

	UPROPERTY()
	UCapsuleComponent* PlayerCapsuleComponent;

private:
	UPROPERTY(EditDefaultsOnly, Category = "BOTW|Input")
	UBOTWInputConfig* ClimbingInputConfig;

	UPROPERTY(EditDefaultsOnly, Category = "BOTW|Input")
	UInputMappingContext* ClimbingInputMappingContext;

private:
	UPROPERTY(EditDefaultsOnly, Category = "BOTW|Animation")
	UAnimMontage* Up_Start_AnimMontage;

	UPROPERTY(EditDefaultsOnly, Category = "BOTW|Animation")
	UAnimMontage* Up_End_AnimMontage;

	UPROPERTY(EditDefaultsOnly, Category = "BOTW|Animation")
	UAnimMontage* Cancel_AnimMontage;

private:
	UPROPERTY(EditAnywhere, Category = "BOTW|Debug")
	bool IsDrawDebug;

	TArray<FHitResult> SurfaceHitResults;
	FVector SurfaceLocation;
	FVector SurfaceNormal;

	bool CanStartClimbing;
	bool EndedClimbing;

	UPROPERTY(EditAnywhere, Category = "BOTW|Trace")
	float TraceUpDistance;

	UPROPERTY(EditAnywhere, Category = "BOTW|Trace")
	float TraceForwardDistance;

private:
	UPROPERTY(EditAnywhere, Category = "BOTW|Collision")
	float ClimbingCapsuleHalfHeight;

	float OriginCapsuleHalfHeight;


public:
	FSimpleMulticastDelegate OnEndClimbing;

public:	
	UClimbingComponent();

protected:
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

public:
	bool CheckStartClimbing();
	void StartClimbing();
	void EndClimbing();
	
private:
	void ReachFloor();
	void ClimbUpLedge();

private:
	void BindClimbingActions();
	void BindClimbingInputs();

	void UnBindClimbingInputs();

	void PlayMontage(UAnimMontage* _Montage);

public:
	void CalcSurfaceInfo();

private:
	bool TraceSurface(const FVector& _Offset = FVector());
	FHitResult TraceHead();

	bool CheckFloor();
	bool CheckLedge();

	TArray<FHitResult> CapsuleTraceMulti(const FVector& _Start, const FVector& _End, float _Radius = 50.f, float _HalfHeight = 72.f);
	FHitResult LineTraceSingle(const FVector& _Start, const FVector& _End);

private:
	UFUNCTION()
	void Climbing_MoveForward(const FInputActionValue& _Value);

	UFUNCTION()
	void Climbing_MoveSide(const FInputActionValue& _Value);

private:
	UFUNCTION()
	void OnMontageEnded(UAnimMontage* _Montage, bool _Interrupted);

public:
	FORCEINLINE bool GetCanStartClimbing() const { return CanStartClimbing; }
	FORCEINLINE bool GetEndedClimbing() const { return EndedClimbing; }
	FORCEINLINE FVector GetSurfaceLocation() const { return SurfaceLocation; }
	FORCEINLINE FVector GetSurfaceNormal() const { return SurfaceNormal; }
	FVector GetClimbVelocity() const;
};
