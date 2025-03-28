// Copyright 2018 wevet works All Rights Reserved.


#include "Singleton/BattleGameManager.h"
#include "Lib/SingletonFunctionLibrary.h"

ABattleGameManager* ABattleGameManager::Instance = nullptr;

ABattleGameManager::ABattleGameManager(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = false;
	BattlePhase = Wevet::BattlePhase::Normal;
}


void ABattleGameManager::BeginPlay()
{
	Super::BeginPlay();
}


void ABattleGameManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}


ABattleGameManager* ABattleGameManager::GetInstance()
{
	return USingletonFunctionLibrary::GetSingletonInstance<ABattleGameManager>(Instance);
}


void ABattleGameManager::SetBattlePhase(const Wevet::BattlePhase InBattlePhase)
{
	if (BattlePhase == InBattlePhase)
	{
		return;
	}

	BattlePhase = InBattlePhase;

#if WITH_EDITOR
	FString Phase;
	switch (BattlePhase)
	{
		case Wevet::BattlePhase::Normal:
		Phase.Append("Normal");
		break;
		case Wevet::BattlePhase::Warning:
		Phase.Append("Warning");
		break;
		case Wevet::BattlePhase::Alert:
		Phase.Append("Alert");
		break;
	}
	UE_LOG(LogWevetClient, Log, TEXT("Phase : %s, funcName : %s"), *Phase, *FString(__FUNCTION__));
#endif

}

