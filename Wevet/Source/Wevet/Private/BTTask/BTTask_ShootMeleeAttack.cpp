// Copyright 2018 wevet works All Rights Reserved.


#include "BTTask/BTTask_ShootMeleeAttack.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/Blackboard/BlackboardKeyAllTypes.h"
#include "AI/AIControllerBase.h"
#include "Character/CharacterBase.h"
#include "Engine.h"

UBTTask_ShootMeleeAttack::UBTTask_ShootMeleeAttack() : Super()
{
	MinBulletInterval = 0.01f;
	MaxBulletInterval = 0.1f;
	MeleeAttackTimeOut = 8.f;
	NodeName = TEXT("BTTask_ShootMeleeAttack");
	BlackboardKey.SelectedKeyName = FName(TEXT("Target"));
}

EBTNodeResult::Type UBTTask_ShootMeleeAttack::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	if (AAIControllerBase* AIController = Cast<AAIControllerBase>(OwnerComp.GetAIOwner()))
	{
		// Do Something
		UE_LOG(LogWevetClient, Log, TEXT("Initialize.. %s"), *FString(__FUNCTION__));
		return EBTNodeResult::Succeeded;

	}
	return EBTNodeResult::Failed;
}

EBTNodeResult::Type UBTTask_ShootMeleeAttack::AbortTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	UE_LOG(LogWevetClient, Log, TEXT("UBTTask_ShootMeleeAttack::AbortTask"));
	return Super::AbortTask(OwnerComp, NodeMemory);
}

void UBTTask_ShootMeleeAttack::OnTaskFinished(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, EBTNodeResult::Type TaskResult)
{
	UE_LOG(LogWevetClient, Log, TEXT("UBTTask_ShootMeleeAttack::OnTaskFinished"));
	Super::OnTaskFinished(OwnerComp, NodeMemory, TaskResult);
}

void UBTTask_ShootMeleeAttack::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	UE_LOG(LogWevetClient, Log, TEXT("UBTTask_ShootMeleeAttack::TickTask"));
	Super::TickTask(OwnerComp, NodeMemory, DeltaSeconds);
}
