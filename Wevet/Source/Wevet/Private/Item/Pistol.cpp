// Copyright 2018 wevet works All Rights Reserved.

#include "Item/Pistol.h"
#include "WevetExtension.h"

APistol::APistol(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	BulletDuration = 0.15f;
	CollisionComponent->SetBoxExtent(FVector(10.f, 24.f, 20.f));

	{
		static ConstructorHelpers::FObjectFinder<USkeletalMesh> FindAsset(TEXT("/Game/Assets/MilitaryWeapon/Weapons/Pistols_A"));
		USkeletalMesh* Asset = FindAsset.Object;
		SkeletalMeshComponent->SetSkeletalMesh(Asset);
	}

	{
		static ConstructorHelpers::FObjectFinder<USoundBase> FindAsset(TEXT("/Game/Assets/MilitaryWeapon/Sound/Pistol/Cues/PistolA_Fire_Cue"));
		FireSound = FindAsset.Object;
	}

	{
		static ConstructorHelpers::FObjectFinder<USoundBase> FindAsset(TEXT("/Game/Assets/MilitaryWeapon/Sound/Pistol/Cues/Pistol_Whip_Cue"));
		ImpactSound = FindAsset.Object;
	}

	{
		static ConstructorHelpers::FObjectFinder<UParticleSystem> FindAsset(TEXT("/Game/Assets/MilitaryWeapon/FX/P_Pistol_MuzzleFlash_01"));
		FlashEmitterTemplate = FindAsset.Object;
	}

	{
		static ConstructorHelpers::FObjectFinder<UParticleSystem> FindAsset(TEXT("/Game/Assets/MilitaryWeapon/FX/P_Impact_Metal_Small_01"));
		ImpactEmitterTemplate = FindAsset.Object;
	}

	{
		static ConstructorHelpers::FObjectFinder<UClass> FindAsset(Wevet::ProjectFile::GetPistolBulletPath());
		BulletsTemplate = FindAsset.Object;
	}

	// Colt Government
	WeaponItemInfo.NeededAmmo = 8;
	WeaponItemInfo.ClipType = 20;
	WeaponItemInfo.CurrentAmmo = 20;
	WeaponItemInfo.MaxAmmo = 200;
	WeaponItemInfo.Damage = 1000.f;
	WeaponItemInfo.EquipSocketName = FName(TEXT("PistolEquip_Socket"));
	WeaponItemInfo.UnEquipSocketName = FName(TEXT("Pistol_Socket"));
	WeaponItemInfo.WeaponItemType = EWeaponItemType::Pistol;
	WeaponItemInfo.DisplayName = FString(TEXT("Colt Government"));
	WeaponItemInfo.MeleeDistance = 1000.f;
	WeaponItemInfo.HearingRange = 1400.f;
}

