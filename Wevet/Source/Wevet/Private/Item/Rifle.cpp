// Copyright 2018 wevet works All Rights Reserved.

#include "Item/Rifle.h"
#include "WevetExtension.h"

ARifle::ARifle(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	BulletDuration = 0.15f;
	CollisionComponent->SetBoxExtent(FVector(16.f, 80.f, 32.f));

	{
		static ConstructorHelpers::FObjectFinder<USkeletalMesh> FindAsset(TEXT("/Game/Assets/MilitaryWeapon/Weapons/Assault_Rifle_A"));
		USkeletalMesh* Asset = FindAsset.Object;
		SkeletalMeshComponent->SetSkeletalMesh(Asset);
	}

	{
		static ConstructorHelpers::FObjectFinder<USoundBase> FindAsset(TEXT("/Game/Assets/MilitaryWeapon/Sound/Rifle/Cues/RifleA_Fire_Cue"));
		FireSound = FindAsset.Object;
	}

	{
		static ConstructorHelpers::FObjectFinder<USoundBase> FindAsset(TEXT("/Game/Assets/MilitaryWeapon/Sound/Rifle/Cues/Rifle_ImpactSurface_Cue"));
		ImpactSound = FindAsset.Object;
	}

	{
		static ConstructorHelpers::FObjectFinder<UParticleSystem> FindAsset(TEXT("/Game/Assets/MilitaryWeapon/FX/P_AssaultRifle_MuzzleFlash"));
		FlashEmitterTemplate = FindAsset.Object;
	}

	{
		static ConstructorHelpers::FObjectFinder<UParticleSystem> FindAsset(TEXT("/Game/Assets/MilitaryWeapon/FX/P_Impact_Metal_Medium_01"));
		ImpactEmitterTemplate = FindAsset.Object;
	}

	{
		static ConstructorHelpers::FObjectFinder<UClass> FindAsset(Wevet::ProjectFile::GetRifleBulletPath());
		BulletsTemplate = FindAsset.Object;
	}

	// AK47
	WeaponItemInfo.NeededAmmo = 50;
	WeaponItemInfo.ClipType = 50;
	WeaponItemInfo.CurrentAmmo = 50;
	WeaponItemInfo.MaxAmmo = 400;
	WeaponItemInfo.Damage = 1500.f;
	WeaponItemInfo.EquipSocketName = FName(TEXT("RifleEquip_Socket"));
	WeaponItemInfo.UnEquipSocketName = FName(TEXT("Rifle_Socket"));
	WeaponItemInfo.WeaponItemType = EWeaponItemType::Rifle;
	WeaponItemInfo.DisplayName = FString(TEXT("FN SCAR"));
	WeaponItemInfo.MeleeDistance = 2000.f;
	WeaponItemInfo.HearingRange = 5000.f;
}

