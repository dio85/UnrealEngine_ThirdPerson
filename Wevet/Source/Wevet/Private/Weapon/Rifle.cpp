// Copyright 2018 wevet works All Rights Reserved.

#include "Weapon/Rifle.h"
#include "Weapon/BulletBase.h"
#include "Engine.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "WevetExtension.h"
#include "Character/CharacterBase.h"

ARifle::ARifle(const FObjectInitializer& ObjectInitializer) 
	: Super(ObjectInitializer)
{
	BulletDuration = 0.15f;
	PrimaryActorTick.bCanEverTick = true;

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
		static ConstructorHelpers::FObjectFinder<UClass> FindAsset(TEXT("/Game/Game/Blueprints/Bullet/BP_Rifle_Bullet.BP_Rifle_Bullet_C"));
		BulletsTemplate = FindAsset.Object;
	}

	NeededAmmo = 50;
	WeaponItemInfo.UnEquipSocketName = FName(TEXT("Spine_Right_Socket"));
}

