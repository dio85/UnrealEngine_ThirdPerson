// Copyright 2018 wevet works All Rights Reserved.


#include "Player/MockPlayerCameraManager.h"
#include "Character/CharacterBase.h"


AMockPlayerCameraManager::AMockPlayerCameraManager(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	CameraBehavior = ObjectInitializer.CreateDefaultSubobject<USkeletalMeshComponent>(this, TEXT("CameraBehavior"));
	CameraBehavior->SetupAttachment(GetTransformComponent());
	CameraBehavior->SetHiddenInGame(true);

	ViewPitchMin = -50.f;
	ViewPitchMax = 50.f;

	RootLocation = FVector::ZeroVector;
}


void AMockPlayerCameraManager::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	ControlledPawn = nullptr;
	Super::EndPlay(EndPlayReason);
}


void AMockPlayerCameraManager::BeginPlay()
{
	Super::BeginPlay();
}


void AMockPlayerCameraManager::OnPossess(APawn* InPawn)
{
	ControlledPawn = InPawn;
	BP_OnPossess();
	UE_LOG(LogWevetClient, Log, TEXT("Possess : %s, funcName : %s"), *ControlledPawn->GetName(), *FString(__FUNCTION__));
}


void AMockPlayerCameraManager::SetLimitPitch(const float InViewPitchMin, const float InViewPitchMax)
{
	//ViewPitchMin = InViewPitchMin;
	//ViewPitchMax = InViewPitchMax;
}


EDrawDebugTrace::Type AMockPlayerCameraManager::GetDrawDebugTrace() const
{
	if (ControlledPawn)
	{
		if (ILocomotionSystemPawn::Execute_HasDebugTrace(ControlledPawn))
		{
			return EDrawDebugTrace::Type::ForOneFrame;
		}
	}
	return EDrawDebugTrace::Type::None;
}


