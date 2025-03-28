// Copyright 2018 wevet works All Rights Reserved.

#include "Component/CharacterInventoryComponent.h"
#include "Item/AbstractWeapon.h"
#include "Item/AbstractItem.h"
#include "Wevet.h"
#include "WevetExtension.h"

UCharacterInventoryComponent::UCharacterInventoryComponent(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	PrimaryComponentTick.bCanEverTick = false;
	WeaponArray.Reset(0);
	ItemArray.Reset(0);
}


void UCharacterInventoryComponent::BeginPlay()
{
	Super::BeginPlay();
}


void UCharacterInventoryComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}


void UCharacterInventoryComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	WeaponArray.Reset(0);
	ItemArray.Reset(0);
	Super::EndPlay(EndPlayReason);
}


#pragma region Weapon
void UCharacterInventoryComponent::AddWeaponInventory(AAbstractWeapon* const NewWeaponBase)
{
	if (!WeaponArray.Contains(NewWeaponBase))
	{
		WeaponArray.Add(NewWeaponBase);
		WeaponArray.Shrink();
	}
}


void UCharacterInventoryComponent::RemoveWeaponInventory(AAbstractWeapon* const WeaponBase)
{
	if (WeaponArray.Contains(WeaponBase))
	{
		WeaponArray.Remove(WeaponBase);
	}
}


void UCharacterInventoryComponent::ClearWeaponInventory()
{
	WeaponArray.Empty();
}


const TArray<class AAbstractWeapon*>& UCharacterInventoryComponent::GetWeaponInventory()
{
	return WeaponArray;
}


AAbstractWeapon* UCharacterInventoryComponent::GetAvailableWeapon()
{
	if (AAbstractWeapon* Weapon = GetUnEquipWeapon())
	{
		if (!Weapon->WasEmpty())
		{
			return Weapon;
		}
	}
	return nullptr;
}


AAbstractWeapon* UCharacterInventoryComponent::GetUnEquipWeapon() const
{
	for (AAbstractWeapon* Weapon : WeaponArray)
	{
		if (!Weapon)
		{
			continue;
		}
		if (!Weapon->WasEquip())
		{
			return Weapon;
		}
	}
	return nullptr;
}


AAbstractWeapon* UCharacterInventoryComponent::FindByWeapon(const EWeaponItemType WeaponItemType) const
{
	for (AAbstractWeapon* Weapon : WeaponArray)
	{
		if (Weapon && Weapon->WasSameWeaponType(WeaponItemType))
		{
			return Weapon;
		}
	}
	return nullptr;
}


void UCharacterInventoryComponent::FindHighPriorityWeapon(bool& OutFoundWeapon, const int32 InPriorityNumber)
{
	for (AAbstractWeapon* Weapon : WeaponArray)
	{
		if (!Weapon)
		{
			continue;
		}
		const bool bResult = (!Weapon->WasEquip() && !Weapon->WasEmpty() && Weapon->GetPriority() >= InPriorityNumber);
		if (bResult)
		{
			OutFoundWeapon = true;
			UE_LOG(LogWevetClient, Log, TEXT("Found : %s, funcName : %s"), *Weapon->GetName(), *FString(__FUNCTION__));
		}
	}
}


AAbstractWeapon* UCharacterInventoryComponent::GetNakedWeapon() const
{
	for (AAbstractWeapon* Weapon : WeaponArray)
	{
		if (!Weapon)
		{
			continue;
		}
		if (!Weapon->CanReleaseItem())
		{
			return Weapon;
		}
	}
	return nullptr;
}


/// <summary>
/// Player Only API
/// </summary>
/// <param name="InIndex"></param>
/// <returns></returns>
AAbstractWeapon* UCharacterInventoryComponent::GetReleaseWeaponByIndex(const int32 InIndex)
{
	for (int Index = 0; Index < WeaponArray.Num(); ++Index)
	{
		if (Index == InIndex)
		{
			continue;
		}
		AAbstractWeapon* Weapon = WeaponArray[Index];
		if (!Weapon || !Weapon->CanReleaseItem())
		{
			continue;
		}
		if (!Weapon->WasEquip())
		{
			return Weapon;
		}
	}
	return nullptr;
}


/// <summary>
/// Player Only API
/// </summary>
/// <param name="InIndex"></param>
/// <returns></returns>
AAbstractWeapon* UCharacterInventoryComponent::GetWeaponByIndex(const int32 InIndex)
{
	if (EmptyWeaponInventory())
	{
		return nullptr;
	}
	else
	{
		if (WeaponArray.Num() < 2)
		{
			return WeaponArray[0];
		}
		else
		{
			for (int Index = 0; Index < WeaponArray.Num(); ++Index)
			{
				if (Index == InIndex)
				{
					AAbstractWeapon* Weapon = WeaponArray[InIndex];
					if (Weapon && Weapon->IsValidLowLevel())
					{
						return Weapon;
					}
				}
			}
		}
	}
	return nullptr;
}


bool UCharacterInventoryComponent::EmptyWeaponInventory() const
{
	return Wevet::ArrayExtension::NullOrEmpty(WeaponArray);
}


void UCharacterInventoryComponent::ReleaseAllWeaponInventory()
{
	for (AAbstractWeapon*& Weapon : WeaponArray)
	{
		if (Weapon && Weapon->IsValidLowLevel())
		{
			Weapon->Destroy();
			Weapon->ConditionalBeginDestroy();
			Weapon = nullptr;
		}
	}
	WeaponArray.Empty();
}
#pragma endregion


#pragma region Item
void UCharacterInventoryComponent::AddItemInventory(AAbstractItem* const NewItemBase)
{
	if (!ItemArray.Contains(NewItemBase))
	{
		ItemArray.Add(NewItemBase);
		ItemArray.Shrink();
	}
}


void UCharacterInventoryComponent::RemoveItemInventory(AAbstractItem* const ItemBase)
{
	if (ItemArray.Contains(ItemBase))
	{
		ItemArray.Remove(ItemBase);
	}
}


void UCharacterInventoryComponent::ClearItemInventory()
{
	ItemArray.Empty();
}


const TArray<class AAbstractItem*>& UCharacterInventoryComponent::GetItemInventory()
{
	return ItemArray;
}


bool UCharacterInventoryComponent::EmptyItemInventory() const
{
	return Wevet::ArrayExtension::NullOrEmpty(ItemArray);
}


void UCharacterInventoryComponent::ReleaseAllItemInventory()
{
	for (AAbstractItem*& Item : ItemArray)
	{
		if (Item && Item->IsValidLowLevel())
		{
			Item->Destroy();
			Item->ConditionalBeginDestroy();
			Item = nullptr;
		}
	}
	ItemArray.Empty();
}
#pragma endregion


void UCharacterInventoryComponent::SetOwnerNoSeeMesh(const bool NewOwnerNoSee)
{
	for (AAbstractWeapon* Weapon : WeaponArray)
	{
		if (Weapon)
		{
			Weapon->SetOwnerNoSeeMesh(NewOwnerNoSee);
		}
	}
}


