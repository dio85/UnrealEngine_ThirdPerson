// Copyright 2018 wevet works All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "WevetTypes.h"
#include "CharacterInventoryComponent.generated.h"

class AAbstractWeapon;
class AAbstractItem;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class WEVET_API UCharacterInventoryComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UCharacterInventoryComponent(const FObjectInitializer& ObjectInitializer);

protected:
	virtual void BeginPlay() override;

public:	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

private:
	TArray<class AAbstractWeapon*> WeaponArray;
	TArray<class AAbstractItem*> ItemArray;


public:
	void AddWeaponInventory(AAbstractWeapon* const NewWeaponBase);
	void RemoveWeaponInventory(AAbstractWeapon* const WeaponBase);
	void ClearWeaponInventory();
	const TArray<class AAbstractWeapon*>& GetWeaponInventory();

	AAbstractWeapon* GetAvailableWeapon();
	AAbstractWeapon* GetUnEquipWeapon() const;
	AAbstractWeapon* GetReleaseWeaponByIndex(const int32 InIndex);
	AAbstractWeapon* GetWeaponByIndex(const int32 InIndex);

	AAbstractWeapon* FindByWeapon(const EWeaponItemType WeaponItemType) const;

	void FindHighPriorityWeapon(bool &OutFoundWeapon, const int32 InPriorityNumber = 1);
	AAbstractWeapon* GetNakedWeapon() const;

	int32 GetWeaponInventoryCount() const
	{
		return WeaponArray.Num();
	}

public:
	void AddItemInventory(AAbstractItem* const NewItemBase);
	void RemoveItemInventory(AAbstractItem* const ItemBase);
	void ClearItemInventory();
	const TArray<class AAbstractItem*>& GetItemInventory();


public:
	void ReleaseAllWeaponInventory();
	void ReleaseAllItemInventory();
	bool EmptyWeaponInventory() const;
	bool EmptyItemInventory() const;


public:
	void SetOwnerNoSeeMesh(const bool NewOwnerNoSee);

};
