// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Weapon.generated.h"

/**
 * 
 */
// blueprinttype means can access struct in animgragh
// for IK properties
USTRUCT(BlueprintType)
struct FIKProperties
{
	GENERATED_BODY()

	// ref to base anim pose for weapon
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UAnimSequence* AnimPose;


	// custom weapon offset transform idk wtf this
	// offset from basepose to where you want weapon to be
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FTransform CustomOffsetTransform;

	// distance between camera and sights when aiming
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float AimOffset = 15.f;

	// makes sway more sluggish or snappier
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float WeightScale = 1.f;
	
};

// Abstract because dont want to directly use this class
UCLASS(Abstract)
class TRUEFPS_API AWeapon : public AActor
{
	GENERATED_BODY()
public:
	AWeapon();

protected:
	virtual void BeginPlay() override;

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Component")
	class USceneComponent* Root;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Component")
	class USkeletalMeshComponent* Mesh;


	// current owner ref
	// if weapon in inventory, have ref to owner
	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category = "State")
	class ATrueFPSCharacter* CurrentOwner;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Configurations")
	FIKProperties IKProperties;

	// offset when grab weapon - when attaching to mesh want to orient certain way
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Configurations")
	FTransform PlacementTransform;

	// returns transform of sights, bpnativeevent = can have 2 implementations: 1 for c++, another for bp.
	// BP implmentation overrides function to return whatever it wants
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="IK")
	FTransform GetSightsWorldTransform() const;
	virtual FTransform GetSightsWorldTransform_Implementation() const { return Mesh->GetSocketTransform(FName("Sights")); }
};

