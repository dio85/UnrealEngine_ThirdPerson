// Copyright 2018 wevet works All Rights Reserved.


#include "Item/AbstractWeapon.h"
#include "Character/CharacterBase.h"
#include "Engine.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Wevet.h"
#include "WevetExtension.h"
#include "Lib/WevetBlueprintFunctionLibrary.h"
#include "Lib/CombatBlueprintFunctionLibrary.h"
#include "Perception/AISense_Hearing.h"


#define IMPACT_RANGE 1400.f


AAbstractWeapon::AAbstractWeapon(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = true;
	bRenderCutomDepthEnable = true;

	SetEquip(false);
	SetReload(false);
	SetFired(false);

	GiveDamageType = EGiveDamageType::None;
	MuzzleSocketName = FName(TEXT("MuzzleFlash"));
	GripSocketName = FName(TEXT("Grip"));

	SkeletalMeshComponent = ObjectInitializer.CreateDefaultSubobject<USkeletalMeshComponent>(this, TEXT("SkeletalMeshComponent"));
	RootComponent = SkeletalMeshComponent;
	SkeletalMeshComponent->SetSimulatePhysics(false);
	SkeletalMeshComponent->PhysicsTransformUpdateMode = EPhysicsTransformUpdateMode::SimulationUpatesComponentTransform;
	SkeletalMeshComponent->SetCollisionProfileName(TEXT("BlockAll"));
	SkeletalMeshComponent->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	SkeletalMeshComponent->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	SkeletalMeshComponent->SetCollisionEnabled(ECollisionEnabled::Type::PhysicsOnly);
	SkeletalMeshComponent->SetEnableGravity(false);

	// Overlap 
	CollisionComponent = ObjectInitializer.CreateDefaultSubobject<UBoxComponent>(this, TEXT("CollisionComponent"));
	CollisionComponent->SetupAttachment(RootComponent);
}


void AAbstractWeapon::BeginPlay()
{
	Super::BeginPlay();
	Super::SetActorTickEnabled(false);
	AddDelegate();
}


void AAbstractWeapon::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	ResetCharacterOwner();
	PrepareDestroy();
	Super::EndPlay(EndPlayReason);
}


#pragma region Public
void AAbstractWeapon::Initialize(APawn* const NewCharacterOwner)
{
	CharacterPtr = MakeWeakObjectPtr<ACharacterBase>(Cast<ACharacterBase>(NewCharacterOwner));
	if (GetPawnOwner())
	{
		Super::Initialize(GetPawnOwner());
	}
}


ACharacterBase* AAbstractWeapon::GetPointer() const
{
	return Cast<ACharacterBase>(GetPawnOwner());
}


APawn* AAbstractWeapon::GetPawnOwner() const
{
	if (CharacterPtr.IsValid())
	{
		return CharacterPtr.Get();
	}
	return nullptr;
}


void AAbstractWeapon::ReloadBulletParams()
{
	WeaponItemInfo.NeededAmmo = (WeaponItemInfo.ClipType - WeaponItemInfo.CurrentAmmo);
	WeaponItemInfo.Replenishment();
}


void AAbstractWeapon::SetOwnerNoSeeMesh(const bool NewOwnerNoSee)
{
	if (SkeletalMeshComponent)
	{
		SkeletalMeshComponent->SetOwnerNoSee(NewOwnerNoSee);
	}
}


void AAbstractWeapon::ResetCharacterOwner()
{
	CharacterPtr.Reset();
	IgnoreActors.Reset(0);
	Super::SetOwner(nullptr);
}


void AAbstractWeapon::SetEquip(const bool InEquip)
{
	bEquip = InEquip;

	if (GetPawnOwner())
	{
		const FString Str = bEquip ? FString(TEXT("Equip")) : FString(TEXT("UnEquip"));
		UWevetBlueprintFunctionLibrary::DrawDebugString(GetPawnOwner(), Str);
	}
}


void AAbstractWeapon::CopyWeaponItemInfo(const FWeaponItemInfo* RefWeaponItemInfo)
{
	WeaponItemInfo.CurrentAmmo = RefWeaponItemInfo->CurrentAmmo;
	WeaponItemInfo.MaxAmmo = RefWeaponItemInfo->MaxAmmo;
	RefWeaponItemInfo = nullptr;
}
#pragma endregion


#pragma region Interface
void AAbstractWeapon::DoFirePressed_Implementation() 
{
	SetFired(true);
}


void AAbstractWeapon::DoFireRelease_Implementation()
{
	SetFired(false);
}


