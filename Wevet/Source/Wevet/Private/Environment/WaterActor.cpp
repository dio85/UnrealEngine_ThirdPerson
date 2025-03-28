// Copyright © 2018 wevet works All Rights Reserved.


#include "Environment/WaterActor.h"
#include "Character/CharacterBase.h"
#include "Kismet/KismetSystemLibrary.h"

AWaterActor::AWaterActor()
{
	PrimaryActorTick.bCanEverTick = true;
	bInWater = false;
	WaterType = EWaterType::None;
	HitPosition = FVector::ZeroVector;

	Tags.Add(FName(TEXT("WaterBody")));
}

void AWaterActor::BeginPlay()
{
	Super::BeginPlay();
}

void AWaterActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (Target)
	{
		const FVector HeadPosition = Target->GetHeadSocketLocation();
		const FVector ChestPosition = Target->GetChestSocketLocation();
		const bool bChestLess = (CurrentHitData.ImpactPoint.Z <= ChestPosition.Z);
		const bool bHeadLess = (CurrentHitData.ImpactPoint.Z <= HeadPosition.Z);

		if (bChestLess)
		{
			WaterType = EWaterType::Shallow;
		}
		else if (bHeadLess)
		{
			WaterType = EWaterType::Deep;
		}

		switch (WaterType)
		{
		case EWaterType::Shallow:
			//UKismetSystemLibrary::DrawDebugSphere(GetWorld(), ChestPosition, 20.f, 12, FLinearColor::Blue, 2.0f, 2.0f);
			break;

		case EWaterType::Deep:
			//UKismetSystemLibrary::DrawDebugSphere(GetWorld(), HeadPosition, 20.f, 12, FLinearColor::Red, 2.0f, 2.0f);
			break;
		}

		//HitPosition = CurrentHitData.ImpactPoint;
		//HitPosition.X = Target->GetActorLocation().X;
		//HitPosition.Y = Target->GetActorLocation().Y;
		//UKismetSystemLibrary::DrawDebugSphere(GetWorld(), HitPosition, 20.f, 12, FLinearColor::Green, 2.0f, 2.0f);
		UE_LOG(LogWevetClient, Log, TEXT("ImpactPoint : %s"), *CurrentHitData.ImpactPoint.ToString());
		UE_LOG(LogWevetClient, Log, TEXT("HeadPosition : %s"), *HeadPosition.ToString());
		UE_LOG(LogWevetClient, Log, TEXT("ChestPosition : %s"), *ChestPosition.ToString());
	}
}

void AWaterActor::SetHitData(const FHitResult& HitResult)
{
	CurrentHitData = HitResult;
}

void AWaterActor::ForceUnEquip(class ACharacterBase* InTarget)
{
	if (InTarget)
	{
		//InTarget->UnEquipmentActionMontage();
	}
}

