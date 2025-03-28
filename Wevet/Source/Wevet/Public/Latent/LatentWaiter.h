// Copyright 2018 wevet works All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "LatentActions.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "LatentWaiter.generated.h"

DECLARE_DYNAMIC_DELEGATE_OneParam(FWaitUntilFinishedDispatcher, ALatenterActor*, Latenter);


UCLASS()
class WEVET_API ALatenterActor : public AActor
{
	GENERATED_BODY()
	
	
public:
	ALatenterActor();

	UFUNCTION(BlueprintCallable, Category = LatentWaiter)
	void FinishWait();


};


class FImplementableLatentAction : public FPendingLatentAction
{
public:
	class ALatenterActor* Latenter;

	FName ExecutionFunction;
	int32 OutputLink;
	FWeakObjectPtr CallbackTarget;

	FImplementableLatentAction(UWorld* InWorld, const FLatentActionInfo& LatentInfo);
	virtual void UpdateOperation(FLatentResponse& Response) override;
};


UCLASS()
class WEVET_API UBlueprintLatenterFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = Latenter, meta = (Latent, LatentInfo = "LatentInfo", HidePin = "WorldContextObject", DefaultToSelf = "WorldContextObject"))
	static void WaitUntilFinished(const UObject* WorldContextObject, FLatentActionInfo LatentInfo, const FWaitUntilFinishedDispatcher OnEventDispatcher);
};
