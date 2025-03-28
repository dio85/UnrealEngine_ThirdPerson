// Copyright 2018 wevet works All Rights Reserved.


#include "Lib/WevetBlueprintFunctionLibrary.h"
#include "Components/PrimitiveComponent.h"
#include "Components/MeshComponent.h"
#include "LoadingScreen.h"
#include "Blueprint/UserWidget.h"
#include "Engine/World.h"
#include "EngineUtils.h"
#include "AI/WayPointBase.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/KismetMathLibrary.h"
#include "SaveGame/WevetGameInstance.h"


void UWevetBlueprintFunctionLibrary::PlayLoadingScreen(bool bPlayUntilStopped, float PlayTime)
{
	ILoadingScreenModule& LoadingScreenModule = ILoadingScreenModule::Get();
	LoadingScreenModule.StartInGameLoadingScreen(bPlayUntilStopped, PlayTime);
}


void UWevetBlueprintFunctionLibrary::StopLoadingScreen()
{
	ILoadingScreenModule& LoadingScreenModule = ILoadingScreenModule::Get();
	LoadingScreenModule.StopInGameLoadingScreen();
}


bool UWevetBlueprintFunctionLibrary::IsInEditor()
{
	return GIsEditor;
}


void UWevetBlueprintFunctionLibrary::CreateDynamicMaterialInstance(UPrimitiveComponent* PrimitiveComponent, TArray<UMaterialInstanceDynamic*>& OutMaterialArray)
{
	const int32 MaterialNum = PrimitiveComponent->GetNumMaterials();
	for (int Index = 0; Index < MaterialNum; ++Index)
	{
		if (UMaterialInstanceDynamic * Instance = PrimitiveComponent->CreateDynamicMaterialInstance(
			Index, PrimitiveComponent->GetMaterial(Index)))
		{
			OutMaterialArray.Emplace(Instance);
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("null ptr material instance"));
		}
	}
}


void UWevetBlueprintFunctionLibrary::OrderByDistance(AActor* PlayerActor, TArray<AActor*>InArray, TArray<AActor*>& OutArray)
{
	InArray.Sort([PlayerActor](const AActor& A, const AActor& B)
	{
		float DistanceA = A.GetDistanceTo(PlayerActor);
		float DistanceB = B.GetDistanceTo(PlayerActor);
		return DistanceA > DistanceB;
	});
	OutArray = InArray;
}


AActor* UWevetBlueprintFunctionLibrary::CloneActor(AActor* InputActor)
{
	UWorld* World = InputActor->GetWorld();
	FActorSpawnParameters params;
	params.Template = InputActor;

	UClass* ItemClass = InputActor->GetClass();
	AActor* const SpawnedActor = World->SpawnActor<AActor>(ItemClass, params);
	return SpawnedActor;
}


float UWevetBlueprintFunctionLibrary::GetMeanValue(TArray<float> Values)
{
	int32 Size = Values.Num();
	float Sum = 0;
	for (int i = 0; i < Size; ++i)
	{
		Sum += Values[i];
	}
	return Sum / Size;
}


void UWevetBlueprintFunctionLibrary::CircleSpawnPoints(const int32 InSpawnCount, const float InRadius, const FVector InRelativeLocation, TArray<FVector>& OutPointArray)
{
	float AngleDiff = 360.f / (float)InSpawnCount;
	for (int i = 0; i < InSpawnCount; ++i)
	{
		FVector Position = InRelativeLocation;
		float Ang = FMath::DegreesToRadians(90 - AngleDiff * i);
		Position.X += InRadius * FMath::Cos(Ang);
		Position.Y += InRadius * FMath::Sin(Ang);
		OutPointArray.Add(Position);
	}
}


void UWevetBlueprintFunctionLibrary::WorldPawnIterator(APawn* Owner, const float InDistance, TArray<class APawn*>& OutPawnArray)
{
	for (TActorIterator<APawn> It(Owner->GetWorld()); It; ++It)
	{
		APawn* Pawn = Cast<APawn>(*It);
		if (Pawn == nullptr || (Pawn && Pawn == Owner))
		{
			continue;
		}

		const float Dist = (Owner->GetActorLocation() - Pawn->GetActorLocation()).Size();
		if (Dist >= InDistance)
		{
			continue;
		}
		OutPawnArray.AddUnique(Pawn);
	}
}


