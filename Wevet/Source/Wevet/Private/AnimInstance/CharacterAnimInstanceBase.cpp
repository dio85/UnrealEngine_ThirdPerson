// Copyright 2018 wevet works All Rights Reserved.

#include "AnimInstance/CharacterAnimInstanceBase.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/KismetMathLibrary.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/PawnMovementComponent.h"
#include "WevetExtension.h"


UCharacterAnimInstanceBase::UCharacterAnimInstanceBase(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	Owner = nullptr;
	bWasMoving = false;
	bWasAiming = false;
	bDebugTrace = false;
	bWasGrounded = false;

	IKTargetInterpolationSpeed = 60.f;
	FalloutInterval = 3.f;
	ActiveLocomotionState = ELSLocomotionState::NotMoving;

	static ConstructorHelpers::FObjectFinder<UCurveVector> LowVectorAsset(Wevet::ProjectFile::GetMantleLowPath());
	static ConstructorHelpers::FObjectFinder<UCurveVector> HighVectorAsset(Wevet::ProjectFile::GetMantleHighPath());

	// DefaultMantleLow
	DefaultLowMantleAsset.StartingOffset = FVector(0.f, 65.f, 100.f);
	DefaultLowMantleAsset.LowHeight = 50.f;
	DefaultLowMantleAsset.LowPlayRate = 1.f;
	DefaultLowMantleAsset.LowStartPosition = 0.6f;
	DefaultLowMantleAsset.HighHeight = 100.f;
	DefaultLowMantleAsset.HighPlayRate = 1.f;
	DefaultLowMantleAsset.HighStartPosition = 0.0f;
	DefaultLowMantleAsset.Position = LowVectorAsset.Object;

	// DefaultMantleHigh
	DefaultHighMantleAsset.StartingOffset = FVector(0.f, 65.f, 200.f);
	DefaultHighMantleAsset.LowHeight = 125.f;
	DefaultHighMantleAsset.LowPlayRate = 1.2f;
	DefaultHighMantleAsset.LowStartPosition = 0.6f;
	DefaultHighMantleAsset.HighHeight = 200.f;
	DefaultHighMantleAsset.HighPlayRate = 1.2f;
	DefaultHighMantleAsset.HighStartPosition = 0.0f;
	DefaultHighMantleAsset.Position = HighVectorAsset.Object;


	// ALS_Layer
	BasePose_N = 1.0f;
	Enable_AimOffset = 1.0f;

	{
		static ConstructorHelpers::FObjectFinder<UCurveFloat> FindAsset(Wevet::ProjectFile::GetLandAlphaCurve());
		LandAlphaCurve = FindAsset.Object;
	}

	{
		static ConstructorHelpers::FObjectFinder<UCurveFloat> FindAsset(Wevet::ProjectFile::GetFlailAlphaCurve());
		FlailAlphaCurve = FindAsset.Object;
	}
}


void UCharacterAnimInstanceBase::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();
	OwningPawn = TryGetPawnOwner();
	Owner = Cast<ACharacterBase>(OwningPawn);

	if (!Owner)
	{
		return;
	}

	CharacterMovementComponent = Owner->GetCharacterMovement();
	CapsuleComponent = Owner->GetCapsuleComponent();

	ILocomotionSystemPawn::Execute_SetALSMovementMode(this, ILocomotionSystemPawn::Execute_GetALSMovementMode(Owner));
	ILocomotionSystemPawn::Execute_SetALSRotationMode(this, ILocomotionSystemPawn::Execute_GetALSRotationMode(Owner));
	ILocomotionSystemPawn::Execute_SetALSGait(this, ILocomotionSystemPawn::Execute_GetALSGait(Owner));
	ILocomotionSystemPawn::Execute_SetALSStance(this, ILocomotionSystemPawn::Execute_GetALSStance(Owner));
	ILocomotionSystemPawn::Execute_SetALSViewMode(this, ILocomotionSystemPawn::Execute_GetALSViewMode(Owner));
	ILocomotionSystemPawn::Execute_SetALSIdleState(this, (ALSStance == ELSStance::Standing) ? ELSIdleEntryState::N_Idle : ELSIdleEntryState::CLF_Idle);

	ILocomotionSystemPawn::Execute_SetWalkingSpeed(this, ILocomotionSystemPawn::Execute_GetWalkingSpeed(Owner));
	ILocomotionSystemPawn::Execute_SetRunningSpeed(this, ILocomotionSystemPawn::Execute_GetRunningSpeed(Owner));
	ILocomotionSystemPawn::Execute_SetSprintingSpeed(this, ILocomotionSystemPawn::Execute_GetSprintingSpeed(Owner));
	ILocomotionSystemPawn::Execute_SetCrouchingSpeed(this, ILocomotionSystemPawn::Execute_GetCrouchingSpeed(Owner));
	ILocomotionSystemPawn::Execute_SetSwimmingSpeed(this, ILocomotionSystemPawn::Execute_GetSwimmingSpeed(Owner));
}


void UCharacterAnimInstanceBase::NativeUpdateAnimation(float DeltaTimeX)
{
	Super::NativeUpdateAnimation(DeltaTimeX);

	if (!OwningPawn)
	{
		return;
	}


	// @NOTE
	// Player not Any Classes Fallout
	{
		if (ALSMovementMode == ELSMovementMode::Falling)
		{
			FalloutTickTime += DeltaTimeX;
			if (FalloutTickTime >= FalloutInterval)
			{
				IsFallout = true;
			}
		}
		else
		{
			IsFallout = false;
			FalloutTickTime = 0.f;
		}
	}

	UpdateCombatSystem();
	UpdateLocomotionSystem();
}


