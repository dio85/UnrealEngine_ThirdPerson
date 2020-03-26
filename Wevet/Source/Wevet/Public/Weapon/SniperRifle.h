// Copyright © 2018 wevet works All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Weapon/AbstractWeapon.h"
#include "SniperRifle.generated.h"

/**
 * 
 */
UCLASS()
class WEVET_API ASniperRifle : public AAbstractWeapon
{
	GENERATED_BODY()

public:
	ASniperRifle(const FObjectInitializer& ObjectInitializer);
};