void AAbstractWeapon::DoReload_Implementation()
{
	if (bReload)
	{
		return;
	}

	if (WeaponItemInfo.EmptyAmmo())
	{
		UE_LOG(LogWevetClient, Log, TEXT("Empty Ammos Current:%d, ClipType:%d"), WeaponItemInfo.CurrentAmmo, WeaponItemInfo.ClipType);
		return;
	}

	if (WeaponItemInfo.CurrentFullAmmo())
	{
		UE_LOG(LogWevetClient, Log, TEXT("Full Ammos Current:%d, ClipType:%d"), WeaponItemInfo.CurrentAmmo, WeaponItemInfo.ClipType);
		return;
	}

	SetFired(false);
	SetReload(true);

	float ReloadDuration = DEFAULT_VALUE;
	ICombatInstigator::Execute_ReloadActionMontage(GetPawnOwner(), ReloadDuration);
	FTimerDelegate TimerCallback;
	TimerCallback.BindLambda([this]()
	{
		SetReload(false);
	});

	GetWorldTimerManager().SetTimer(ReloadTimerHandle, TimerCallback, ReloadDuration, false);
}


void AAbstractWeapon::DoReplenishment_Implementation(const int32 InAddAmmo)
{
	WeaponItemInfo.MaxAmmo += InAddAmmo;
	WeaponItemInfo.Replenishment();
}


bool AAbstractWeapon::CanStrike_Implementation() const
{
	FString Message;
	//unsafe pointer
	if (!CharacterPtr.IsValid())
	{
		Message.Append(TEXT("Unsafe Pointer"));
		UE_LOG(LogWevetClient, Error, TEXT("%s => %s"), *Message, *FString(__FUNCTION__));
		return false;
	}

	if (ICombatInstigator* Interface = Cast<ICombatInstigator>(GetPawnOwner()))
	{
		if (Interface == nullptr)
		{
			Message.Append(TEXT("Pawn Dont have ICombatInstigator Interface"));
			UE_LOG(LogWevetClient, Error, TEXT("%s => %s"), *Message, *FString(__FUNCTION__));
			return false;
		}

		if (ICombatInstigator::Execute_IsDeath(Interface->_getUObject()))
		{
			Message.Append(TEXT("Pawn Death"));
			UE_LOG(LogWevetClient, Error, TEXT("%s => %s"), *Message, *FString(__FUNCTION__));
			return false;
		}
	}


	if (!WasEquip())
	{
		Message.Append(TEXT("No Equip"));
		UE_LOG(LogWevetClient, Warning, TEXT("%s => %s"), *Message, *FString(__FUNCTION__));
		return false;
	}

	if (WasReload())
	{
		Message.Append(TEXT("Now Reloading..."));
		UE_LOG(LogWevetClient, Warning, TEXT("%s => %s"), *Message, *FString(__FUNCTION__));
		return false;
	}

	if (WasEmpty())
	{
		Message.Append(TEXT("EmptyAmmos"));
		UE_LOG(LogWevetClient, Log, TEXT("EmptyAmmos => %s"), *FString(__FUNCTION__));
		return false;
	}

	return true;
}


void AAbstractWeapon::Take_Implementation(APawn* NewCharacter)
{
	ResetCharacterOwner();
	Initialize(NewCharacter);

	if (PickupSound)
	{
		UGameplayStatics::PlaySoundAtLocation(GetWorld(), PickupSound, GetActorLocation());
	}

	Super::MarkRenderStateDirty(SkeletalMeshComponent, false, ECustomDepthType::None);
	RemoveDelegate();
	if (CollisionComponent)
	{
		CollisionComponent->ConditionalBeginDestroy();
		CollisionComponent = nullptr;
	}
}


void AAbstractWeapon::Release_Implementation(APawn* NewCharacter)
{
	ResetCharacterOwner();
	SetEquip(false);

	// If Player Postprocess is enabled, disable it
	if (NewCharacter)
	{
		IInteractionPawn::Execute_OverlapActor(NewCharacter, nullptr);
	}

	if (IsValidLowLevel())
	{
		PrepareDestroy();
		Super::Destroy();
		Super::ConditionalBeginDestroy();
	}	
}


void AAbstractWeapon::SpawnToWorld_Implementation()
{
	if (SkeletalMeshComponent)
	{
		SkeletalMeshComponent->SetCollisionEnabled(ECollisionEnabled::Type::QueryAndPhysics);
		SkeletalMeshComponent->SetEnableGravity(true);
		SkeletalMeshComponent->SetCollisionProfileName(TEXT("Ragdoll"));
		SkeletalMeshComponent->SetAllBodiesSimulatePhysics(true);
		SkeletalMeshComponent->WakeAllRigidBodies();
	}
}


EItemType AAbstractWeapon::GetItemType_Implementation() const
{
	return WeaponItemInfo.GetItemType();
}


EGiveDamageType AAbstractWeapon::GetGiveDamageType_Implementation() const
{
	return GiveDamageType;
}
#pragma endregion


