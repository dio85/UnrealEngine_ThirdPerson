// Copyright 2018 wevet works All Rights Reserved.


#include "Item/AbstractItem.h"
#include "Character/CharacterBase.h"
#include "Engine.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "WevetExtension.h"


AAbstractItem::AAbstractItem(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = true;

	CollisionComponent = ObjectInitializer.CreateDefaultSubobject<USphereComponent>(this, TEXT("CollisionComponent"));
	CollisionComponent->SetSphereRadius(90.0f);
	RootComponent = CollisionComponent;

	StaticMeshComponent = ObjectInitializer.CreateDefaultSubobject<UStaticMeshComponent>(this, TEXT("StaticMeshComponent"));
	StaticMeshComponent->SetupAttachment(CollisionComponent);
	StaticMeshComponent->bRenderCustomDepth = false;
	StaticMeshComponent->CustomDepthStencilValue = 0;
	StaticMeshComponent->bUseAttachParentBound = 1;

	WidgetComponent = ObjectInitializer.CreateDefaultSubobject<UWidgetComponent>(this, TEXT("WidgetComponent"));
	WidgetComponent->SetDrawSize(FVector2D(250.f, 70.f));
	WidgetComponent->SetWorldLocation(FVector(0.f, 0.f, 60.f));
	WidgetComponent->SetupAttachment(CollisionComponent);
	WidgetComponent->bUseAttachParentBound = 1;
	WidgetComponent->SetVisibility(false);
}


void AAbstractItem::BeginPlay()
{
	Super::BeginPlay();
	Super::SetActorTickEnabled(false);
	AddDelegate();
}


void AAbstractItem::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Target = nullptr;
	Super::EndPlay(EndPlayReason);
}


void AAbstractItem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}


#pragma region Interface
void AAbstractItem::Release_Implementation(APawn* NewCharacter)
{
	if (PickupSound)
	{
		UGameplayStatics::PlaySoundAtLocation(GetWorld(), PickupSound, GetActorLocation());
	}

	if (NewCharacter)
	{
		IInteractionPawn::Execute_OverlapActor(NewCharacter, nullptr);
	}

	Target = nullptr;
	RemoveDelegate();

	Super::MarkRenderStateDirty(StaticMeshComponent, false, ECustomDepthType::None);
	Super::SetActorTickEnabled(false);
	WidgetComponent->SetVisibility(false);

	if (IsValidLowLevel())
	{
		Super::Destroy();
		Super::ConditionalBeginDestroy();
	}

}


void AAbstractItem::SpawnToWorld_Implementation()
{
	if (StaticMeshComponent)
	{
		StaticMeshComponent->SetCollisionEnabled(ECollisionEnabled::Type::QueryAndPhysics);
		StaticMeshComponent->SetEnableGravity(true);
		StaticMeshComponent->SetCollisionProfileName(TEXT("Ragdoll"));
		StaticMeshComponent->SetSimulatePhysics(true);
		StaticMeshComponent->WakeAllRigidBodies();
	}
}


EItemType AAbstractItem::GetItemType_Implementation() const
{
	return WeaponAmmoInfo.BaseItemType;
}
#pragma endregion


#pragma region Override
void AAbstractItem::BeginOverlapRecieve(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	IInteractionPawn* Interface = Cast<IInteractionPawn>(OtherActor);
	if (Interface == nullptr)
	{
		return;
	}

	if (IInteractionPawn::Execute_CanPickup(Interface->_getUObject()))
	{
		OverlapActor(OtherActor);
		Super::MarkRenderStateDirty(StaticMeshComponent, true, ECustomDepthType::Item);
		IInteractionPawn::Execute_OverlapActor(Interface->_getUObject(), this);
	}
}


void AAbstractItem::EndOverlapRecieve(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	IInteractionPawn* Interface = Cast<IInteractionPawn>(OtherActor);
	if (Interface == nullptr)
	{
		return;
	}

	if (IInteractionPawn::Execute_CanPickup(Interface->_getUObject()))
	{
		OverlapActor(nullptr);
		Super::MarkRenderStateDirty(StaticMeshComponent, false, ECustomDepthType::None);
		IInteractionPawn::Execute_OverlapActor(Interface->_getUObject(), nullptr);
	}
}


void AAbstractItem::AddDelegate()
{
	if (CollisionComponent)
	{
		CollisionComponent->OnComponentBeginOverlap.AddDynamic(this, &AAbstractItem::BeginOverlapRecieve);
		CollisionComponent->OnComponentEndOverlap.AddDynamic(this, &AAbstractItem::EndOverlapRecieve);
	}
}


void AAbstractItem::RemoveDelegate()
{
	if (CollisionComponent)
	{
		CollisionComponent->OnComponentBeginOverlap.RemoveDynamic(this, &AAbstractItem::BeginOverlapRecieve);
		CollisionComponent->OnComponentEndOverlap.RemoveDynamic(this, &AAbstractItem::EndOverlapRecieve);
	}
}


void AAbstractItem::OverlapActor(AActor* OtherActor)
{
	Target = OtherActor;
	Super::SetActorTickEnabled(Target ? true : false);
	WidgetComponent->SetVisibility(Target ? true : false);
}
#pragma endregion

