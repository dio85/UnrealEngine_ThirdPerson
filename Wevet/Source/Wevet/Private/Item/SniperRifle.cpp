// Copyright 2018 wevet works All Rights Reserved.

#include "Item/SniperRifle.h"
#include "WevetExtension.h"

ASniperRifle::ASniperRifle(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	BulletDuration = 0.6f;
	CollisionComponent->SetBoxExtent(FVector(16.f, 80.f, 32.f));

	{
		static ConstructorHelpers::FObjectFinder<USkeletalMesh> FindAsset(TEXT("/Game/Assets/MilitaryWeapon/Weapons/Sniper_Rifle_A"));
		USkeletalMesh* Asset = FindAsset.Object;
		SkeletalMeshComponent->SetSkeletalMesh(Asset);
	}

	{
		static ConstructorHelpers::FObjectFinder<USoundBase> FindAsset(TEXT("/Game/Assets/MilitaryWeapon/Sound/SniperRifle/Cues/SniperRifleA_Fire_Cue"));
		FireSound = FindAsset.Object;
	}

	{
		static ConstructorHelpers::FObjectFinder<USoundBase> FindAsset(TEXT("/Game/Assets/MilitaryWeapon/Sound/SniperRifle/Cues/SniperRifle_ImpactSurface_Cue"));
		ImpactSound = FindAsset.Object;
	}

	{
		static ConstructorHelpers::FObjectFinder<UParticleSystem> FindAsset(TEXT("/Game/Assets/MilitaryWeapon/FX/P_SniperRifle_MuzzleFlash_01"));
		FlashEmitterTemplate = FindAsset.Object;
	}

	{
		static ConstructorHelpers::FObjectFinder<UParticleSystem> FindAsset(TEXT("/Game/Assets/MilitaryWeapon/FX/P_Impact_Metal_Large_01"));
		ImpactEmitterTemplate = FindAsset.Object;
	}

	{
		static ConstructorHelpers::FObjectFinder<UClass> FindAsset(Wevet::ProjectFile::GetSniperBulletPath());
		BulletsTemplate = FindAsset.Object;
	}

	// SVD
	WeaponItemInfo.NeededAmmo = 2;
	WeaponItemInfo.ClipType = 10;
	WeaponItemInfo.CurrentAmmo = 10;
	WeaponItemInfo.MaxAmmo = 50;
	WeaponItemInfo.Damage = 2000.f;
	WeaponItemInfo.EquipSocketName = FName(TEXT("RifleEquip_Socket"));
	WeaponItemInfo.UnEquipSocketName = FName(TEXT("Sniper_Rifle_Socket"));
	WeaponItemInfo.WeaponItemType = EWeaponItemType::Sniper;
	WeaponItemInfo.DisplayName = FString(TEXT("SVD"));
	WeaponItemInfo.MeleeDistance = 3000.f;
	WeaponItemInfo.HearingRange = 10000.f;
}
