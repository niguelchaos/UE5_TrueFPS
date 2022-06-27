// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "Weapons/Weapon.h"
#include "TrueFPSAnimInstance.generated.h"

/**
 * 
 */
UCLASS()
class TRUEFPS_API UTrueFPSAnimInstance : public UAnimInstance
{
	GENERATED_BODY()
public:
	UTrueFPSAnimInstance();

protected:
	// anim blueprints
	virtual void NativeBeginPlay() override;
	virtual void NativeUpdateAnimation(float DeltaTime) override;

	// since adddynamic is reflection function in order for it to work needs to be ufunction
	UFUNCTION()
	virtual void CurrentWeaponChanged(class AWeapon* NewWeapon, const class AWeapon* OldWeapon);
	
	virtual void SetVars(const float DeltaTime);
	virtual void CalculateWeaponSway(const float DeltaTime);

	// call once everytime equip weapon
	virtual void SetIKTransforms();

public:
	/////////////////////////////////// References for character ///////////////////////////////////
	// control rig for IK
	UPROPERTY(BlueprintReadWrite, Category = "Anim")
	class ATrueFPSCharacter* Character;

	UPROPERTY(BlueprintReadWrite, Category = "Anim")
	class USkeletalMeshComponent* Mesh;

	UPROPERTY(BlueprintReadWrite, Category = "Anim")
	class AWeapon* CurrentWeapon;
	
	// might want to edit IK later on
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Anim")
	FIKProperties IKProperties;

	/////////////////////////////////// State ///////////////////////////////////
	UPROPERTY(BlueprintReadOnly, Category = "Anim")
	FRotator LastRotation;

	///////////////////////////////// IK Variables ///////////////////////////////////
	// dont want to reference cam transform each time, want to be slightly different
	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category = "Anim")
	FTransform CameraTransform;

	// world transform but relative to base of mesh
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Anim")
	FTransform RelativeCameraTransform;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Anim")
	FTransform RHandToSightsTransform;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Anim")
	FTransform OffsetTransform;

	// 0 = idle, 1 = aiming
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Anim")
	float ADSWeight = 0.f;

	///////////////////////////////// Accumulative Offsets ///////////////////////////////////
	UPROPERTY(BlueprintReadWrite, Category = "Anim")
	FRotator AccumulativeRotation;

	// tries to catch up to target by interpolating towards it - actual offset
	UPROPERTY(BlueprintReadWrite, Category = "Anim")
	FRotator AccumulativeRotationInterp;
	
};


