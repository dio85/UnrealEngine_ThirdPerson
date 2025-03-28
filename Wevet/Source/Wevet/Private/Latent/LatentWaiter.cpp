// Copyright 2018 wevet works All Rights Reserved.

#include "Latent/LatentWaiter.h"


FImplementableLatentAction::FImplementableLatentAction(UWorld* InWorld, const FLatentActionInfo& LatentInfo)
	: Latenter(InWorld->SpawnActor<ALatenterActor>()),
	ExecutionFunction(LatentInfo.ExecutionFunction),
	OutputLink(LatentInfo.Linkage),
	CallbackTarget(LatentInfo.CallbackTarget)
{
}


void FImplementableLatentAction::UpdateOperation(FLatentResponse& Response)
{
	const bool bWasCondition = !(Latenter->IsValidLowLevel() && !Latenter->IsActorBeingDestroyed());
	Response.FinishAndTriggerIf(
		bWasCondition, 
		ExecutionFunction,
		OutputLink, 
		CallbackTarget);
}


ALatenterActor::ALatenterActor()
{
	PrimaryActorTick.bCanEverTick = false;
	SetActorHiddenInGame(true);
}


void ALatenterActor::FinishWait()
{
	Destroy();
}


void UBlueprintLatenterFunctionLibrary::WaitUntilFinished(const UObject* WorldContextObject, FLatentActionInfo LatentInfo, const FWaitUntilFinishedDispatcher OnEventDispatcher)
{
	UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::ReturnNull);
	if (World)
	{
		FLatentActionManager& LatentActionManager = World->GetLatentActionManager();
		auto OldLatentAction = LatentActionManager.FindExistingAction<FImplementableLatentAction>(LatentInfo.CallbackTarget, LatentInfo.UUID);
		if (OldLatentAction == nullptr)
		{
			FImplementableLatentAction* LatentAction = new FImplementableLatentAction(World, LatentInfo);
			LatentActionManager.AddNewAction(LatentInfo.CallbackTarget, LatentInfo.UUID, LatentAction);
			OnEventDispatcher.ExecuteIfBound(LatentAction->Latenter);
		}
	}
}

