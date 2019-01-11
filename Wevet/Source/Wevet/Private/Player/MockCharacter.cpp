// Copyright 2018 wevet works All Rights Reserved.

#include "MockCharacter.h"
#include "CharacterPickupComponent.h"
#include "Engine.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
//#include "ShaderCompiler.h"

AMockCharacter::AMockCharacter(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	GetCapsuleComponent()->InitCapsuleSize(32.f, 96.0f);

	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw   = false;
	bUseControllerRotationRoll  = false;

	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 540.0f, 0.0f);
	GetCharacterMovement()->JumpZVelocity = 570.f;
	GetCharacterMovement()->AirControl = 0.2f;

	CameraBoomComponent = ObjectInitializer.CreateDefaultSubobject<USpringArmComponent>(this, TEXT("CameraBoomComponent"));
	CameraBoomComponent->SetupAttachment(RootComponent);
	CameraBoomComponent->TargetArmLength = 240.0f;
	CameraBoomComponent->bUsePawnControlRotation = true;

	FollowCameraComponent = ObjectInitializer.CreateDefaultSubobject<UCameraComponent>(this, TEXT("FollowCameraComponent"));
	FollowCameraComponent->SetupAttachment(CameraBoomComponent, USpringArmComponent::SocketName);
	FollowCameraComponent->bUsePawnControlRotation = false;
	FollowCameraComponent->SetRelativeLocation(FVector(0.f, 70.f, 30.f));
}

void AMockCharacter::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
}

void AMockCharacter::BeginPlay()
{
	Super::BeginPlay();
}

void AMockCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	check(PlayerInputComponent);

	// basic action
	PlayerInputComponent->BindAction("Crouch", IE_Pressed,   this, &ACharacterBase::OnCrouch);
	PlayerInputComponent->BindAction("Sprint", IE_Pressed,   this, &ACharacterBase::OnSprint);
	PlayerInputComponent->BindAction("Jump",   IE_Pressed,   this, &AMockCharacter::Jump);
	PlayerInputComponent->BindAction("Jump",   IE_Released,  this, &AMockCharacter::StopJumping);
	PlayerInputComponent->BindAxis("Turn",        this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("LookUp",      this, &APawn::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("TurnRate",    this, &AMockCharacter::TurnAtRate);
	PlayerInputComponent->BindAxis("LookUpRate",  this, &AMockCharacter::LookUpAtRate);
	PlayerInputComponent->BindAxis("MoveForward", this, &AMockCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight",   this, &AMockCharacter::MoveRight);
	PlayerInputComponent->BindTouch(IE_Pressed,   this, &AMockCharacter::TouchStarted);
	PlayerInputComponent->BindTouch(IE_Released,  this, &AMockCharacter::TouchStopped);

	// interaction
	PlayerInputComponent->BindAction("ReleaseObjects", IE_Pressed, this, &AMockCharacter::ReleaseObjects);
	PlayerInputComponent->BindAction("PickupObjects",  IE_Pressed, this, &AMockCharacter::PickupObjects);

	// combat action
	PlayerInputComponent->BindAction("EquipWeapon", IE_Pressed, this, &AMockCharacter::EquipmentHandleEvent);
	PlayerInputComponent->BindAction("SwapWeapon",  IE_Pressed, this, &AMockCharacter::UpdateWeapon);
	PlayerInputComponent->BindAction("Fire", IE_Pressed,   this, &AMockCharacter::FirePressed);
	PlayerInputComponent->BindAction("Fire", IE_Released,  this, &AMockCharacter::FireReleassed);
	PlayerInputComponent->BindAction("Reload", IE_Pressed, this, &AMockCharacter::Reload);
}

void AMockCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//auto M = GShaderCompilingManager;
	//if (M == NULL)
	//{
	//	return;
	//}
	//if (M->IsCompiling())
	//{
	//	UE_LOG(LogWevetClient, Warning, TEXT("Compiling"));
	//}
	//else
	//{
	//	UE_LOG(LogWevetClient, Log, TEXT("Success"));
	//}
}

void AMockCharacter::TouchStarted(ETouchIndex::Type FingerIndex, FVector Location)
{
	Super::Jump();
}

void AMockCharacter::TouchStopped(ETouchIndex::Type FingerIndex, FVector Location)
{
	Super::StopJumping();
}

void AMockCharacter::TurnAtRate(float Rate)
{
	AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void AMockCharacter::LookUpAtRate(float Rate)
{
	AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}

void AMockCharacter::MoveForward(float Value)
{
	if (Controller && Value != 0.0f)
	{
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0.f, Rotation.Yaw, 0.f);
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		AddMovementInput(Direction, Value);
	}
}

void AMockCharacter::MoveRight(float Value)
{
	if (Controller && Value != 0.f)
	{
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0.f, Rotation.Yaw, 0.f);
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		AddMovementInput(Direction, Value);
	}
}

