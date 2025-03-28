// Copyright 2018 wevet works All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "Camera/CameraShakeBase.h"
#include "LocomotionSystemTypes.h"
#include "LocomotionSystemStructs.h"
#include "LocomotionSystemPawn.generated.h"

UINTERFACE(BlueprintType)
class ULocomotionSystemPawn : public UInterface
{
	GENERATED_UINTERFACE_BODY()
};

class LOCOMOTIONSYSTEM_API ILocomotionSystemPawn
{
	GENERATED_IINTERFACE_BODY()

public:
	// Apply Editor TurnInPlace_AnimNotifyState
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "ALS_Pawn")
	void SetALSCharacterRotation(const FRotator AddAmount);

	// Apply Player Only
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "ALS_Pawn")
	void SetALSCameraShake(TSubclassOf<class UCameraShakeBase> InShakeClass, const float InScale);

	// MovementMode
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "ALS_Pawn")
	void SetALSMovementMode(const ELSMovementMode NewALSMovementMode);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "ALS_Pawn")
	void OnALSMovementModeChange();

	// MovementAction
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "ALS_Pawn")
	void SetALSMovementAction(const ELSMovementAction NewALSMovementAction);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "ALS_Pawn")
	void OnALSMovementActionChange();

	// Gait
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "ALS_Pawn")
	void SetALSGait(const ELSGait NewALSGait);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "ALS_Pawn")
	void OnALSGaitChange();

	// Stance
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "ALS_Pawn")
	void SetALSStance(const ELSStance NewALSStance);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "ALS_Pawn")
	void OnALSStanceChange();

	// RotationMode
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "ALS_Pawn")
	void SetALSRotationMode(const ELSRotationMode NewALSRotationMode);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "ALS_Pawn")
	void OnALSRotationModeChange();

	// ViewMode
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "ALS_Pawn")
	void SetALSViewMode(const ELSViewMode NewALSViewMode);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "ALS_Pawn")
	void OnALSViewModeChange();

	// Aiming
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "ALS_Pawn")
	void SetALSAiming(const bool NewALSAiming);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "ALS_Pawn")
	void OnALSAimingChange();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "ALS_Pawn")
	ELSMovementMode GetALSMovementMode() const;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "ALS_Pawn")
	ELSMovementAction GetALSMovementAction() const;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "ALS_Pawn")
	ELSGait GetALSGait() const;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "ALS_Pawn")
	ELSStance GetALSStance() const;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "ALS_Pawn")
	ELSViewMode GetALSViewMode() const;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "ALS_Pawn")
	ELSRotationMode GetALSRotationMode() const;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "ALS_Pawn")
	FCameraFOVParam GetCameraFOVParam() const;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "ALS_Pawn")
	FCameraTraceParam GetCameraTraceParam() const;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "ALS_Pawn")
	FTransform GetPivotTarget() const;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "ALS_Pawn")
	FVector GetCameraTarget() const;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "ALS_Pawn")
	bool HasMovementInput() const;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "ALS_Pawn")
	bool HasMoving() const;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "ALS_Pawn")
	bool HasAiming() const;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "ALS_Pawn")
	bool HasDebugTrace() const;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "ALS_Pawn")
	void SetWalkingSpeed(const float InWalkingSpeed);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "ALS_Pawn")
	void SetRunningSpeed(const float InRunningSpeed);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "ALS_Pawn")
	void SetSprintingSpeed(const float InSprintingSpeed);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "ALS_Pawn")
	void SetCrouchingSpeed(const float InCrouchingSpeed);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "ALS_Pawn")
	void SetSwimmingSpeed(const float InSwimmingSpeed);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "ALS_Pawn")
	float GetWalkingSpeed() const;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "ALS_Pawn")
	float GetRunningSpeed() const;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "ALS_Pawn")
	float GetSprintingSpeed() const;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "ALS_Pawn")
	float GetCrouchingSpeed() const;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "ALS_Pawn")
	float GetSwimmingSpeed() const;

	// AnimInstance Only
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "ALS_Pawn")
	void SetALSIdleState(const ELSIdleEntryState InLSIdleEntryState);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "ALS_Pawn")
	void SetGetup(const bool InFaceDown);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "ALS_Pawn")
	void SetRF(const bool InRF);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "ALS_Pawn")
	void PoseSnapShot(const FName InPoseName);

};