#pragma region Combat
void UCharacterAnimInstanceBase::UpdateCombatSystem()
{
	SetEquip();
	SetWeaponItemType();
	SetWeaponFabrikIKTransform();
}


void UCharacterAnimInstanceBase::SetEquip()
{
	if (Owner)
	{
		IsEquip = Owner->HasEquipWeapon();
	}
}


void UCharacterAnimInstanceBase::SetWeaponItemType()
{
	if (Owner)
	{
		EWeaponItemType Current = Owner->GetCurrentWeaponType();

		if (Current != WeaponItemType)
		{
			WeaponItemType = Current;
		}
	}
}


void UCharacterAnimInstanceBase::SetWeaponFabrikIKTransform()
{
	if (!Owner)
	{
		return;
	}

	auto Weapon = Owner->GetCurrentWeapon();
	if (!Weapon)
	{
		return;
	}

	switch (WeaponItemType)
	{
		case EWeaponItemType::Pistol:
		case EWeaponItemType::Rifle:
		case EWeaponItemType::Sniper:
		{
			Super::SetHandTransform(Weapon->GetGripTransform(), FTransform::Identity);
		}
		break;
	}
}


void UCharacterAnimInstanceBase::SetWeaponFabrikEnable()
{
	if (!Owner)
	{
		return;
	}


	auto Weapon = Owner->GetCurrentWeapon();
	if (!Weapon)
	{
		Super::SetArmTorsoIKMode(false, false);
		return;
	}

	switch (WeaponItemType)
	{
		case EWeaponItemType::Pistol:
		{
			if (!Owner->HasAiming_Implementation())
			{
			}
			Super::SetArmTorsoIKMode(true, false);
		}
		break;
		case EWeaponItemType::Rifle:
		case EWeaponItemType::Sniper:
		{
			Super::SetArmTorsoIKMode(true, false);
		}
		break;
	}
}
#pragma endregion


#pragma region Utils
bool UCharacterAnimInstanceBase::IsLocallyControlled() const
{
	if (TryGetPawnOwner())
	{
		return TryGetPawnOwner()->IsLocallyControlled();
	}
	return false;
}


FWeaponActionInfo* UCharacterAnimInstanceBase::GetActionInfo(const EWeaponItemType InWeaponItemType)
{
	if (Wevet::ArrayExtension::NullOrEmpty(ActionInfoArray))
	{
		return nullptr;
	}

	for (FWeaponActionInfo& Info : ActionInfoArray)
	{
		if (InWeaponItemType == Info.WeaponItemType)
		{
			return &Info;
		}
	}
	return nullptr;
}


const float UCharacterAnimInstanceBase::TakeDefaultDamage()
{
	if (DefaultHitDamageMontage == nullptr || Montage_IsPlaying(DefaultHitDamageMontage))
	{
		return ZERO_VALUE;
	}
	return Super::Montage_Play(DefaultHitDamageMontage);
}


const float UCharacterAnimInstanceBase::PlayAnimationSequence(const FAnimSequenceInfo InAnimSequenceInfo, const FName InSlotNodeName)
{
	check(InAnimSequenceInfo.Animation);

	PlaySlotAnimationAsDynamicMontage(
		InAnimSequenceInfo.Animation,
		InSlotNodeName,
		InAnimSequenceInfo.BlendInTime,
		InAnimSequenceInfo.BlendOutTime,
		InAnimSequenceInfo.PlayRate);
	return InAnimSequenceInfo.Animation->GetPlayLength();
}


const float UCharacterAnimInstanceBase::TakeDamageAnimation(FWeaponActionInfo* const InActionInfoPtr, const FName InSlotNodeName)
{
	if (InActionInfoPtr == nullptr)
	{
		return TakeDefaultDamage();
	}

	if (InActionInfoPtr->HitDamageSequence.Animation == nullptr)
	{
		return TakeDefaultDamage();
	}

	return PlayAnimationSequence(InActionInfoPtr->HitDamageSequence, InSlotNodeName);
}


float UCharacterAnimInstanceBase::GetAnimCurve(const FName InCurveName) const
{
	return GetCurveValue(InCurveName);
}
#pragma endregion


#pragma region ALS_Core
// MovementMode
void UCharacterAnimInstanceBase::SetALSMovementMode_Implementation(const ELSMovementMode InLSMovementMode)
{
	ALSPrevMovementMode = ALSMovementMode;
	ALSMovementMode = InLSMovementMode;
	ILocomotionSystemPawn::Execute_OnALSMovementModeChange(this);
}


void UCharacterAnimInstanceBase::OnALSMovementModeChange_Implementation()
{
	switch (ALSMovementMode)
	{
		case ELSMovementMode::Falling:
		{
			N_PlayRate = 0.0f;
			C_PlayRate = 0.0f;
			bShouldTurnInPlace = false;
		}
		break;
		case ELSMovementMode::Ragdoll:
		{
			const float BlendOut = 0.2f;
			Montage_Stop(BlendOut, nullptr);
		}
		break;
	}
}

// MovementAction
void UCharacterAnimInstanceBase::SetALSMovementAction_Implementation(const ELSMovementAction NewALSMovementAction)
{
	ALSMovementAction = NewALSMovementAction;
}


void UCharacterAnimInstanceBase::OnALSMovementActionChange_Implementation()
{
}

// Gait
void UCharacterAnimInstanceBase::SetALSGait_Implementation(const ELSGait InLSGait)
{
	ALSGait = InLSGait;
}