void AMockCharacter::ReleaseObjects()
{
	OnReleaseItemExecuter_Implementation();
}

void AMockCharacter::PickupObjects()
{
	OnPickupItemExecuter_Implementation(GetPickupComponent()->GetPickupActor());
}

void AMockCharacter::FirePressed()
{
	if (Super::SelectedWeapon)
	{
		BP_FirePressReceive();
	}
}

void AMockCharacter::FireReleassed()
{
	if (Super::SelectedWeapon)
	{
		BP_FireReleaseReceive();
	}
}

void AMockCharacter::Reload()
{
	if (Super::SelectedWeapon)
	{
		BP_ReloadReceive();
	}
}

void AMockCharacter::Jump()
{
	if (Super::bCrouch)
	{
		return;
	}
	Super::Jump();
}

void AMockCharacter::StopJumping()
{
	Super::StopJumping();
}

void AMockCharacter::OnCrouch()
{
	Super::OnCrouch();

	if (Super::bCrouch)
	{
		Super::bSprint = false;
		MovementSpeed = DefaultMaxSpeed *0.5f;
		GetCharacterMovement()->MaxWalkSpeed = MovementSpeed;
	}
}

void AMockCharacter::UpdateWeapon()
{
	if (ArrayExtension::NullOrEmpty(WeaponList))
	{
		return;
	}

	if (WeaponCurrentIndex >= WeaponList.Num() - 1) 
	{
		WeaponCurrentIndex = 0;
	}
	else 
	{
		++WeaponCurrentIndex;
	}
}

void AMockCharacter::Die_Implementation()
{
	if (Super::bDied)
	{
		return;
	}

	if (APlayerController* Controller = UGameplayStatics::GetPlayerController(this, 0))
	{
		Super::DisableInput(Controller);
	}
	Super::Die_Implementation();
}

void AMockCharacter::OnReleaseItemExecuter_Implementation()
{
	ReleaseWeapon();
	Super::OnReleaseItemExecuter_Implementation();
}

