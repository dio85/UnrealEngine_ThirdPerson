#pragma once

#include "CoreMinimal.h"
#include "IKTypes.h"
#include "HumanoidIK.h"
#include "BoneControllers/AnimNode_SkeletalControlBase.h"
#include "Engine/SkeletalMeshSocket.h"
#include "AnimNode_HumanoidPelvisHeightAdjustment.generated.h"

USTRUCT()
struct RTIK_API FAnimNode_HumanoidPelvisHeightAdjustment : public FAnimNode_SkeletalControlBase
{
	GENERATED_USTRUCT_BODY()

protected:	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Bones, meta = (PinShownByDefault))
	UHumanoidLegChain_Wrapper* LeftLeg;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Bones, meta = (PinShownByDefault))
	UHumanoidLegChain_Wrapper* RightLeg;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Trace, meta = (PinShownByDefault))
	UHumanoidIKTraceData_Wrapper* LeftLegTraceData;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Trace, meta = (PinShownByDefault))
	UHumanoidIKTraceData_Wrapper* RightLegTraceData;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Bones, meta = (PinShownByDefault))
	UIKBoneWrapper* PelvisBone;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Settings, meta = (PinHiddenByDefault))
	float PelvisAdjustVelocity;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Settings, meta = (PinShownByDefault))
	float MaxPelvisAdjustSize;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Settings)
	bool bEnableDebugDraw;

public:
	FAnimNode_HumanoidPelvisHeightAdjustment() : Super()
	{
		PelvisAdjustVelocity = 150.0f;
		MaxPelvisAdjustSize = 50.0f;
		LastPelvisOffset = FVector::ZeroVector;
		bEnableDebugDraw = false;
		DeltaTime = 0.0f;
	}

	virtual void UpdateInternal(const FAnimationUpdateContext& Context) override
	{
		DeltaTime = Context.GetDeltaTime();
		Super::UpdateInternal(Context);
	}

	virtual bool IsValidToEvaluate(const USkeleton* Skeleton, const FBoneContainer& RequiredBones) override
	{
		if (LeftLeg == nullptr || RightLeg == nullptr || PelvisBone == nullptr)
		{
			return false;
		}	

		if (!LeftLeg->InitIfInvalid(RequiredBones))
		{
#if ENABLE_IK_DEBUG_VERBOSE
			UE_LOG(LogNIK, Warning, TEXT("Could not initialize LeftLeg : %s"), *FString(__FUNCTION__));
#endif
			return false;
		}

		if (!RightLeg->InitIfInvalid(RequiredBones))
		{
#if ENABLE_IK_DEBUG_VERBOSE
			UE_LOG(LogNIK, Warning, TEXT("Could not initialize RightLeg : %s"), *FString(__FUNCTION__));
#endif
			return false;
		}

		if (!PelvisBone->InitIfInvalid(RequiredBones))
		{
#if ENABLE_IK_DEBUG_VERBOSE
			UE_LOG(LogNIK, Warning, TEXT("Could not initialize PelvisBone : %s"), *FString(__FUNCTION__));
#endif
			return false;
		}
		return LeftLeg->InitIfInvalid(RequiredBones) && RightLeg->InitIfInvalid(RequiredBones) && PelvisBone->InitIfInvalid(RequiredBones);
	}

	virtual void InitializeBoneReferences(const FBoneContainer& RequiredBones) override
	{
		if (LeftLeg == nullptr || RightLeg == nullptr || PelvisBone == nullptr)
		{
			return;
		}

		if (!RightLeg->InitBoneReferences(RequiredBones))
		{
#if ENABLE_IK_DEBUG_VERBOSE
			UE_LOG(LogNIK, Warning, TEXT("Could not initialize RightLeg : %s"), *FString(__FUNCTION__));
#endif
		}

		if (!LeftLeg->InitBoneReferences(RequiredBones))
		{
#if ENABLE_IK_DEBUG_VERBOSE
			UE_LOG(LogNIK, Warning, TEXT("Could not initialize LeftLeg : %s"), *FString(__FUNCTION__));
#endif
		}

		if (!PelvisBone->Init(RequiredBones))
		{
#if ENABLE_IK_DEBUG_VERBOSE
			UE_LOG(LogNIK, Warning, TEXT("Could not initialize PelvisBone : %s"), *FString(__FUNCTION__));
#endif
		}
		Super::InitializeBoneReferences(RequiredBones);
	}

	virtual void EvaluateSkeletalControl_AnyThread(FComponentSpacePoseContext& Output, TArray<FBoneTransform>& OutBoneTransforms) override;

protected:
	float DeltaTime;
	FVector LastPelvisOffset;
};
