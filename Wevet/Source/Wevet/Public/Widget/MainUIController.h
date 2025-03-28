// Copyright 2018 wevet works All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Player/MockCharacter.h"
#include "Widget/PlayerHealth.h"
#include "Widget/WeaponFocus.h"
#include "Widget/WeaponWindow.h"
#include "Components/UniformGridPanel.h"
#include "Components/UniformGridSlot.h"
#include "Components/CanvasPanel.h"
#include "MainUIController.generated.h"


UCLASS()
class WEVET_API UMainUIController : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UMainUIController(const FObjectInitializer& ObjectInitializer);
	virtual void NativeConstruct() override;

public:
	void Initializer(ACharacterBase* const NewCharacter);
	void Renderer(float InDeltaTime);
	void SetTickableWhenPaused();

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "MainUI|Variable")
	class AMockCharacter* CharacterOwner;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "MainUI|Variable")
	FName PlayerHealthKeyName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "MainUI|Variable")
	FName WeaponFocusKeyName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "MainUI|Variable")
	FName BasePanelKeyName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "MainUI|Variable")
	FName WeaponWindowKeyName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "MainUI|Variable")
	TSubclassOf<class UWeaponWindow> WindowTemplate;

	class UPlayerHealth* PlayerHealth;
	class UWeaponFocus* WeaponFocus;
	class UWeaponWindow* WeaponWindow;
	class UCanvasPanel* BasePanel;

protected:
	void VisibleWeaponFocus();
	bool bWasEquipWeapon;
	int32 PrevWeaponIndex;

protected:
	UFUNCTION(BlueprintCallable, Category="MainUI|Function")
	void CreateWeaponWindow(UUniformGridPanel* GridPanel);

protected:
	UPROPERTY(EditDefaultsOnly, Category = "Tick", AdvancedDisplay)
	bool bTickEventWhenPaused;
};
