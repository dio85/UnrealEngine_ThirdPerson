// Copyright 2018 wevet works All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "LatentWaiter.generated.h"


UCLASS()
class WEVET_API ALatenterActor : public AActor
{
	GENERATED_BODY()
	
	
public:
	ALatenterActor();

	UFUNCTION(BlueprintCallable, Category = LatentWaiter)
	void FinishWait();


};
