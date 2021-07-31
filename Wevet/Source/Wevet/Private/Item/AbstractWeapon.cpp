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


AAbstractWeapon::AAbstractWeapon(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = true;
	bRenderCutomDepthEnable = true;

	SetEquip(false);
	SetReload(false);
	SetFired(false);
	SetMeleeAttack(false);

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

	// Melee
	MeleeCollisionComponent = ObjectInitializer.CreateDefaultSubobject<UBoxComponent>(this, TEXT("MeleeCollision"));
	MeleeCollisionComponent->SetupAttachment(RootComponent);
	MeleeCollisionComponent->SetNotifyRigidBodyCollision(true);
	MeleeCollisionComponent->SetGenerateOverlapEvents(false);
	MeleeCollisionComponent->CanCharacterStepUpOn = ECanBeCharacterBase::ECB_No;
}


void AAbstractWeapon::BeginPlay()
{
	Super::BeginPlay();
	Super::SetActorTickEnabled(false);
	AddDelegate();

	if (MeleeCollisionComponent)
	{
		MeleeCollisionComponent->OnComponentBeginOverlap.AddDynamic(this, &AAbstractWeapon::OnMeleeOverlapRecieve);
	}
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
		IgnoreActors.Add(GetPawnOwner());
		IgnoreActors.Add(this);
		Super::SetOwner(GetPawnOwner());
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


void AAbstractWeapon::DoMeleeAttack_Implementation(const bool InEnableMeleeAttack)
{
	if (bWasMeleeAttack == InEnableMeleeAttack)
	{
		return;
	}

	SetMeleeAttack(InEnableMeleeAttack);
	MeleeCollisionComponent->SetGenerateOverlapEvents(bWasMeleeAttack);

	//const float Duration = 2.0f;
	//const FString Str = FString::Printf(TEXT("MeleeAttack => %s"), bWasMeleeAttack ? TEXT("begin") : TEXT("end"));
	//UWevetBlueprintFunctionLibrary::DrawDebugString(GetPawnOwner(), Str, FLinearColor::Green, Duration);
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


bool AAbstractWeapon::CanMeleeStrike_Implementation() const
{
	//unsafe pointer
	if (!CharacterPtr.IsValid())
	{
		return false;
	}

	if (ICombatInstigator* Interface = Cast<ICombatInstigator>(GetPawnOwner()))
	{
		if (Interface == nullptr)
		{
			return false;
		}

		if (ICombatInstigator::Execute_IsDeath(Interface->_getUObject()))
		{
			return false;
		}
	}


	if (!WasEquip())
	{
		UE_LOG(LogWevetClient, Log, TEXT("No Equip => %s"), *FString(__FUNCTION__));
		return false;
	}

	if (WasReload())
	{
		UE_LOG(LogWevetClient, Log, TEXT("Reload => %s"), *FString(__FUNCTION__));
		return false;
	}

	if (WasMeleeAttack())
	{
		UE_LOG(LogWevetClient, Log, TEXT("MeleeAttack => %s"), *FString(__FUNCTION__));
		return false;
	}

	if (WasEmpty())
	{
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


#pragma region HitEvent
void AAbstractWeapon::BeginOverlapRecieve(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	IInteractionPawn* Interface = Cast<IInteractionPawn>(OtherActor);
	if (Interface == nullptr)
	{
		return;
	}

	if (IInteractionPawn::Execute_CanPickup(Interface->_getUObject()))
	{
		Super::MarkRenderStateDirty(SkeletalMeshComponent, true, ECustomDepthType::Weapon);
		IInteractionPawn::Execute_OverlapActor(Interface->_getUObject(), this);
	}
}


void AAbstractWeapon::EndOverlapRecieve(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	IInteractionPawn* Interface = Cast<IInteractionPawn>(OtherActor);
	if (Interface == nullptr)
	{
		return;
	}

	if (IInteractionPawn::Execute_CanPickup(Interface->_getUObject()))
	{
		Super::MarkRenderStateDirty(SkeletalMeshComponent, false, ECustomDepthType::None);
		IInteractionPawn::Execute_OverlapActor(Interface->_getUObject(), nullptr);
	}
}


void AAbstractWeapon::OnMeleeOverlapRecieve(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (UCombatBlueprintFunctionLibrary::CanDamagedActor(SweepResult.GetActor(), this, GetPawnOwner()))
	{
		GiveDamageType = EGiveDamageType::Melee;
		TakeDamageOuter(SweepResult);

#if WITH_EDITOR
		const float Duration = 4.0f;
		const FString Str = FString::Printf(TEXT("MeleeAttack => %s"), *SweepResult.GetActor()->GetName());
		UWevetBlueprintFunctionLibrary::DrawDebugString(GetPawnOwner(), Str, FLinearColor::Green, Duration);
#endif
	}
}
#pragma endregion


void AAbstractWeapon::OnFirePressInternal()
{
	if (!IWeaponInstigator::Execute_CanMeleeStrike(this))
	{
		return;
	}

	if (!bCanFire)
	{
		return;
	}

	if (WeaponItemInfo.EmptyCurrentAmmo())
	{
		IWeaponInstigator::Execute_DoReload(this);
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
	ICombatInstigator::Execute_FireActionMontage(GetPawnOwner());
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

	Bullet->SetOwners(IgnoreActors);
	const bool bCanDamageResult = UCombatBlueprintFunctionLibrary::CanDamagedActor(HitResult.GetActor(), this, GetPawnOwner());
	if (bCanDamageResult)
	{
		GiveDamageType = EGiveDamageType::Shoot;
		TakeDamageOuter(HitResult);
	}

	const float BulletImpactRange = 1400.f;
	ISoundInstigator::Execute_ReportNoiseOther(GetPawnOwner(), Bullet, ImpactSound, DEFAULT_VOLUME, ImpactLocation);
	UAISense_Hearing::ReportNoiseEvent(GetWorld(), ImpactLocation, DEFAULT_VOLUME, Bullet, BulletImpactRange);

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
	float TotalDamage = UCombatBlueprintFunctionLibrary::CalcurateBaseDamage(HitResult, this, GetPawnOwner(), WeaponItemInfo.Damage);

	switch (GiveDamageType)
	{
		case EGiveDamageType::Shoot:
		{
			//
		}
		break;
		case EGiveDamageType::Melee:
		{
			// override melee damage
			TotalDamage = WeaponItemInfo.ConvertMeleeDamage(TotalDamage);
		}
		break;
	}

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

	if (!HitResult.GetActor()->IsA(ACharacterBase::StaticClass()))
	{
		// @TODO
		// Actors with a WaterBody tag will not show ImpactEmitter
		if (!HitResult.GetActor()->ActorHasTag(WATER_BODY_TAG))
		{
			FTransform EmitterTransform;
			EmitterTransform.SetIdentity();
			EmitterTransform.SetLocation(HitResult.ImpactPoint);
			UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ImpactEmitterTemplate, EmitterTransform, true);
		}
		OutHitActor = true;
	}
	else
	{
		UE_LOG(LogWevetClient, Warning, TEXT("Failed HitActor : %s"), *HitResult.GetActor()->GetName());
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


void AAbstractWeapon::PrepareDestroy()
{
	FTimerManager& TimerManager = GetWorldTimerManager();
	if (TimerManager.IsTimerActive(ReloadTimerHandle))
	{
		TimerManager.ClearTimer(ReloadTimerHandle);
		TimerManager.ClearAllTimersForObject(this);
	}

	if (MeleeCollisionComponent)
	{
		MeleeCollisionComponent->OnComponentBeginOverlap.RemoveDynamic(this, &AAbstractWeapon::OnMeleeOverlapRecieve);
	}
}


#pragma region Delegate
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
#pragma endregion

