// Copyright 2018 wevet works All Rights Reserved.


#include "FullbodyAnimInstance.h"
#include "RTIK.h"
#include "Kismet/KismetMathLibrary.h"

#define MIN 0.0f
#define MAX 1.0f
#define INIT_BONE_SETUP 0

UFullbodyAnimInstance::UFullbodyAnimInstance(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	MaxPelvisAdjustHeight = 60.f;
	LeftArmIKAlpha = MIN;
	RightArmIKAlpha = MIN;
	IKTargetInterpolationSpeed = 10.f;

	LeftFootIKMode = EHumanoidLegIKMode::IK_Human_Leg_Locomotion;
	RightFootIKMode = EHumanoidLegIKMode::IK_Human_Leg_Locomotion;
	ArmTorsoIKMode = EHumanoidArmTorsoIKMode::IK_Human_ArmTorso_Disabled;

	IKLeftLegTarget = FTransform::Identity;
	IKRightLegTarget = FTransform::Identity;
	IKLeftHandTarget = FTransform::Identity;
	IKRightHandTarget = FTransform::Identity;
	IKLeftHandTargetInterpolated = FTransform::Identity;
	IKRightHandTargetInterpolated = FTransform::Identity;

	bIKLeftHandEnabled = false;
	bIKRightHandEnabled = false;

	{
		FBoneReference RefBone(FName(TEXT("pelvis")));
		PelvisBone.BoneRef = RefBone;
	}

#if INIT_BONE_SETUP
	{
		FBoneReference RefBone1(FName(TEXT("upperarm_r")));
		FIKBone Bone1;
		Bone1.BoneRef = RefBone1;
		RightArmChain.BonesRootToEffector.Add(Bone1);

		FBoneReference RefBone2(FName(TEXT("lowerarm_r")));
		FIKBone Bone2;
		Bone2.BoneRef = RefBone2;
		RightArmChain.BonesRootToEffector.Add(Bone2);

		FBoneReference RefBone3(FName(TEXT("hand_r")));
		FIKBone Bone3;
		Bone3.BoneRef = RefBone3;
		RightArmChain.BonesRootToEffector.Add(Bone3);
	}
#endif

}

void UFullbodyAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	// Legs Initialize
	LeftLegWrapper = NewObject<UHumanoidLegChain_Wrapper>(GetOwningActor());
	LeftLegWrapper->Initialize(LeftLeg);
	RightLegWrapper = NewObject<UHumanoidLegChain_Wrapper>(GetOwningActor());
	RightLegWrapper->Initialize(RightLeg);

	// PelvisInitialize
	PelvisBoneWrapper = NewObject<UIKBoneWrapper>(GetOwningActor());
	PelvisBoneWrapper->Initialize(PelvisBone);

	// TraceData Create
	LeftTraceDataWrapper = NewObject<UHumanoidIKTraceData_Wrapper>(GetOwningActor());
	RightTraceDataWrapper = NewObject<UHumanoidIKTraceData_Wrapper>(GetOwningActor());

	// ArmChain Initialize
	LeftArmChainWrapper = NewObject<URangeLimitedIKChainWrapper>(GetOwningActor());
	LeftArmChainWrapper->Initialize(LeftArmChain);
	RightArmChainWrapper = NewObject<URangeLimitedIKChainWrapper>(GetOwningActor());
	RightArmChainWrapper->Initialize(RightArmChain);
}


void UFullbodyAnimInstance::NativeUpdateAnimation(float DeltaTimeX)
{
	Super::NativeUpdateAnimation(DeltaTimeX);
}


void UFullbodyAnimInstance::SetLegTransform(const FTransform InLeftFootTransform, const FTransform InRightFootTransform)
{
	IKLeftLegTarget = InLeftFootTransform;
	IKRightLegTarget = InRightFootTransform;
}


void UFullbodyAnimInstance::SetLegOntoTarget(const bool InIKLeftFootEnable, const bool InIKRightFootEnable)
{
	LeftFootIKMode = InIKLeftFootEnable ? EHumanoidLegIKMode::IK_Human_Leg_WorldLocation : EHumanoidLegIKMode::IK_Human_Leg_Locomotion;
	RightFootIKMode = InIKRightFootEnable ? EHumanoidLegIKMode::IK_Human_Leg_WorldLocation : EHumanoidLegIKMode::IK_Human_Leg_Locomotion;
}


void UFullbodyAnimInstance::SetHandTransform(const FTransform InLeftHandTransform, const FTransform InRightHandTransform)
{
	SetIKHandTarget(InLeftHandTransform, IKLeftHandTarget, IKLeftHandTargetInterpolated);
	SetIKHandTarget(InRightHandTransform, IKRightHandTarget, IKRightHandTargetInterpolated);
}


void UFullbodyAnimInstance::SetIKHandTarget(const FTransform InTransform, FTransform& OutIKTarget, FTransform& OutIKTargetInterpolated)
{
	OutIKTarget = InTransform;
	const FVector Current = OutIKTargetInterpolated.GetLocation();
	const FVector Target = InTransform.GetLocation();
	const FVector Value = UKismetMathLibrary::VInterpTo(Current, Target, GetWorld()->DeltaTimeSeconds, IKTargetInterpolationSpeed);

	FTransform Result = InTransform;
	Result.SetLocation(Value);
	Result.SetRotation(OutIKTarget.GetRotation());
	OutIKTargetInterpolated = Result;
}


#pragma region Torso
void UFullbodyAnimInstance::SetArmTorsoIKMode(const bool InIKLeftHandOntoTarget, const bool InIKRightHandOntoTarget)
{
	bIKLeftHandEnabled = InIKLeftHandOntoTarget;
	bIKRightHandEnabled = InIKRightHandOntoTarget;
	SetArmTorsoIKMode();
}


void UFullbodyAnimInstance::SetArmTorsoIKMode()
{
	if (bIKLeftHandEnabled && bIKRightHandEnabled)
	{
		ArmTorsoIKMode = EHumanoidArmTorsoIKMode::IK_Human_ArmTorso_BothArms;
		LeftArmIKAlpha = MAX;
		RightArmIKAlpha = MAX;
	}
	else if (bIKLeftHandEnabled && !bIKRightHandEnabled)
	{
		ArmTorsoIKMode = EHumanoidArmTorsoIKMode::IK_Human_ArmTorso_LeftArmOnly;
		LeftArmIKAlpha = MAX;
		RightArmIKAlpha = MIN;
	}
	else if (!bIKLeftHandEnabled && bIKRightHandEnabled)
	{
		ArmTorsoIKMode = EHumanoidArmTorsoIKMode::IK_Human_ArmTorso_RightArmOnly;
		LeftArmIKAlpha = MIN;
		RightArmIKAlpha = MAX;
	}
	else
	{
		ArmTorsoIKMode = EHumanoidArmTorsoIKMode::IK_Human_ArmTorso_Disabled;
		LeftArmIKAlpha = MIN;
		RightArmIKAlpha = MIN;
	}
}
#pragma endregion
