// Copyright © 2018 wevet works All Rights Reserved.

#include "MainLevelScriptActor.h"
#include "MockCharacter.h"
#include "Engine.h"


AMainLevelScriptActor::AMainLevelScriptActor(const class FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{

}

void AMainLevelScriptActor::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
}

void AMainLevelScriptActor::BeginPlay()
{
	Super::BeginPlay();

	UWorld* const World = GetWorld();

	if (World == nullptr)
	{
		return;
	}
	TArray<AActor*> FoundActors;
	UGameplayStatics::GetAllActorsOfClass(World, ACharacterBase::StaticClass(), FoundActors);

	for (AActor*& Actor : FoundActors)
	{
		if (ACharacterBase* Character = Cast<ACharacterBase>(Actor))
		{
			CharacterArray.Add(Character);
		}
	}

	UE_LOG(LogWevetClient, Warning, TEXT("StartPlay \n CharacterNum : [%d] \n funcName : [%s]"), CharacterArray.Num(), *FString(__FUNCTION__));
}