void UCharacterAnimInstanceBase::OnALSGaitChange_Implementation()
{
}

// Stance
void UCharacterAnimInstanceBase::SetALSStance_Implementation(const ELSStance InLSStance)
{
	ALSStance = InLSStance;
	ILocomotionSystemPawn::Execute_OnALSStanceChange(this);
}


void UCharacterAnimInstanceBase::OnALSStanceChange_Implementation()
{
	if (bTurningInPlace)
	{
		Montage_Stop(DEFAULT_ANIM_TRANSITION, ActiveTurnInPlaceMontage ? ActiveTurnInPlaceMontage : nullptr);
	}
}

// RotationMode
void UCharacterAnimInstanceBase::SetALSRotationMode_Implementation(const ELSRotationMode InLSRotationMode)
{
	ALSRotationMode = InLSRotationMode;
}


void UCharacterAnimInstanceBase::OnALSRotationModeChange_Implementation()
{
}

// ViewMode
void UCharacterAnimInstanceBase::SetALSViewMode_Implementation(const ELSViewMode InLSViewMode)
{
	ALSViewMode = InLSViewMode;
}


void UCharacterAnimInstanceBase::OnALSViewModeChange_Implementation()
{
}

// Aiming
void UCharacterAnimInstanceBase::SetALSAiming_Implementation(const bool InAiming)
{
	bWasAiming = InAiming;
}


void UCharacterAnimInstanceBase::OnALSAimingChange_Implementation()
{
}

// Speeds
void UCharacterAnimInstanceBase::SetWalkingSpeed_Implementation(const float InWalkingSpeed)
{
	WalkingSpeed = InWalkingSpeed;
	//UE_LOG(LogWevetClient, Log, TEXT("Walk : %s"), *FString(__FUNCTION__));
}


void UCharacterAnimInstanceBase::SetRunningSpeed_Implementation(const float InRunningSpeed)
{
	RunningSpeed = InRunningSpeed;
	//UE_LOG(LogWevetClient, Log, TEXT("Run : %s"), *FString(__FUNCTION__));
}


void UCharacterAnimInstanceBase::SetSprintingSpeed_Implementation(const float InSprintingSpeed)
{
	SprintingSpeed = InSprintingSpeed;
	//UE_LOG(LogWevetClient, Log, TEXT("Sprint : %s"), *FString(__FUNCTION__));
}


void UCharacterAnimInstanceBase::SetCrouchingSpeed_Implementation(const float InCrouchingSpeed)
{
	CrouchingSpeed = InCrouchingSpeed;
	//UE_LOG(LogWevetClient, Log, TEXT("Crouch : %s"), *FString(__FUNCTION__));
}


void UCharacterAnimInstanceBase::SetSwimmingSpeed_Implementation(const float InSwimmingSpeed)
{
	SwimmingSpeed = InSwimmingSpeed;
	//UE_LOG(LogWevetClient, Log, TEXT("Swim : %s"), *FString(__FUNCTION__));
}
#pragma endregion


#pragma region ALS_Other
void UCharacterAnimInstanceBase::SetALSIdleState_Implementation(const ELSIdleEntryState InLSIdleEntryState)
{
	ALSIdleEntryState = InLSIdleEntryState;
}


void UCharacterAnimInstanceBase::SetGetup_Implementation(const bool InFaceDown)
{
	if (!GetUpFromFront || !GetUpFromBack)
	{
		UE_LOG(LogWevetClient, Error, TEXT("Any Montages nullptr : %s"), *FString(__FUNCTION__));
		return;
	}
	const float PlayRate = 1.0f;
	const float InTimeToStartMontageAt = 0.0f;
	auto Montage = (InFaceDown) ? GetUpFromFront : GetUpFromBack;
	Montage_Play(Montage, PlayRate, EMontagePlayReturnType::MontageLength, InTimeToStartMontageAt, true);
	BP_ReplicatePlayMontage(Montage, PlayRate, InTimeToStartMontageAt, true);
}


void UCharacterAnimInstanceBase::SetRF_Implementation(const bool InRF)
{
	bRF = InRF;
}


void UCharacterAnimInstanceBase::PoseSnapShot_Implementation(const FName InPoseName)
{
	SavePoseSnapshot(InPoseName);
}


void UCharacterAnimInstanceBase::SetALSAnimNotifyTurnInPlace_Implementation(UAnimMontage* InTurnInPlaceMontage, const bool InShouldTurnInPlace, const bool InTurningInPlace, const bool InTurningRight)
{
	ActiveTurnInPlaceMontage = InTurnInPlaceMontage;
	bShouldTurnInPlace = InShouldTurnInPlace;
	bTurningInPlace = InTurningInPlace;
	bTurningRight = InTurningRight;

#if false
	if (ActiveTurnInPlaceMontage)
	{
		UE_LOG(LogWevetClient, Log, TEXT("TurnInPlace : %s, funcName : %s"), *ActiveTurnInPlaceMontage->GetName(), *FString(__FUNCTION__));
	}
#endif

}


void UCharacterAnimInstanceBase::SetALSAnimNotifyPivotData_Implementation(const FPivotData InPivotData)
{
	PivotData = InPivotData;
}
#pragma endregion


#pragma region ALS_NotUseAPI
ELSMovementMode UCharacterAnimInstanceBase::GetALSMovementMode_Implementation() const
{
	return ALSMovementMode;
}


ELSMovementAction UCharacterAnimInstanceBase::GetALSMovementAction_Implementation() const
{
	return ALSMovementAction;
}


