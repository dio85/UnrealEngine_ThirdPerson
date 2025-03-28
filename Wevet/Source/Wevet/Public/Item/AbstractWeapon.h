// Copyright 2018 wevet works All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "WorldItem.h"
#include "Structs/BaseItem.h"
#include "Interface/WeaponInstigator.h"
#include "Interface/DamageTypeInstigator.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/BoxComponent.h"
#include "Trigger/BulletBase.h"
#include "WevetTypes.h"
#include "AbstractWeapon.generated.h"

class APawn;


DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FWeaponActionDelegate, bool const, InFiredAction);


UCLASS(ABSTRACT)
class WEVET_API AAbstractWeapon : public AWorldItem, public IWeaponInstigator, public IDamageTypeInstigator
{
	GENERATED_BODY()
	
public:	
	AAbstractWeapon(const FObjectInitializer& ObjectInitializer);
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason);

protected:
	virtual void BeginPlay() override;

public:
#pragma region Interface
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "AbstractWeapon|WeaponInstigator")
	void DoFirePressed();
	virtual void DoFirePressed_Implementation() override;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "AbstractWeapon|WeaponInstigator")
	void DoFireRelease();
	virtual void DoFireRelease_Implementation() override;	

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "AbstractWeapon|WeaponInstigator")
	void DoReload();
	virtual void DoReload_Implementation() override;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "AbstractWeapon|WeaponInstigator")
	void DoReplenishment(const int32 InAddAmmo);
	virtual void DoReplenishment_Implementation(const int32 InAddAmmo) override;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "AbstractWeapon|WeaponInstigator")
	bool CanStrike() const;
	virtual bool CanStrike_Implementation() const override;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "AbstractWeapon|DamageTypeInstigator")
	EGiveDamageType GetGiveDamageType() const;
	virtual EGiveDamageType GetGiveDamageType_Implementation() const override;

	virtual void Take_Implementation(APawn* NewCharacter) override;
	virtual void Release_Implementation(APawn* NewCharacter) override;
	virtual void SpawnToWorld_Implementation() override;
	virtual EItemType GetItemType_Implementation() const override;
#pragma endregion

public:
	virtual void Initialize(APawn* const NewCharacterOwner) override;

protected:
	virtual void BeginOverlapRecieve(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) override;
	virtual	void EndOverlapRecieve(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex) override;
	virtual	void AddDelegate() override;
	virtual	void RemoveDelegate() override;
	virtual	void PrepareDestroy() override;
	virtual void OverlapActor(AActor* OtherActor) override;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Components, meta = (AllowPrivateAccess = "true"))
	class USkeletalMeshComponent* SkeletalMeshComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Components, meta = (AllowPrivateAccess = "true"))
	class UBoxComponent* CollisionComponent;


public:
	UFUNCTION(BlueprintCallable, Category = "AbstractWeapon|Function")
	ACharacterBase* GetPointer() const;
	UFUNCTION(BlueprintCallable, Category = "AbstractWeapon|Function")
	APawn* GetPawnOwner() const;

	void ReloadBulletParams();
	void SetOwnerNoSeeMesh(const bool NewOwnerNoSee);
	void ResetCharacterOwner();
	void SetEquip(const bool InEquip);
	void CopyWeaponItemInfo(const FWeaponItemInfo* RefWeaponItemInfo);

	// AddDelegate
	UPROPERTY(BlueprintAssignable)
	FWeaponActionDelegate WeaponActionDelegate;


public:
	FORCEINLINE class USkeletalMeshComponent* GetSkeletalMeshComponent() const { return SkeletalMeshComponent; }

	struct FWeaponItemInfo GetWeaponItemInfo() const { return WeaponItemInfo; }

	EWeaponItemType GetWeaponItemType() const { return WeaponItemInfo.WeaponItemType; }

	FORCEINLINE int32 GetPriority() const { return WeaponItemInfo.Priority; }

	FORCEINLINE bool WasEquip() const { return bEquip; }	
	FORCEINLINE bool WasReload() const { return bReload; }
	FORCEINLINE float GetTraceDistance() const { return WeaponItemInfo.TraceDistance; }

	FORCEINLINE bool WasEmpty() const 
	{
		return (WeaponItemInfo.EmptyCurrentAmmo()) && (WeaponItemInfo.EmptyAmmo());
	}

	FORCEINLINE bool WasSameWeaponType(EWeaponItemType InWeaponItemType) const 
	{
		return WeaponItemInfo.WeaponItemType == InWeaponItemType; 
	}

	FORCEINLINE FTransform GetMuzzleTransform() const 
	{
		check(SkeletalMeshComponent);
		return SkeletalMeshComponent->GetSocketTransform(MuzzleSocketName); 
	}

	FORCEINLINE FTransform GetGripTransform() const 
	{
		check(SkeletalMeshComponent);
		return SkeletalMeshComponent->GetSocketTransform(GripSocketName); 
	}

	FORCEINLINE FTransform GetGripTransform(const FName InGripSocketName) const
	{
		check(SkeletalMeshComponent);
		return SkeletalMeshComponent->GetSocketTransform(InGripSocketName);
	}

public:
	virtual bool CanReleaseItem() const
	{
		return true;
	}

	UFUNCTION(BlueprintCallable, Category = "AbstractWeapon|Function")
	virtual void ClearCollisionApply()
	{

	}


protected:
	// Apply to Blueprint
	UFUNCTION(BlueprintCallable, Category = "AbstractWeapon|Function")
	virtual void OnFirePressInternal();

	virtual void TakeDamageOuter(const FHitResult& HitResult);


protected:
	void SetReload(const bool InReload)
	{
		bReload = InReload;
	}
	void SetFired(const bool InCanFire)
	{
		bCanFire = InCanFire;
	}


	void OnHitTriggerAction(const FTransform InTransform, const FHitResult HitResult, const FVector ImpactLocation, const bool bHitSuccess);
	void PlayEffect(const FHitResult& HitResult, USceneComponent* const InComponent, bool& OutHitActor);
	void CreateDamage(AActor* OtherActor, const float TotalDamage, const FHitResult& HitResult);


protected:
#pragma region Uproperty
	TWeakObjectPtr<class ACharacterBase> CharacterPtr;
	FTimerHandle ReloadTimerHandle;
	EGiveDamageType GiveDamageType;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "AbstractWeapon|Variable")
	FWeaponItemInfo WeaponItemInfo;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "AbstractWeapon|Variable")
	FName MuzzleSocketName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "AbstractWeapon|Variable")
	FName GripSocketName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "AbstractWeapon|Variable")
	float BulletDuration;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AbstractWeapon|Variable")
	bool bEquip;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AbstractWeapon|Variable")
	bool bReload;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AbstractWeapon|Variable")
	bool bCanFire;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "AbstractWeapon|Asset")
	class USoundBase* FireSound;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "AbstractWeapon|Asset")
	class USoundBase* ImpactSound;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "AbstractWeapon|Asset")
	class UParticleSystem* FlashEmitterTemplate;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "AbstractWeapon|Asset")
	class UParticleSystem* ImpactEmitterTemplate;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "AbstractWeapon|Asset")
	TSubclassOf<class ABulletBase> BulletsTemplate;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "AbstractWeapon|Asset")
	TSubclassOf<class UDamageType> BaseDamageType;
#pragma endregion


};