#pragma region Override
void AAbstractWeapon::BeginOverlapRecieve(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	Super::BeginOverlapRecieve(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);
}


void AAbstractWeapon::EndOverlapRecieve(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	Super::EndOverlapRecieve(OverlappedComp, OtherActor, OtherComp, OtherBodyIndex);
}


void AAbstractWeapon::AddDelegate()
{
	if (CollisionComponent)
	{
		if (!CollisionComponent->OnComponentBeginOverlap.IsBound())
		{
			CollisionComponent->OnComponentBeginOverlap.AddDynamic(this, &AAbstractWeapon::BeginOverlapRecieve);
		}
		if (!CollisionComponent->OnComponentEndOverlap.IsBound())
		{
			CollisionComponent->OnComponentEndOverlap.AddDynamic(this, &AAbstractWeapon::EndOverlapRecieve);
		}
	}
}


void AAbstractWeapon::RemoveDelegate()
{
	if (CollisionComponent)
	{
		if (CollisionComponent->OnComponentBeginOverlap.IsBound())
		{
			CollisionComponent->OnComponentBeginOverlap.RemoveDynamic(this, &AAbstractWeapon::BeginOverlapRecieve);
		}

		if (CollisionComponent->OnComponentEndOverlap.IsBound())
		{
			CollisionComponent->OnComponentEndOverlap.RemoveDynamic(this, &AAbstractWeapon::EndOverlapRecieve);
		}
	}
}


void AAbstractWeapon::OverlapActor(AActor* OtherActor)
{
	const bool bWasHitResult = OtherActor ? true : false;
	const ECustomDepthType DepthType = OtherActor ? ECustomDepthType::Weapon : ECustomDepthType::None;
	Super::MarkRenderStateDirty(SkeletalMeshComponent, bWasHitResult, DepthType);
}


void AAbstractWeapon::PrepareDestroy()
{
	FTimerManager& TimerManager = GetWorldTimerManager();
	if (TimerManager.IsTimerActive(ReloadTimerHandle))
	{
		TimerManager.ClearTimer(ReloadTimerHandle);
		TimerManager.ClearAllTimersForObject(this);
	}
}
#pragma endregion


void AAbstractWeapon::OnFirePressInternal()
{
	if (!IWeaponInstigator::Execute_CanStrike(this))
	{
		return;
	}

	if (!bCanFire)
	{
		UE_LOG(LogWevetClient, Warning, TEXT("bCanFire False => %s"), *FString(__FUNCTION__));
		return;
	}

	if (WeaponItemInfo.EmptyCurrentAmmo())
	{
		IWeaponInstigator::Execute_DoReload(this);
		UE_LOG(LogWevetClient, Warning, TEXT("EmptyCurrentAmmo => %s"), *FString(__FUNCTION__));
		return;
	}


	const FVector StartLocation = ICombatInstigator::Execute_BulletTraceRelativeLocation(GetPawnOwner()); 
	const FVector EndLocation = ICombatInstigator::Execute_BulletTraceForwardLocation(GetPawnOwner());

	FHitResult HitData(ForceInit);
	FCollisionQueryParams CollisionQueryParams;
	CollisionQueryParams.TraceTag = DAMAGE_TAG;
	CollisionQueryParams.OwnerTag = CHARACTER_TAG;
	CollisionQueryParams.bTraceComplex = true;
	CollisionQueryParams.bFindInitialOverlaps = false;
	CollisionQueryParams.bReturnFaceIndex = false;
	CollisionQueryParams.bReturnPhysicalMaterial = false;
	CollisionQueryParams.bIgnoreBlocks = false;
	CollisionQueryParams.IgnoreMask = 0;
	//CollisionQueryParams.bTraceAsyncScene = true;
	CollisionQueryParams.AddIgnoredActors(IgnoreActors);

	const bool bSuccess = GetWorld()->LineTraceSingleByChannel(HitData, StartLocation, EndLocation, ECollisionChannel::ECC_Visibility, CollisionQueryParams);
	ISoundInstigator::Execute_ReportNoiseOther(GetPawnOwner(), this, FireSound, DEFAULT_VOLUME, GetMuzzleTransform().GetLocation());

	float FireDuration = DEFAULT_VALUE;
	ICombatInstigator::Execute_FireActionMontage(GetPawnOwner(), FireDuration);
	WeaponItemInfo.DecrementAmmos();

	const FVector StartPoint = GetMuzzleTransform().GetLocation();
	const FVector EndPoint = UKismetMathLibrary::SelectVector(HitData.ImpactPoint, HitData.TraceEnd, bSuccess);
	const FRotator Rotation = UKismetMathLibrary::FindLookAtRotation(StartPoint, EndPoint);
	const FTransform Transform = UKismetMathLibrary::MakeTransform(StartPoint, Rotation, FVector::OneVector);
	ISoundInstigator::Execute_ReportNoiseOther(GetPawnOwner(), this, ImpactSound, DEFAULT_VOLUME, EndPoint);
	UAISense_Hearing::ReportNoiseEvent(GetWorld(), EndPoint, DEFAULT_VOLUME, this, WeaponItemInfo.HearingRange);


	if (ILocomotionSystemPawn::Execute_HasDebugTrace(GetPawnOwner()))
	{
		UKismetSystemLibrary::DrawDebugLine(GetWorld(), StartLocation, EndLocation, FLinearColor::Green, 2.0f, 0.0f);
		UKismetSystemLibrary::DrawDebugSphere(GetWorld(), EndLocation, 20.f, 12, FLinearColor::Green, 1.0f, 0.0f);
	}

#if WITH_EDITOR
	if (bSuccess)
	{
		if (HitData.GetActor())
		{
			UE_LOG(LogWevetClient, Log, TEXT("HitActor : %s"), *HitData.GetActor()->GetName());
		}
	}
#endif

	OnHitTriggerAction(Transform, HitData, EndPoint, bSuccess);
}