ELSGait UCharacterAnimInstanceBase::GetALSGait_Implementation() const
{
	return ALSGait;
}


ELSStance UCharacterAnimInstanceBase::GetALSStance_Implementation() const
{
	return ALSStance;
}


ELSViewMode UCharacterAnimInstanceBase::GetALSViewMode_Implementation() const
{
	return ALSViewMode;
}


ELSRotationMode UCharacterAnimInstanceBase::GetALSRotationMode_Implementation() const
{
	return ALSRotationMode;
}


void UCharacterAnimInstanceBase::SetALSCharacterRotation_Implementation(const FRotator AddAmount)
{
}


void UCharacterAnimInstanceBase::SetALSCameraShake_Implementation(TSubclassOf<class UCameraShakeBase> InShakeClass, const float InScale)
{
}


bool UCharacterAnimInstanceBase::HasMovementInput_Implementation() const
{
	return bWasMovementInput;
}


bool UCharacterAnimInstanceBase::HasMoving_Implementation() const
{
	return bWasMoving;
}


bool UCharacterAnimInstanceBase::HasDebugTrace_Implementation() const
{
	return bDebugTrace;
}


bool UCharacterAnimInstanceBase::HasAiming_Implementation() const
{
	return bWasAiming;
}


FTransform UCharacterAnimInstanceBase::GetPivotTarget_Implementation() const
{
	// Not Use
	return ILocomotionSystemPawn::Execute_GetPivotTarget(Owner);
}


FVector UCharacterAnimInstanceBase::GetCameraTarget_Implementation() const
{
	// Not Use
	return ILocomotionSystemPawn::Execute_GetCameraTarget(Owner);
}


float UCharacterAnimInstanceBase::GetWalkingSpeed_Implementation() const
{
	// Not Use
	return WalkingSpeed;
}


float UCharacterAnimInstanceBase::GetRunningSpeed_Implementation() const
{
	// Not Use
	return RunningSpeed;
}


float UCharacterAnimInstanceBase::GetSprintingSpeed_Implementation() const
{
	// Not Use
	return SprintingSpeed;
}


float UCharacterAnimInstanceBase::GetCrouchingSpeed_Implementation() const
{
	// Not Use
	return CrouchingSpeed;
}


float UCharacterAnimInstanceBase::GetSwimmingSpeed_Implementation() const
{
	// Not Use
	return SwimmingSpeed;
}


FCameraFOVParam UCharacterAnimInstanceBase::GetCameraFOVParam_Implementation() const
{
	// Not Use
	return ILocomotionSystemPawn::Execute_GetCameraFOVParam(Owner);
}


FCameraTraceParam UCharacterAnimInstanceBase::GetCameraTraceParam_Implementation() const
{
	// Not Use
	return ILocomotionSystemPawn::Execute_GetCameraTraceParam(Owner);
}
#pragma endregion


void UCharacterAnimInstanceBase::UpdateLocomotionSystem()
{
	SetVariableFromOwner();
	CalculateAimOffset();
	CalculateMovementState();
	CalculateLayerValue();
}


void UCharacterAnimInstanceBase::SetVariableFromOwner()
{
	bWasMovementInput = ILocomotionSystemPawn::Execute_HasMovementInput(Owner);
	bDebugTrace = ILocomotionSystemPawn::Execute_HasDebugTrace(Owner);
	bWasMoving = ILocomotionSystemPawn::Execute_HasMoving(Owner);

	LastVelocityRotation = Owner->GetLastVelocityRotation();
	LastMovementInputRotation = Owner->GetLastMovementInputRotation();
	RotationDifference = Owner->GetRotationDifference();
	VelocityDifference = Owner->GetVelocityDifference();
	CharacterRotation = Owner->GetCharacterRotation();
	LookingRotation = Owner->GetLookingRotation();
	Direction = Owner->GetDirection();
	AimYawDelta = Owner->GetAimYawDelta();
	AimYawRate = Owner->GetAimYawRate();
	Velocity = Owner->ChooseVelocity();

	bWasHealthHalf = Owner->IsHealthHalf();
	bWasGrounded = (ALSMovementMode != ELSMovementMode::Falling);
}


void UCharacterAnimInstanceBase::CalculateAimOffset()
{
	const float DeltaSeconds = GetWorld()->GetDeltaSeconds();
	switch (ALSRotationMode)
	{
		case ELSRotationMode::VelocityDirection:
		{
			if (bWasMovementInput)
			{
				const FRotator DeltaRot = UKismetMathLibrary::NormalizedDeltaRotator(LastMovementInputRotation, CharacterRotation);
				const float InterpSpeed = UKismetMathLibrary::MapRangeClamped(FMath::Abs(DeltaRot.Yaw - AimOffset.X), 0.0f, 180.f, 15.f, 5.0f);
				const float X = FMath::Clamp(DeltaRot.Yaw, -90.f, 90.f);
				const float Y = DeltaRot.Pitch;
				AimOffset = UKismetMathLibrary::Vector2DInterpTo(AimOffset, FVector2D(X, Y), DeltaSeconds, InterpSpeed);
			}
			else
			{
				const float InterpSpeed = 4.0f;
				AimOffset = UKismetMathLibrary::Vector2DInterpTo(AimOffset, FVector2D::ZeroVector, DeltaSeconds, InterpSpeed);
			}
		}
		break;

		case ELSRotationMode::LookingDirection:
		{
			const FRotator DeltaRot = UKismetMathLibrary::NormalizedDeltaRotator(LookingRotation, CharacterRotation);
			const float InterpSpeed = UKismetMathLibrary::MapRangeClamped(FMath::Abs(DeltaRot.Yaw - AimOffset.X), 0.0f, 180.f, 30.f, 5.0f);
			AimOffset = UKismetMathLibrary::Vector2DInterpTo(AimOffset, FVector2D(DeltaRot.Yaw, DeltaRot.Pitch), DeltaSeconds, InterpSpeed);
		}
		break;
	}
}


