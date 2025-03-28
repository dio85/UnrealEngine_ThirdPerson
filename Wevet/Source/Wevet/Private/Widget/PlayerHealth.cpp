// Copyright 2018 wevet works All Rights Reserved.

#include "Widget/PlayerHealth.h"

UPlayerHealth::UPlayerHealth(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	SourceImageKeyName = FName(TEXT("SourceImage"));
	MaterialParamKeyName = FName(TEXT("Progress"));
	InterpSpeed = 4.0f;
}

void UPlayerHealth::NativeConstruct()
{
	Super::NativeConstruct();
	SourceImage = Cast<UImage>(GetWidgetFromName(SourceImageKeyName));
}

void UPlayerHealth::Initializer(ACharacterBase* const NewCharacter)
{
	CharacterPtr = MakeWeakObjectPtr<ACharacterBase>(NewCharacter);
}

void UPlayerHealth::Renderer(float InDeltaTime)
{
	if (!SourceImage || !CharacterPtr.IsValid())
	{
		return;
	}

	const float Health = ICombatInstigator::Execute_IsDeath(CharacterPtr.Get()) ? 0.0f : (CharacterPtr->GetHealthToWidget());
	if (!FMath::IsNearlyEqual(Health, CurrentHealth))
	{
		CurrentHealth = Health;

		if (MaterialInstance == nullptr)
		{
			MaterialInstance = SourceImage->GetDynamicMaterial();
		}

		const float Value = FMath::FInterpTo(Health, CurrentHealth, InDeltaTime, InterpSpeed);
		MaterialInstance->SetScalarParameterValue(MaterialParamKeyName, Value);
		const FSlateBrush Brush = SourceImage->GetBrush();
		SourceImage->SetBrush(Brush);
	}

}

