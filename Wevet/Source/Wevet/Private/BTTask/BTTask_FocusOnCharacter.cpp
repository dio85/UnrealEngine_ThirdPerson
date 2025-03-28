// Copyright 2018 wevet works All Rights Reserved.

#include "BTTask/BTTask_FocusOnCharacter.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/Blackboard/BlackboardKeyAllTypes.h"
#include "AI/AIControllerBase.h"
#include "Character/CharacterBase.h"
#include "Engine.h"

UBTTask_FocusOnCharacter::UBTTask_FocusOnCharacter() : Super()
{
	NodeName = TEXT("BTTask_FocusOnCharacter");
}

EBTNodeResult::Type UBTTask_FocusOnCharacter::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	if (AAIControllerBase* AIController = Cast<AAIControllerBase>(OwnerComp.GetAIOwner()))
	{
		ICombatInstigator* Interface = Cast<ICombatInstigator>(AIController->GetPawn());
		if (!Interface)
		{
			return EBTNodeResult::Failed;
		}

		if (APawn* Pawn = Cast<APawn>(ICombatInstigator::Execute_GetTarget(Interface->_getUObject())))
		{
			AIController->SetFocus(Pawn->GetController());
		}
	}
	return EBTNodeResult::Succeeded;
}