void UCharacterAnimInstanceBase::CalculateMovementState()
{
	switch (ALSMovementMode)
	{
		case ELSMovementMode::Falling:
		DoWhileFalling();
		break;

		case ELSMovementMode::Grounded:
		case ELSMovementMode::Swimming:
		DoWhileGrounded();
		DoWhileLocomotionState();
		break;

		case ELSMovementMode::Ragdoll:
		DoWhileRagdoll();
		break;
	}

}


void UCharacterAnimInstanceBase::CalculateLayerValue()
{
	Enable_AimOffset = FMath::Lerp(0.0f, 1.0f, GetAnimCurve(FName(TEXT("Mask_AimOffset"))));

	BasePose_N = ALSStance == ELSStance::Standing ? 1.0f : 0.f;
	BasePose_CLF = ALSStance == ELSStance::Standing ? 0.0f : 1.0f;

	Spine_Add = GetAnimCurve(FName(TEXT("Layering_Spine_Add")));
	Head_Add = GetAnimCurve(FName(TEXT("Layering_Head_Add")));
	Arm_L_Add = GetAnimCurve(FName(TEXT("Layering_Arm_L_Add")));
	Arm_R_Add = GetAnimCurve(FName(TEXT("Layering_Arm_R_Add")));

	Hand_L = GetAnimCurve(FName(TEXT("Layering_Hand_L")));
	Hand_R = GetAnimCurve(FName(TEXT("Layering_Hand_R")));

	Arm_L_LS = GetAnimCurve(FName(TEXT("Layering_Arm_L_LS")));
	Arm_R_LS = GetAnimCurve(FName(TEXT("Layering_Arm_R_LS")));

	{
		const int32 Diff = (1 - (int32)Arm_L_LS);
		Arm_L_MS = (float)Diff;
	}
	{
		const int32 Diff = (1 - (int32)Arm_R_LS);
		Arm_R_MS = (float)Diff;
	}
}


void UCharacterAnimInstanceBase::DoWhileGrounded()
{
	UpdateMovementSpeed(true);
	if (bWasMoving)
	{
		WhileMoving();
	}
	else
	{
		WhileTurnInPlace();
	}
}


void UCharacterAnimInstanceBase::DoWhileLocomotionState()
{
	switch (ActiveLocomotionState)
	{
		case ELSLocomotionState::NotMoving:
		CalculateStartPosition();
		break;
		case ELSLocomotionState::Moving:
		CalculateGroundedLeaningValues();
		break;
		case ELSLocomotionState::Pivot:
		CalcuratePivotState();
		break;
		case ELSLocomotionState::Stopping:
		{
			//
		}
		break;
	}
}


void UCharacterAnimInstanceBase::DoWhileFalling()
{
	UpdateMovementSpeed(false);
	UpdateFlailBlendAlpha();
	CalculateInAirLeaningValues();
	CalculateLandPredictionAlpha();
}


void UCharacterAnimInstanceBase::DoWhileRagdoll()
{
	FlailRate = UKismetMathLibrary::MapRangeClamped(UKismetMathLibrary::VSize(Velocity), 0.0f, 1000.f, 0.0f, 1.25f);
}


void UCharacterAnimInstanceBase::WhileMoving()
{
	CalculateGaitValue();

	const float Walk = 150.f;
	const float Run = 300.f;
	const float Sprint = 600.f;
	const float Crouch = 150.f;
	CalculatePlayRates(Walk, Run, Sprint, Crouch);

	const float Min = -90.f;
	const float Max = 90.f;
	const float Buffer = 5.f;
	CalculateMovementDirection(Min, Max, Buffer);
}


void UCharacterAnimInstanceBase::WhileTurnInPlace()
{
	if (!IsLocallyControlled())
	{
		return;
	}

	if (!Owner || Owner->IsPlayingRootMotion())
	{
		return;
	}

	if (ALSRotationMode != ELSRotationMode::LookingDirection)
	{
		return;
	}

	if (bWasAiming)
	{
		const float YawLimit = 60.f;
		const float PlayRate = 1.5f;
		switch (ALSStance)
		{
			case ELSStance::Standing:
			OnTurnInPlaceRespons(YawLimit, GetTurnData(), PlayRate);
			break;
			case ELSStance::Crouching:
			OnTurnInPlaceRespons(YawLimit, GetCrouchTurnData(), PlayRate);
			break;
		}
	}
	else
	{
		switch (ALSViewMode)
		{
			case ELSViewMode::FirstPerson:
			{
				const float YawLimit = 70.f;
				const float PlayRate = 1.5f;
				switch (ALSStance)
				{
					case ELSStance::Standing:
					OnTurnInPlaceRespons(YawLimit, N_Turn_90, PlayRate);
					break;
					case ELSStance::Crouching:
					OnTurnInPlaceRespons(YawLimit, GetCrouchTurnData(), PlayRate);
					break;
				}
			}
			break;

			case ELSViewMode::ThirdPerson:
			{
				if (bTurningInPlace)
				{
					return;
				}

				const float MaxCameraSpeed = 100.f;
				const float YawFirst = 60.f;
				const float YawSecond = 130.f;
				const float PlayRateStand  = 1.5f;
				const float PlayRateCrouch = 1.25f;
				const float Delay = 0.5f;
				switch (ALSStance)
				{
					case ELSStance::Standing:
					OnTurnInPlaceDelay(MaxCameraSpeed, YawFirst, Delay, PlayRateStand, N_Turn_90, YawSecond, 0.0f, PlayRateCrouch, N_Turn_180);
					break;
					case ELSStance::Crouching:
					const FTurnMontages Montage = GetCrouchTurnData();
					OnTurnInPlaceDelay(MaxCameraSpeed, YawFirst, Delay, PlayRateCrouch, Montage, YawSecond, 0.0f, PlayRateStand, Montage);
					break;
				}
			}
			break;
		}
	}
}


