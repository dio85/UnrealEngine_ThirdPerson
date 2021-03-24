
#pragma once

#include "AnimGraphNode_SkeletalControlBase.h"
#include "AnimNode_HumanoidLegIKKneeCorrection.h"
#include "AnimGraphNode_HumanoidLegIKKneeCorrection.generated.h"

UCLASS()
class RTIKEDITOR_API UAnimGraphNode_HumanoidLegIKKneeCorrection : public UAnimGraphNode_SkeletalControlBase
{
	GENERATED_BODY()
	
public:
	FText GetNodeTitle(ENodeTitleType::Type TitleType) const override
	{
		return FText::FromString(FString("Humanoid LegIK KneeCorrection"));
	}

	FLinearColor GetNodeTitleColor() const override
	{
		return FLinearColor(0, 1, 1, 1);
	}
	
	FString GetNodeCategory() const override
	{
		return FString("IK Nodes");
	}
	
	virtual const FAnimNode_SkeletalControlBase* GetNode() const override
	{
		return &Node; 
	}

protected:
	virtual FText GetControllerDescription() const
	{
		return FText::FromString(FString("Corrects Knee Angle After IK"));
	}

protected:
	UPROPERTY(EditAnywhere, Category = Settings)
	FAnimNode_HumanoidLegIKKneeCorrection Node;

};