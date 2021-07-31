// Copyright 2018 wevet works All Rights Reserved.

#include "Latent/LatentWaiter.h"


ALatenterActor::ALatenterActor()
{
	PrimaryActorTick.bCanEverTick = false;
	SetActorHiddenInGame(true);
}


void ALatenterActor::FinishWait()
{
	Destroy();
}