FMantleAsset UCharacterAnimInstanceBase::GetMantleAsset(const EMantleType InMantleType) const
{
	FMantleAsset Temp;
	switch (InMantleType)
	{
		case EMantleType::LowMantle:
		Temp = DefaultLowMantleAsset;
		break;

		case EMantleType::FallingCatch:
		case EMantleType::HighMantle:
		Temp = DefaultHighMantleAsset;
		break;
	}
	return Temp;
}


void UCharacterAnimInstanceBase::CalculateGaitValue()
{
	const float MoveSpeed = UKismetMathLibrary::MapRangeClamped(MovementSpeed, 0.0f, WalkingSpeed, 0.0f, 1.0f);
	const float WalkSpeed = UKismetMathLibrary::MapRangeClamped(MovementSpeed, WalkingSpeed, RunningSpeed, 1.0f, 2.0f);
	const float RunSpeed = UKismetMathLibrary::MapRangeClamped(MovementSpeed, RunningSpeed, SprintingSpeed, 2.0f, 3.0f);

	const bool bWalkedGreater = (MovementSpeed > WalkingSpeed);
	const bool bRunnedGreater = (MovementSpeed > RunningSpeed);

	const float CurrentSpeed = bRunnedGreater ? RunSpeed : bWalkedGreater ? WalkSpeed : MoveSpeed;
	GaitValue = CurrentSpeed;
}


void UCharacterAnimInstanceBase::CalculatePlayRates(const float Walk, const float Run, const float Sprint, const float Crouch)
{
	const float MinRate = 0.0f;
	const float MaxRate = 1.0f;
	const float WalkValue = UKismetMathLibrary::MapRangeUnclamped(MovementSpeed, 0.0f, Walk, MinRate, MaxRate);
	const float RunValue = UKismetMathLibrary::MapRangeUnclamped(MovementSpeed, 0.0f, Run, MinRate, MaxRate);
	const float SprintValue = UKismetMathLibrary::MapRangeUnclamped(MovementSpeed, 0.0f, Sprint, MinRate, MaxRate);
	const float CrouchValue = UKismetMathLibrary::MapRangeUnclamped(MovementSpeed, 0.0f, Crouch, MinRate, MaxRate);

	const float GaitWalk = UKismetMathLibrary::MapRangeClamped(GaitValue, 1.0f, 2.0f, WalkValue, RunValue);
	const float GaitRun = UKismetMathLibrary::MapRangeClamped(GaitValue, 2.0f, 3.0f, RunValue, SprintValue);
	const float Diff = (GaitValue > 2.0f) ? GaitRun : GaitWalk;
	N_PlayRate = (Diff / CapsuleComponent->GetComponentScale().Z);
	C_PlayRate = (CrouchValue / CapsuleComponent->GetComponentScale().Z);
}


void UCharacterAnimInstanceBase::CalculateMovementDirection(const float Min, const float Max, const float Buffer)
{
	const bool A = UKismetMathLibrary::InRange_FloatFloat(Direction, (Min - Buffer), (Max + Buffer));
	const bool B = UKismetMathLibrary::InRange_FloatFloat(Direction, (Min + Buffer), (Max - Buffer));
	const bool bResult = (MovementDirection == ELSMovementDirection::Forwards) ? A : B;
	MovementDirection = bResult ? ELSMovementDirection::Forwards : ELSMovementDirection::Backwards;
}


// Locomotion State NotMoving
void UCharacterAnimInstanceBase::CalculateStartPosition()
{
	if (!bWasMoving)
	{
		return;
	}

	switch (ALSStance)
	{
		case ELSStance::Standing:
		{
			if (bWasAiming)
			{
				StartPosition = (Direction > 0.0f) ? 0.187f : 1.0f;
			}
			else
			{
				StartPosition = (Direction > 0.0f) ? 0.3f : 0.867f;
			}
		}
		break;
		case ELSStance::Crouching:
		{
			StartPosition = (Direction > 0.0f) ? 0.25f : 0.5f;
		}
		break;
	}
}

// Locomotion State Pivot
void UCharacterAnimInstanceBase::CalcuratePivotState()
{
	const float Tolerance = 45.0f;
	const bool bEqual = FMath::IsNearlyEqual(Direction, PivotData.PivotDirection, Tolerance);
	MovementDirection = (bEqual) ? PivotData.InterruptedMovementDirection : PivotData.CompletedMovementDirection;
	StartPosition = (bEqual) ? PivotData.InterruptedStartTime : PivotData.CompletedStartTime;
}

