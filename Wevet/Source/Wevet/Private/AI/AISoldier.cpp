// Copyright © 2018 wevet works All Rights Reserved.

#include "AISoldier.h"
#include "Player/MockCharacter.h"
#include "AIControllerBase.h"
#include "CharacterModel.h"

AAISoldier::AAISoldier(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer),
	SenceWarningTimeOut(10.f)
{
	//
}

void AAISoldier::MainLoop(float DeltaTime)
{
	UWorld* const World = GetWorld();
	if (World == nullptr)
	{
		return;
	}

	const float TimeSeconds = World->TimeSeconds;
	if (bSeeTarget && (TimeSeconds - LastSeenTime) > SenseTimeOut)
	{
		bSeeTarget = false;
		SetSeeTargetActor(nullptr);
	}

	if (bHearTarget && (TimeSeconds - LastHeardTime) > SenseTimeOut)
	{
		bHearTarget = false;
		SetHearTargetActor(nullptr);
	}

	if (bCrouch && (TimeSeconds - LastWarningTime) > SenceWarningTimeOut)
	{
		OnCrouch();
	}

	// only see target
	if (CanMeleeStrike_Implementation())
	{
		if (!Super::HasEquipWeapon())
		{
			return;
		}
		if (!IDamageInstigator::Execute_IsDeath(TargetCharacter))
		{
			BulletInterval += DeltaTime;
			if (BulletInterval >= BulletDelay)
			{
				Super::FirePressed();
				BulletInterval = 0.f;
			}
		}
		else
		{
			bSeeTarget = false;
			SetSeeTargetActor(nullptr);
		}
	}
	else
	{
		// not found
	}
}

void AAISoldier::OnSeePawnRecieve(APawn* OtherPawn)
{
	UWorld* const World = GetWorld();

	if (World == nullptr)
	{
		return;
	}

	AMockCharacter* Character = Cast<AMockCharacter>(OtherPawn);
	if (Character && !IDamageInstigator::Execute_IsDeath(Character))
	{
		LastSeenTime = World->GetTimeSeconds();
		if (!bSeeTarget)
		{
			bSeeTarget = true;
			Super::SetSeeTargetActor(Character);
		}
	}
}

void AAISoldier::OnHearNoiseRecieve(APawn* OtherActor, const FVector& Location, float Volume)
{
	UWorld* const World = GetWorld();
	if (World == nullptr)
	{
		return;
	}

	LastHeardTime = World->GetTimeSeconds();
	if (!bHearTarget)
	{
		bHearTarget = true;
		Super::SetHearTargetActor(OtherActor);
	}
}

