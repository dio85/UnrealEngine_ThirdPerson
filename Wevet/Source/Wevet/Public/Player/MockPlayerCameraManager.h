// Copyright 2018 wevet works All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Camera/PlayerCameraManager.h"
#include "Components/SkeletalMeshComponent.h"
#include "MockPlayerCameraManager.generated.h"

/**
 * 
 */
UCLASS()
class WEVET_API AMockPlayerCameraManager : public APlayerCameraManager
{
	GENERATED_BODY()
	
public:
	AMockPlayerCameraManager(const FObjectInitializer& ObjectInitializer);
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

protected:
	virtual void BeginPlay() override;


protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "PlayerCameraManager|Variable")
	class APawn* ControlledPawn;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "PlayerCameraManager|Variable")
	FVector RootLocation;


protected:
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "PlayerCameraManager|Event")
	void BP_OnPossess();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Components, meta = (AllowPrivateAccess = "true"))
	class USkeletalMeshComponent* CameraBehavior;

	UFUNCTION(BlueprintCallable, Category = "PlayerCameraManager|Function")
	EDrawDebugTrace::Type GetDrawDebugTrace() const;

public:
	FORCEINLINE class USkeletalMeshComponent* GetCameraBehavior() const
	{
		return CameraBehavior;
	}

public:
	void OnPossess(APawn* InPawn);

	void SetLimitPitch(const float InViewPitchMin, const float InViewPitchMax);
};