// Locomotion State Moving
void UCharacterAnimInstanceBase::CalculateGroundedLeaningValues()
{
	check(GetWorld());

	const float DeltaSeconds = GetWorld()->GetDeltaSeconds();
	const float Speed = UKismetMathLibrary::MapRangeClamped(MovementSpeed, WalkingSpeed, RunningSpeed, 0.0f, 1.0f);
	const FRotator DeltaRot = UKismetMathLibrary::NormalizedDeltaRotator(LastVelocityRotation, PreviousVelocityRotation);
	DeltaVelocityDifference = (DeltaRot.Yaw / DeltaSeconds);
	PreviousVelocityRotation = LastVelocityRotation;
	const float DeltaVelocityClampValue = UKismetMathLibrary::MapRangeClamped(DeltaVelocityDifference, -200.0f, 200.0f, -1.0f, 1.0f);
	const float LeanRotation = (DeltaVelocityClampValue * Speed);

	AccelerationDifference = (MovementSpeed - PreviousSpeed) / DeltaSeconds;
	PreviousSpeed = MovementSpeed;
	const float MaxAcceleration = CharacterMovementComponent->GetMaxAcceleration();
	const float BrakingDecelerationWalking = CharacterMovementComponent->GetMaxBrakingDeceleration();
	const float MaxAccelerationClamp = UKismetMathLibrary::MapRangeClamped(FMath::Abs(AccelerationDifference), 0.0f, MaxAcceleration, 0.0f, 1.0f);
	const float BrakingDecelerationClamp = UKismetMathLibrary::MapRangeClamped(FMath::Abs(AccelerationDifference), 0.0f, BrakingDecelerationWalking, 0.0f, -1.0f);
	const float LeanAcceleration = Speed * UKismetMathLibrary::SelectFloat(MaxAccelerationClamp, BrakingDecelerationClamp, (AccelerationDifference > 0.0f));

	const FVector LeanPosition = FVector(LeanRotation, LeanAcceleration, 0.0f);
	const FVector AngleAxis = UKismetMathLibrary::RotateAngleAxis(LeanPosition, Direction, FVector(0.0f, 0.0f, -1.0f));
	LeanGrounded.X = AngleAxis.X;
	LeanGrounded.Y = AngleAxis.Y;
}

// Falling Update
void UCharacterAnimInstanceBase::CalculateInAirLeaningValues()
{
	const float JumpVelocity = CharacterMovementComponent->JumpZVelocity;
	const float Value = UKismetMathLibrary::MapRangeClamped(Velocity.Z, JumpVelocity, JumpVelocity * -2.f, 1.0f, -1.0f);
	const float NormalizeValue = UKismetMathLibrary::NormalizeToRange(MovementSpeed, 0.0f, RunningSpeed);
	LeanInAir = (Value * NormalizeValue);
}

// Falling Update
void UCharacterAnimInstanceBase::CalculateLandPredictionAlpha()
{
	const float DeltaSeconds = GetWorld()->GetDeltaSeconds();
	float InterpSpeed = 10.f;

	if (Velocity.Z > 0.0f)
	{
		LandPredictionAlpha = UKismetMathLibrary::FInterpTo(LandPredictionAlpha, 0.0f, DeltaSeconds, InterpSpeed);
		return;
	}

	if (!Owner || !CharacterMovementComponent || !CapsuleComponent)
	{
		LandPredictionAlpha = UKismetMathLibrary::FInterpTo(LandPredictionAlpha, 0.0f, DeltaSeconds, InterpSpeed);
		return;
	}


	const FVector Location = Owner->GetActorLocation();
	const float Radius = CapsuleComponent->GetScaledCapsuleRadius();
	const float OffsetZ = (Location.Z - CapsuleComponent->GetScaledCapsuleHalfHeight_WithoutHemisphere());
	const FVector StartLocation = FVector(Location.X, Location.Y, OffsetZ);

	const float ClampMin = -4000.f;
	const float ClampMax = 2000.f;
	const float DrawTime = 1.0f;

	FVector EndLocation = UKismetMathLibrary::Normal(FVector(Velocity.X, Velocity.Y, FMath::Clamp(Velocity.Z, ClampMin, -200.f)));
	EndLocation *= UKismetMathLibrary::MapRangeClamped(Velocity.Z, 0.0f, ClampMin, 50.f, ClampMax);
	EndLocation += StartLocation;
	//TArray<AActor*> IgnoreActors = Owner->GetIgnoreActors();
	FHitResult HitData(ForceInit);
	TArray<AActor*> IgnoreActors;

	UKismetSystemLibrary::SphereTraceSingle(
		GetWorld(),
		StartLocation,
		EndLocation,
		Radius,
		UEngineTypes::ConvertToTraceType(ECC_Visibility),
		false,
		IgnoreActors,
		Owner->GetDrawDebugTrace(),
		HitData,
		true, 
		FLinearColor::Red, 
		FLinearColor::Green, 
		DrawTime);

	const float FloorZ = CharacterMovementComponent->GetWalkableFloorZ();
	const bool bWasHitNormalGreater = (HitData.ImpactNormal.Z >= FloorZ);
	if (HitData.bBlockingHit && bWasHitNormalGreater)
	{
		InterpSpeed = 20.f;
		const float Value = UKismetMathLibrary::MapRangeClamped(HitData.Time, 0.0f, 1.0f, 1.0f, 0.0f);
		const float CurveValue = LandAlphaCurve ? LandAlphaCurve->GetFloatValue(Value) : Value;
		LandPredictionAlpha = UKismetMathLibrary::FInterpTo(LandPredictionAlpha, CurveValue, DeltaSeconds, InterpSpeed);
	}
	else
	{
		LandPredictionAlpha = UKismetMathLibrary::FInterpTo(LandPredictionAlpha, 0.0f, DeltaSeconds, InterpSpeed);
	}
}

