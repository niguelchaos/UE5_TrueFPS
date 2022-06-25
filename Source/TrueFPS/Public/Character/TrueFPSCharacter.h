// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "TrueFPSCharacter.generated.h"

// multicast delegate, can bind to it in anim instance - whenever switch weapons, notify anim instance
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(
	FCurrentWeaponChangedDelegate, class AWeapon*, CurrentWeapon, const class AWeapon*, OldWeapon);

UCLASS()
class TRUEFPS_API ATrueFPSCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ATrueFPSCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	//make sure its replicated
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

public:	
	// Called every frame
	// virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components")
	class UCameraComponent* Camera;

protected:
	// Weapon Classes spawned by default
	// no need to edit outside of beginplay - only affected in beginplay
	// array of classes of weapons
	UPROPERTY(EditDefaultsOnly, Category = "Configurations")
	TArray<TSubclassOf<class AWeapon>> DefaultWeapons;

public:
	// array of weapons to replicate
	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Replicated, Category = "State")
	TArray<class AWeapon*> Weapons;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, ReplicatedUsing = OnRep_CurrentWeapon, Category = "State")
	class AWeapon* CurrentWeapon;

	// Called whenever CurrentWeapon is changed
	UPROPERTY(BlueprintAssignable, Category = "Delegates")
	FCurrentWeaponChangedDelegate CurrentWeaponChangedDelegate;

	// current index - keeps track at all times when switching weapons locally
	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category = "State")
	int32 CurrentIndex = 0;

	UFUNCTION(BlueprintCallable, Category = "Character")
	virtual void EquipWeapon(const int32 Index);

protected:
	// can run logic on old and new weapon
	UFUNCTION()
	virtual void OnRep_CurrentWeapon(const class AWeapon* OldWeapon);

	UFUNCTION(Server, Reliable)
	void Server_SetCurrentWeapon(class AWeapon* Weapon);
	virtual void Server_SetCurrentWeapon_Implementation(class AWeapon* Weapon);







	
// Controls
protected:
	virtual void NextWeapon(); 
	virtual void LastWeapon(); 

	
	void MoveForward(const float Value);
	void MoveRight(const float Value);
	void LookUp(const float Value);
	void LookRight(const float Value);
};
