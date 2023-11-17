#include "CarryComponent.h"

#include "Components/BoxComponent.h"
#include "Components/MeshComponent.h"
#include "Character/Player/PlayerCharacter.h"
#include "Character/Player/PlayerAnimInstance.h"
#include "Interface/CarryableInterface.h"
#include "Equipment/EquipmentManagerComponent.h"
#include "Kismet/KismetMathLibrary.h"

UCarryComponent::UCarryComponent()
	: IsCarrying(false)
	, IsDropStart(false)
{
	PrimaryComponentTick.bCanEverTick = true;

	CarryBoxComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("CarryBoxComponent"));
	if (CarryBoxComponent)
	{
		CarryBoxComponent->SetCollisionResponseToAllChannels(ECR_Ignore);
		CarryBoxComponent->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Overlap);
		CarryBoxComponent->SetCollisionResponseToChannel(ECC_PhysicsBody, ECR_Overlap);
	}
}

void UCarryComponent::OnRegister()
{
	Super::OnRegister();

	if (CarryBoxComponent)
	{
		CarryBoxComponent->AttachToComponent(GetOwner()->GetRootComponent(), FAttachmentTransformRules::KeepRelativeTransform);
	}
}

void UCarryComponent::BeginPlay()
{
	Super::BeginPlay();

	if (CarryBoxComponent)
	{
		CarryBoxComponent->OnComponentBeginOverlap.AddDynamic(this, &UCarryComponent::OnBeginOverlap);
		CarryBoxComponent->OnComponentEndOverlap.AddDynamic(this, &UCarryComponent::OnEndOverlap);
	}

	OwingCharacter = GetOwner<APlayerCharacter>();
	if (OwingCharacter)
	{
		PlayerAnimInstance = Cast<UPlayerAnimInstance>(OwingCharacter->GetMesh()->GetAnimInstance());
		AbilitySystemComponent = OwingCharacter->GetAbilitySystemComponent();

		GameplayEventTagDelegateHandle = AbilitySystemComponent->AddGameplayEventTagContainerDelegate(FGameplayTagContainer(GAMEPLAYTAG.Event_Character_Carry),
			FGameplayEventTagMulticastDelegate::FDelegate::CreateUObject(this, &UCarryComponent::OnGameplayEvent));

		OwingCharacter->GetMesh()->AddTickPrerequisiteComponent(this);
	}
}

void UCarryComponent::TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (CarryingObject)
	{
		if (ICarryableInterface* carryableInterface = Cast<ICarryableInterface>(CarryingObject))
		{
			FTransform leftHandSocketTransform = carryableInterface->GetLeftHandSocketTransform();
			leftHandSocketTransform.SetToRelativeTransform(OwingCharacter->GetMesh()->GetComponentTransform());

			FTransform rightHandSocketTransform = carryableInterface->GetRightHandSocketTransform();
			rightHandSocketTransform.SetToRelativeTransform(OwingCharacter->GetMesh()->GetComponentTransform());

			PlayerAnimInstance->SetCarryingObjectSocketTransform(leftHandSocketTransform, rightHandSocketTransform);

			FVector locationOffset = carryableInterface->GetLocationOffset();
			if (IsDropStart)
			{
				float dropElapsedTime = GetWorld()->GetTimeSeconds() - DropStartTime;
				locationOffset *= DropToGroundCurve.GetRichCurve()->Eval(dropElapsedTime);
			}

			PlayerAnimInstance->SetCarryingObjectLocationOffset(locationOffset);
		}
	}
}

void UCarryComponent::Carry()
{
	if (OwingCharacter)
	{
		if (UEquipmentManagerComponent* equipManagerCom = OwingCharacter->FindComponentByClass<UEquipmentManagerComponent>())
		{
			equipManagerCom->UnEquipWeapon();
		}

		OwingCharacter->GetCharacterMovement()->StopMovementImmediately();

		OverlappedActors.Sort([&](const AActor& _Actor1, const AActor& _Actor2)
			{
				FVector OwingCharacterLocation = OwingCharacter->GetActorLocation();
				return FVector::Distance(_Actor1.GetActorLocation(), OwingCharacterLocation) < FVector::Distance(_Actor2.GetActorLocation(), OwingCharacterLocation);
			});

		CarryingObject = *OverlappedActors.begin();

		FRotator targetRot = OwingCharacter->GetActorRotation();
		targetRot.Yaw = UKismetMathLibrary::FindLookAtRotation(OwingCharacter->GetActorLocation(), CarryingObject->GetActorLocation()).Yaw;
		OwingCharacter->SetActorRotation(targetRot);

		if (PlayerAnimInstance)
		{
			PlayerAnimInstance->SetOverlayType(EOverlayType::Carry);
			PlayerAnimInstance->SetIsDropCarryingObject(false);
		}
	}

	IsDropStart = false;
	IsCarrying = false;

	SetComponentTickEnabled(true);
}