void AAbstractWeapon::OnHitTriggerAction(const FTransform InTransform, const FHitResult HitResult, const FVector ImpactLocation, const bool bHitSuccess)
{
	if (!BulletsTemplate)
	{
		return;
	}

	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = this;
	SpawnParams.Instigator = GetPawnOwner();
	ABulletBase* const Bullet = GetWorld()->SpawnActor<ABulletBase>(BulletsTemplate, InTransform, SpawnParams);

	if (!Bullet)
	{
		return;
	}

#if WITH_EDITOR
	Bullet->SetFolderPath("/Bullet");
#endif

	Bullet->Initialize(IgnoreActors);

	const bool bCanDamageResult = UCombatBlueprintFunctionLibrary::CanDamagedActor(HitResult.GetActor(), this, GetPawnOwner());
	if (bCanDamageResult)
	{
		GiveDamageType = EGiveDamageType::Shoot;
		TakeDamageOuter(HitResult);
	}

	ISoundInstigator::Execute_ReportNoiseOther(GetPawnOwner(), Bullet, ImpactSound, DEFAULT_VOLUME, ImpactLocation);
	UAISense_Hearing::ReportNoiseEvent(GetWorld(), ImpactLocation, DEFAULT_VOLUME, Bullet, IMPACT_RANGE);

	bool bWasHitActor = false;
	PlayEffect(HitResult, SkeletalMeshComponent, bWasHitActor);
}


void AAbstractWeapon::TakeDamageOuter(const FHitResult& HitResult)
{
	if (!HitResult.GetActor())
	{
		return;
	}

	// calcurate baseDamage
	const float TotalDamage = UCombatBlueprintFunctionLibrary::CalcurateBaseDamage(
		HitResult, 
		this, 
		GetPawnOwner(), 
		WeaponItemInfo.Damage, 
		WeaponItemInfo.WeaponItemType);

	CreateDamage(HitResult.GetActor(), TotalDamage, HitResult);
}


void AAbstractWeapon::PlayEffect(const FHitResult& HitResult, USceneComponent* const InComponent, bool& OutHitActor)
{
	auto PS = UGameplayStatics::SpawnEmitterAttached(
		FlashEmitterTemplate,
		InComponent,
		MuzzleSocketName,
		GetMuzzleTransform().GetLocation(),
		FRotator(GetMuzzleTransform().GetRotation()),
		EAttachLocation::KeepWorldPosition,
		true);

	if (!HitResult.GetActor())
	{
		return;
	}

	OutHitActor = HitResult.GetActor()->ActorHasTag(DAMAGE_TAG);


	if (!HitResult.GetActor()->ActorHasTag(DAMAGE_TAG))
	{
		
	}

	// @TODO
	// Actors with a WaterBody tag will not show ImpactEmitter
	if (!HitResult.GetActor()->ActorHasTag(WATER_BODY_TAG))
	{
		FTransform EmitterTransform;
		EmitterTransform.SetIdentity();
		EmitterTransform.SetLocation(HitResult.ImpactPoint);
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ImpactEmitterTemplate, EmitterTransform, true);
	}
}


void AAbstractWeapon::CreateDamage(AActor* OtherActor, const float TotalDamage, const FHitResult& HitResult)
{
	// SomeWeaponClass Override
	UGameplayStatics::ApplyPointDamage(
		OtherActor, 
		TotalDamage, 
		HitResult.ImpactPoint, 
		HitResult, 
		GetPawnOwner()->GetController(), 
		this, 
		BaseDamageType);
}


