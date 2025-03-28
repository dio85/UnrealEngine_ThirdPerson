// Copyright 2018 wevet works All Rights Reserved.


#include "AnimNotify/ReloadAnimNotify.h"
#include "Character/CharacterBase.h"

UReloadAnimNotify::UReloadAnimNotify(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
}


void UReloadAnimNotify::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	ICombatInstigator* Interface = Cast<ICombatInstigator>(MeshComp->GetOwner());
	if (Interface)
	{
		if (AAbstractWeapon* Weapon = ICombatInstigator::Execute_GetCurrentWeapon(Interface->_getUObject()))
		{
			Weapon->ReloadBulletParams();
		}
	}
	Super::Notify(MeshComp, Animation);
}