void UCarryComponent::EndCarry()
{
	if (PlayerAnimInstance)
	{
		PlayerAnimInstance->SetOverlayType(EOverlayType::Default);
	}

	SetComponentTickEnabled(false);
}

bool UCarryComponent::IsCarryable() const
{
	return IsValid(OwingCharacter) && IsValid(PlayerAnimInstance) && OverlappedActors.IsEmpty() == false;
}

void UCarryComponent::DropToGround()
{
	if (IsCarrying == false)
	{
		return;
	}

	IsDropStart = true;
	DropStartTime = GetWorld()->GetTimeSeconds();

	OwingCharacter->GetCharacterMovement()->StopMovementImmediately();

	PlayerAnimInstance->SetIsDropCarryingObject(true);
}

void UCarryComponent::CarryObject()
{
	TArray<UActorComponent*> actorComponents = CarryingObject->GetComponentsByClass(UPrimitiveComponent::StaticClass());
	for (UActorComponent* actorCom : actorComponents)
	{
		if (UPrimitiveComponent* primitiveCom = Cast<UPrimitiveComponent>(actorCom))
		{
			FCarryingObjectCachedData cachedData;
			cachedData.CollisionType = primitiveCom->GetCollisionEnabled();
			cachedData.IsSimulatedPhysics = primitiveCom->GetBodyInstance() ? primitiveCom->GetBodyInstance()->bSimulatePhysics : false;

			CachedCarryingObjectData.Add(primitiveCom, cachedData);

			primitiveCom->SetSimulatePhysics(false);
			primitiveCom->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		}
	}

	if (OwingCharacter)
	{
		CarryingObject->SetActorRotation(FRotator(0.f, OwingCharacter->GetActorRotation().Yaw + 180.f, 0.f));

		FAttachmentTransformRules transformRules(EAttachmentRule::SnapToTarget, EAttachmentRule::KeepWorld, EAttachmentRule::KeepWorld, true);
		CarryingObject->AttachToComponent(OwingCharacter->GetMesh(), transformRules, FName("VB Carrying"));
	}

	IsCarrying = true;
}

void UCarryComponent::DropObject()
{
	if (OwingCharacter && CarryingObject)
	{
		CarryingObject->DetachRootComponentFromParent();
		CarryingObject = nullptr;

		for (auto& [primitiveCom, cachedData] : CachedCarryingObjectData)
		{
			if (primitiveCom->GetBodyInstance())
			{
				primitiveCom->SetSimulatePhysics(cachedData.IsSimulatedPhysics);
			}
			primitiveCom->SetCollisionEnabled(cachedData.CollisionType);
		}

		CachedCarryingObjectData.Empty();
	}
}

void UCarryComponent::OnGameplayEvent(FGameplayTag _EventTag, const FGameplayEventData* _Payload)
{
	if (_EventTag == GAMEPLAYTAG.Event_Character_Carry_CarryObject)
	{
		CarryObject();
	}
	else if (_EventTag == GAMEPLAYTAG.Event_Character_Carry_DropObject)
	{
		DropObject();
	}
}

void UCarryComponent::OnBeginOverlap(UPrimitiveComponent* _OverlappedComponent, AActor* _OtherActor, UPrimitiveComponent* _OtherComp, int32 _OtherBodyIndex, bool _bFromSweep, const FHitResult& _SweepResult)
{
	ICarryableInterface* carryableInterface = Cast<ICarryableInterface>(_OtherActor);
	if (carryableInterface)
	{
		OverlappedActors.Add(_OtherActor);
	}
}

void UCarryComponent::OnEndOverlap(UPrimitiveComponent* _OverlappedComponent, AActor* _OtherActor, UPrimitiveComponent* _OtherComp, int32 _OtherBodyIndex)
{
	if (OverlappedActors.Contains(_OtherActor))
	{
		OverlappedActors.Remove(_OtherActor);
	}
}