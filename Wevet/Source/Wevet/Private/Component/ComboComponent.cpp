// Copyright 2018 wevet works All Rights Reserved.


#include "Component/ComboComponent.h"
#include "Wevet.h"

UComboComponent::UComboComponent(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	PrimaryComponentTick.bCanEverTick = true;
	bEnableComboPeroid = false;
	bAnyComboActionKeyPressed = false;
	ChainComboIndex = INT_ZERO;
	PrevChainComboIndex = INDEX_NONE;
}

void UComboComponent::BeginPlay()
{
	Super::BeginPlay();
	Super::SetComponentTickEnabled(false);
}

void UComboComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UComboComponent::ResetChainCombo()
{
	ChainComboIndex = INT_ZERO;
	PrevChainComboIndex = INDEX_NONE;
}

void UComboComponent::InitChainCombo()
{
	ResetChainCombo();
	SetAnyComboActionKeyPressed(false);
	SetComboPeroid(false);
}

void UComboComponent::OnChainStrikePressed(bool& OutAnyKeyPressedSuccess)
{
	if (!bAnyComboActionKeyPressed && bEnableComboPeroid)
	{
		SetAnyComboActionKeyPressed(true);
		OutAnyKeyPressedSuccess = true;
	}
	else
	{
		OutAnyKeyPressedSuccess = false;
	}
}

void UComboComponent::ValidChainCombo(const int32 ArrayNum)
{
	const int32 LastIndex = (ArrayNum - 1);
	if (ChainComboIndex > LastIndex)
	{
		ResetChainCombo();
	}
}

bool UComboComponent::CanNextChain() const
{
	return ChainComboIndex != PrevChainComboIndex;
}

bool UComboComponent::IsValidChainComboIndex() const
{
	return (ChainComboIndex <= INT_ZERO);
}

int32 UComboComponent::GetChainComboIndex() const
{
	return ChainComboIndex;
}

void UComboComponent::UpdateChainComboIndex()
{
	PrevChainComboIndex = ChainComboIndex;
}

void UComboComponent::SetComboPeroid(const bool NewEnableComboPeroid)
{
	bEnableComboPeroid = NewEnableComboPeroid;
}

bool UComboComponent::GetComboPeroid() const
{
	return bEnableComboPeroid;
}

void UComboComponent::SetAnyComboActionKeyPressed(const bool NewAnyComboActionKeyPressed)
{
	bAnyComboActionKeyPressed = NewAnyComboActionKeyPressed;
}

bool UComboComponent::GetAnyComboActionKeyPressed() const
{
	return bAnyComboActionKeyPressed;
}

void UComboComponent::HandledChainComboUpdateIndex(const bool bWasMoving, const int32 UpperComboNum, const int32 FullBodyComboNum)
{
	const int32 LastIndex = (bWasMoving) ? (UpperComboNum - 1) : (FullBodyComboNum - 1);

	if (ChainComboIndex > LastIndex)
	{
		ResetChainCombo();
	}
	else
	{
		++ChainComboIndex;
	}
}

void UComboComponent::DrawLog()
{
	UE_LOG(LogTemp, Warning, TEXT("Combo Index => %d Prev Combo Index => %d"), ChainComboIndex, PrevChainComboIndex);
}