void AMockCharacter::OnPickupItemExecuter_Implementation(AActor* Actor)
{
	UWorld* const World = GetWorld();

	if (Actor == nullptr || World == nullptr)
	{
		return;
	}

	if (AWeaponBase* Weapon = Cast<AWeaponBase>(Actor))
	{
		if (Super::SameWeapon(Weapon))
		{
			UE_LOG(LogWevetClient, Warning, TEXT("SameWeapon : %s"), *(Weapon->GetName()));
			return;
		}

		FWeaponItemInfo WeaponItemInfo = Weapon->WeaponItemInfo;
		if (WeaponItemInfo.WeaponItemType == EWeaponItemType::None)
		{
			UE_LOG(LogWevetClient, Warning, TEXT("UnKnownItemType : %s"), *(Weapon->GetName()));
			return;
		}

		TSubclassOf<class AWeaponBase> WeaponClass = WeaponItemInfo.WeaponClass;
		const FTransform Transform = Super::GetMesh()->GetSocketTransform(WeaponItemInfo.UnEquipSocketName);
		AWeaponBase* const SpawningObject = World->SpawnActorDeferred<AWeaponBase>(
			WeaponClass,
			Transform,
			nullptr,
			nullptr,
			ESpawnActorCollisionHandlingMethod::AlwaysSpawn);

		SpawningObject->CopyWeaponItemInfo(WeaponItemInfo);
		SpawningObject->FinishSpawning(Transform);
		SpawningObject->AttachToComponent(
			Super::GetMesh(),
			{ EAttachmentRule::SnapToTarget, true },
			WeaponItemInfo.UnEquipSocketName);
		SpawningObject->Take(this);

		if (Super::WeaponList.Find(SpawningObject) == INDEX_NONE)
		{
			Super::WeaponList.Emplace(SpawningObject);
		}
		Weapon->Release(nullptr);
		Actor  = nullptr;
	}

	if (AItemBase* Item = Cast<AItemBase>(Actor))
	{
		const EItemType ItemType = Item->GetItemType();
		switch (ItemType)
		{
		case EItemType::Weapon:
			{
				const FWeaponItemInfo ItemInfo = Item->WeaponItemInfo;
				if (auto Weapon = Super::FindByWeapon(ItemInfo.WeaponItemType))
				{
					Weapon->Recover(ItemInfo);
					Item->TakeResult();
					Actor = nullptr;
				}
			}
			break;
		case EItemType::Health:
			// @TODO
			break;
		}
	}
	Super::OnPickupItemExecuter_Implementation(Actor);
}

void AMockCharacter::OnTakeDamage_Implementation(FName BoneName, float Damage, AActor* Actor)
{
	Super::OnTakeDamage_Implementation(BoneName, Damage, Actor);
	if (ICombatExecuter::Execute_IsDeath(this))
	{
		Die_Implementation();
	}
}

void AMockCharacter::Equipment_Implementation()
{
	SelectedWeapon = WeaponList[WeaponCurrentIndex];
	check(SelectedWeapon);
	Super::Equipment_Implementation();
	SelectedWeapon->AttachToComponent(
		Super::GetMesh(),
		{ EAttachmentRule::SnapToTarget, true },
		Super::SelectedWeapon->WeaponItemInfo.EquipSocketName);
	GetCharacterMovement()->bOrientRotationToMovement = false;
	Super::bUseControllerRotationYaw = true;
}

void AMockCharacter::UnEquipment_Implementation()
{
	check(SelectedWeapon);
	Super::UnEquipment_Implementation();
	SelectedWeapon->AttachToComponent(
		Super::GetMesh(),
		{ EAttachmentRule::SnapToTarget, true },
		SelectedWeapon->WeaponItemInfo.UnEquipSocketName);
	GetCharacterMovement()->bOrientRotationToMovement = true;
	Super::bUseControllerRotationYaw = false;
	SelectedWeapon = nullptr;
}

FVector AMockCharacter::BulletTraceRelativeLocation() const
{
	return GetFollowCameraComponent()->GetComponentLocation();
}

FVector AMockCharacter::BulletTraceForwardLocation() const
{
	return GetFollowCameraComponent()->GetForwardVector();
}

//����,�񑕒��̔���
void AMockCharacter::EquipmentHandleEvent()
{
	if (SelectedWeapon)
	{
		Super::UnEquipmentActionMontage();
	}
	else
	{
		Super::EquipmentActionMontage();
	}
}

void AMockCharacter::ReleaseWeapon()
{
	UWorld* const World = GetWorld();

	if (World == nullptr)
	{
		return;
	}

	const FRotator Rotation = Super::GetActorRotation();
	const FVector Forward   = Super::GetActorLocation() + (Controller->GetControlRotation().Vector() * 200);
	const FTransform Transform  = UKismetMathLibrary::MakeTransform(Forward, Rotation, FVector::OneVector);
	
	if (AWeaponBase* Weapon = Super::GetUnEquipWeapon())
	{
		if (WeaponList.Find(Weapon) <= 0)
		{
			WeaponList.Remove(Weapon);
		}
		Super::ReleaseWeaponToWorld(Transform, Weapon);
	}
}