void UWevetBlueprintFunctionLibrary::GetWorldWayPointsArray(AActor* Owner, const float InDistance, TArray<class AWayPointBase*>& OutWayPointArray)
{
	for (TActorIterator<AWayPointBase> ActorIterator(Owner->GetWorld()); ActorIterator; ++ActorIterator)
	{
		if (AWayPointBase* WayPoint = *ActorIterator)
		{
			if (WayPoint == nullptr)
			{
				continue;
			}

			const float Dist = (Owner->GetActorLocation() - WayPoint->GetActorLocation()).Size();
			if (Dist >= InDistance)
			{
				continue;
			}
			OutWayPointArray.Emplace(WayPoint);
		}
	}

}


void UWevetBlueprintFunctionLibrary::DrawDebugString(AActor* const Owner, const FString LogString, FLinearColor DebugColor, float Duration)
{
	if (!Owner->IsValidLowLevel())
	{
		return;
	}
#if WITH_EDITOR
	UKismetSystemLibrary::DrawDebugString(Owner->GetWorld(), Owner->GetActorLocation(), *LogString, nullptr, DebugColor, Duration);
#endif
}


/*
*	ゲームインスタンスがスタンドアロン(ネットワーキングなし)かどうかを返す
*/
const bool UWevetBlueprintFunctionLibrary::IsNetworked(AActor* const Owner)
{
	if (!Owner)
	{
		return false;
	}
	return UKismetSystemLibrary::IsStandalone(Owner->GetWorld());
}


void UWevetBlueprintFunctionLibrary::SetDepthValue(const ECustomDepthType InCustomDepthType, UMeshComponent* MeshComponent)
{
	if (MeshComponent)
	{
		MeshComponent->SetCustomDepthStencilValue((int32)InCustomDepthType);
		MeshComponent->SetRenderCustomDepth((InCustomDepthType == ECustomDepthType::None) ? false : true);
	}
}


const FString UWevetBlueprintFunctionLibrary::NormalizeFileName(const char* String)
{
	return NormalizeFileName(FString(UTF8_TO_TCHAR(String)));
}


const FString UWevetBlueprintFunctionLibrary::NormalizeFileName(const FString& String)
{
	FString Ret = String;
	FText ErrorText;

	if (!FName::IsValidXName(*Ret, INVALID_OBJECTNAME_CHARACTERS INVALID_LONGPACKAGE_CHARACTERS, &ErrorText))
	{
		FString InString = INVALID_OBJECTNAME_CHARACTERS;
		InString += INVALID_LONGPACKAGE_CHARACTERS;

		const TArray<TCHAR> CharArray = InString.GetCharArray();
		for (int i = 0; i < CharArray.Num(); ++i)
		{
			FString Template;
			Template.AppendChar(CharArray[i]);
			Ret = Ret.Replace(Template.GetCharArray().GetData(), TEXT("_"));
		}
	}
	return Ret;
}


TArray<FLinearColor> UWevetBlueprintFunctionLibrary::GetInverseMatrix(const FTransform& InTransform)
{
	TArray<FLinearColor> Ret;
	Ret.SetNum(4);

	//UKismetMathLibrary::MatricWithSca
	const FMatrix InverseMatrix = InTransform.ToMatrixWithScale().Inverse();
	for (int i = 0; i < 4; i++) 
	{
		FLinearColor color;
		color.R = InverseMatrix.M[i][0];
		color.G = InverseMatrix.M[i][1];
		color.B = InverseMatrix.M[i][2];
		color.A = InverseMatrix.M[i][3];
		Ret[i] = color;
	}
	return Ret;
}


/*
*	WLIB = unreal.WevetBlueprintFunctionLibrary()
*	WLIB.py_to_cpp("LogOutput is Success!")
*/
void UWevetBlueprintFunctionLibrary::PyToCpp(const FString String)
{
	UE_LOG(LogWevetPython, Log, TEXT("(%s)"), *String);
}

