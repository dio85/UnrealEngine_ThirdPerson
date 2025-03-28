// Copyright 2018 wevet works All Rights Reserved.


#include "AnimInstance/PlayerAnimInstance.h"
#include "Kismet/KismetMathLibrary.h"

UPlayerAnimInstance::UPlayerAnimInstance(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	{
		const wchar_t* Path = TEXT("/Game/Player_Assets/Ellie/Animations/ALS/Mantle/ALS_N_Mantle_1m_RH_Montage");
		static ConstructorHelpers::FObjectFinder<UAnimMontage> FindAsset(Path);
		DefaultLowMantleAsset.AnimMontage = FindAsset.Object;
	}

	{
		const wchar_t* Path = TEXT("/Game/Player_Assets/Ellie/Animations/ALS/Mantle/ALS_N_Mantle_2m_Montage");
		static ConstructorHelpers::FObjectFinder<UAnimMontage> FindAsset(Path);
		DefaultHighMantleAsset.AnimMontage = FindAsset.Object;
	}

}


void UPlayerAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();
	Player = Cast<AMockCharacter>(Owner);
}


void UPlayerAnimInstance::NativeUpdateAnimation(float DeltaTimeX)
{
	Super::NativeUpdateAnimation(DeltaTimeX);
}

