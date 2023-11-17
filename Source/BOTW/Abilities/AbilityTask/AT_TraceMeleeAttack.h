#pragma once

#include "Library/GASLibrary.h"

#include "CoreMinimal.h"
#include "Abilities/Tasks/AbilityTask.h"
#include "AT_TraceMeleeAttack.generated.h"

DECLARE_DELEGATE_OneParam(FOnHit, const FHitResult&);

class AEquipmentActor;

UCLASS()
class BOTW_API UAT_TraceMeleeAttack : public UAbilityTask
{
	GENERATED_BODY()
	
private:
	UPROPERTY()
	AActor* OwingActor;

	UPROPERTY()
	AEquipmentActor* WeaponActor;

	UPROPERTY()
	UMeshComponent* Mesh;

	FGameplayTagContainer EventTagContainer;
	FDelegateHandle EventHandle;

	float TraceSphereRadius;

	float DrawDebugDuration;

	bool IsTrace;

public:
	FOnHit OnHit;

private:
	UPROPERTY()
	TArray<AActor*> HitArray;

	UPROPERTY()
	TArray<FName> SocketNames;

	UPROPERTY()
	TArray<FVector> LastSocketLocations;

	UPROPERTY()
	TArray<AActor*> IgnoreActors;

public:
	UAT_TraceMeleeAttack(const FObjectInitializer& ObjectInitializer);

	UFUNCTION()
	static UAT_TraceMeleeAttack* TraceMeleeAttack(
		UGameplayAbility* _OwningAbility,
		AEquipmentActor* _WeaponActor,
		float _TraceSphereRadius,
		float _DrawDebugDuration);

public:
	virtual void Activate() override;
	virtual void TickTask(float DeltaTime);
	virtual void OnDestroy(bool AbilityEnded) override;

public:
	void HitArrayClear();

private:
	void Trace(TArray<FHitResult>& _HitResult);
	void Trace(const FVector& _TraceStart, const FVector& _TraceEnd, TArray<FHitResult>& _HitResult);

private:
	void OnGameplayEvent(FGameplayTag _EventTag, const FGameplayEventData* _Payload);
};
