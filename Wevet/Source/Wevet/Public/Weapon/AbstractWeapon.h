// Copyright © 2018 wevet works All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "AbstractWeapon.generated.h"

class ACharacterBase;

UCLASS(ABSTRACT)
class WEVET_API AAbstractWeapon : public AActor
{
	GENERATED_BODY()
	
public:	
	AAbstractWeapon(const FObjectInitializer& ObjectInitializer);

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason);

protected:
	TWeakObjectPtr<ACharacterBase> CharacterPtr;

public:
	virtual void Initialize(ACharacterBase* const NewCharacterOwner);
	virtual void ResetCharacterOwner();

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Components, meta = (AllowPrivateAccess = "true"))
	class USceneComponent* SceneComponent;

	TArray<class AActor*> IgnoreActors;

public:
	FORCEINLINE class USceneComponent* GetSceneComponent() const { return SceneComponent; }

	UFUNCTION(BlueprintCallable, Category = "AbstractWeapon|Function")
	ACharacterBase* GetPointer() const;

	UFUNCTION(BlueprintCallable, Category = "AbstractWeapon|Function")
	APawn* GetPawnOwner() const;
};
