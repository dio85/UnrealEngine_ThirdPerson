// Copyright 2018 wevet works All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "WorldItem.h"
#include "Structs/BaseItem.h"
#include "Components/SphereComponent.h"
#include "Components/WidgetComponent.h"
#include "Components/StaticMeshComponent.h"
#include "AbstractItem.generated.h"

class APawn;

UCLASS()
class WEVET_API AAbstractItem : public AWorldItem
{
	GENERATED_BODY()
	
public:	
	AAbstractItem(const FObjectInitializer& ObjectInitializer);
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason);

protected:
	virtual void BeginPlay() override;

public:
	virtual void Release_Implementation(APawn* NewCharacter) override;
	virtual void SpawnToWorld_Implementation() override;
	virtual EItemType GetItemType_Implementation() const override;


protected:
	virtual void BeginOverlapRecieve(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) override;
	virtual void EndOverlapRecieve(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex) override;
	virtual void AddDelegate() override;
	virtual void RemoveDelegate() override;
	virtual void OverlapActor(AActor* OtherActor) override;


public:
	FORCEINLINE class UStaticMeshComponent* GetStaticMeshComponent() const { return StaticMeshComponent; }
	struct FWeaponAmmoInfo GetWeaponAmmoInfo() const { return WeaponAmmoInfo; }
	FORCEINLINE EWeaponItemType GetWeaponItemType() const { return WeaponAmmoInfo.WeaponItemType; }
	FORCEINLINE int32 GetReplenishmentAmmo() const { return WeaponAmmoInfo.Ammo; }


protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "AbstractItem|Variable")
	FWeaponAmmoInfo WeaponAmmoInfo;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Components, meta = (AllowPrivateAccess = "true"))
	class UStaticMeshComponent* StaticMeshComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Components, meta = (AllowPrivateAccess = "true"))
	class USphereComponent* CollisionComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Components, meta = (AllowPrivateAccess = "true"))
	class UWidgetComponent* WidgetComponent;

	class AActor* Target;
};
