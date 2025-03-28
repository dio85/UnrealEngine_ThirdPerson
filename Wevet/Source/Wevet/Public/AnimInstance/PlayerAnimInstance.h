// Copyright 2018 wevet works All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "AnimInstance/CharacterAnimInstanceBase.h"
#include "Player/MockCharacter.h"
#include "PlayerAnimInstance.generated.h"

/**
 * 
 */
UCLASS()
class WEVET_API UPlayerAnimInstance : public UCharacterAnimInstanceBase
{
	GENERATED_BODY()
	
public:
	UPlayerAnimInstance(const FObjectInitializer& ObjectInitializer);
	virtual void NativeInitializeAnimation() override;
	virtual void NativeUpdateAnimation(float DeltaTimeX) override;


protected:
	class AMockCharacter* Player;

};

