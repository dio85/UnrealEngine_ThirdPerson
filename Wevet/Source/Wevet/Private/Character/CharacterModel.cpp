// Copyright 2018 wevet works All Rights Reserved.

#include "Character/CharacterModel.h"
#include "Wevet.h"


UCharacterModel::UCharacterModel(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer),
	Health(100), 
	MaxHealth(100), 
	Attack(100), 
	Defence(100), 
	Wisdom(60), 
	bDie(false)
{
	//
}


void UCharacterModel::SetHealth(const int32 NewHealth)
{
	Health = FMath::Clamp<int32>(NewHealth, INT_ZERO, MaxHealth);
}


void UCharacterModel::SetMaxHealth(const int32 NewMaxHealth)
{
	MaxHealth = NewMaxHealth;
}


void UCharacterModel::SetAttack(const int32 NewAttack)
{
	Attack = NewAttack;
}


void UCharacterModel::SetDefence(const int32 NewDefence)
{
	Defence = NewDefence;
}


void UCharacterModel::SetWisdom(const int32 NewWisdom)
{
	Wisdom = NewWisdom;
}


void UCharacterModel::DoTakeDamage(const int32 InDamage)
{
	const int32 Value = GetHealth();
	SetHealth(Value - InDamage);
}


void UCharacterModel::DoRecover(const int32 AddHealth)
{
	Health += AddHealth;
	Health = FMath::Clamp<int32>(Health, INT_ZERO, MaxHealth);
}


void UCharacterModel::DoDie()
{
	bDie = true;
}


void UCharacterModel::DoAlive()
{
	const float Value = ((float)MaxHealth * QUART_WEIGHT);
	SetHealth((int)FMath::Abs(Value));
	bDie = false;
}