// Falling Update
void UCharacterAnimInstanceBase::UpdateFlailBlendAlpha()
{
	FlailBlendAlpha = FlailAlphaCurve->GetFloatValue(Velocity.Z * -1.f);
}


void UCharacterAnimInstanceBase::OnTurnInPlaceRespons(const float AimYawLimit, const FTurnMontages TurnAnims, const float PlayRate)
{
	bShouldTurnInPlace = (FMath::Abs(AimYawDelta) > AimYawLimit);
	UAnimMontage* SelectMontage = (AimYawDelta > 0.0f) ? TurnAnims.TurnRAnim : TurnAnims.TurnLAnim;

	if (!bShouldTurnInPlace)
	{
		return;
	}

	if (!SelectMontage)
	{
		return;
	}

	bool bSuccess = false;
	if (bTurningInPlace)
	{
		const bool bAimYawGreater = (AimYawDelta > 0.0f);

		if (bTurningRight)
		{
			if (!bAimYawGreater)
			{
				bSuccess = true;
			}
		}
		else
		{
			if (bAimYawGreater)
			{
				bSuccess = true;
			}
		}
	}
	else
	{
		bSuccess = true;
	}

	if (!bSuccess || Montage_IsPlaying(SelectMontage))
	{
		UE_LOG(LogWevetClient, Error, TEXT("fail Success or Playing Montage => %s"), *SelectMontage->GetName());
		return;
	}
	const float Value = PlayRate * UKismetMathLibrary::MapRangeClamped(FMath::Abs(AimYawRate), 120.f, 400.f, 1.0f, 2.0f);
	Montage_Play(SelectMontage, Value, EMontagePlayReturnType::MontageLength, 0.0f, true);
	BP_ReplicatePlayMontage(SelectMontage, Value, 0.0f, true);
}


void UCharacterAnimInstanceBase::OnTurnInPlaceDelay(
	const float CameraSpeed,
	const float AimLimitFirst, 
	const float TimeFirst, 
	const float RateFirst, 
	const FTurnMontages TurnAnimsFirst, 
	const float AimLimitSecond, 
	const float TimeSecond, 
	const float RateSecond, 
	const FTurnMontages TurnAnimsSecond)
{

	check(GetWorld());

	UAnimMontage* FirstMontage = (AimYawDelta > 0.0f) ? TurnAnimsFirst.TurnRAnim : TurnAnimsFirst.TurnLAnim;
	UAnimMontage* SecondMontage = (AimYawDelta > 0.0f) ? TurnAnimsSecond.TurnRAnim : TurnAnimsSecond.TurnLAnim;
	UAnimMontage* SelectMontage = (FMath::Abs(AimYawDelta) >= AimLimitSecond) ? SecondMontage : FirstMontage;

	const float PlayRate = (FMath::Abs(AimYawDelta) >= AimLimitSecond) ? RateSecond : RateFirst;
	const float AimClamp = UKismetMathLibrary::MapRangeClamped(FMath::Abs(AimYawDelta), AimLimitFirst, AimLimitSecond, TimeFirst, TimeSecond);
	const bool bWasAimYawDeltaGreater = (FMath::Abs(AimYawDelta) > AimLimitFirst);
	const bool bWasCameraSpeedGreater = (FMath::Abs(AimYawRate) < CameraSpeed);
	const float DeltaSeconds = GetWorld()->GetDeltaSeconds();

	if (bWasCameraSpeedGreater && bWasAimYawDeltaGreater)
	{
		TurnInPlaceDelayCount += DeltaSeconds;
		bShouldTurnInPlace = (TurnInPlaceDelayCount > AimClamp);

		if (bShouldTurnInPlace)
		{
			if (!Montage_IsPlaying(SelectMontage))
			{
				Montage_Play(SelectMontage, PlayRate, EMontagePlayReturnType::MontageLength, 0.0f, true);
				BP_ReplicatePlayMontage(SelectMontage, PlayRate, 0.0f, true);
			}
		}
	}
	else
	{
		TurnInPlaceDelayCount = 0.0f;
	}
}


// if Falling UpdateParams Set 'MovementSpeed' to only use 'X' and 'Y' Velocity values.
void UCharacterAnimInstanceBase::UpdateMovementSpeed(const bool bWasGround)
{
	const FVector Value = FVector(Velocity.X, Velocity.Y, bWasGround ? Velocity.Z : 0.0f);
	MovementSpeed = UKismetMathLibrary::VSize(Value);
}


// Apply to AnimNotify
void UCharacterAnimInstanceBase::SetActiveLocomotionState(const ELSLocomotionState NewActiveLocomotionState)
{
	ActiveLocomotionState = NewActiveLocomotionState;
}


void UCharacterAnimInstanceBase::IdleTransition(UAnimSequenceBase* Animation, const float InPlayRate, const float InTimeToStartMontageAt)
{
	if (!Animation)
	{
		return;
	}

	const FName SlotName(TEXT("Transition"));
	const float BlendInTime = 0.2f;
	const float BlendOutTime = 0.2f;
	const float BlendOutTriggerTime = 0.0f;
	const int32 LoopCount = 1;
	PlaySlotAnimationAsDynamicMontage(Animation, SlotName, BlendInTime, BlendOutTime, InPlayRate, LoopCount, BlendOutTriggerTime, InTimeToStartMontageAt);
}


