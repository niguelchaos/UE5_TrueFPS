// Fill out your copyright notice in the Description page of Project Settings.


#include "TrueFPSAnimInstance.h"

#include <concrt.h>

#include "Camera/CameraComponent.h"
#include "Character/TrueFPSCharacter.h"
#include "Kismet/KismetMathLibrary.h"


UTrueFPSAnimInstance::UTrueFPSAnimInstance()
{
	
}

void UTrueFPSAnimInstance::NativeBeginPlay()
{
	Super::NativeBeginPlay();

	// Character not valid yet apparently
	
	// Character = Cast<ATrueFPSCharacter>(TryGetPawnOwner());
	// // if valid char
	// if (Character)
	// {
	// 	Mesh = Character->GetMesh();
	// 	// call at beginning of game to be changed before any logic is run
	// 	Character->CurrentWeaponChangedDelegate.AddDynamic(this, &UTrueFPSAnimInstance::CurrentWeaponChanged);
	// 	CurrentWeaponChanged(Character->CurrentWeapon, nullptr);
	// }
}

void UTrueFPSAnimInstance::NativeUpdateAnimation(float DeltaTime)
{
	Super::NativeUpdateAnimation(DeltaTime);

	// keep casting until its valid
	if(!Character)
	{
		Character = Cast<ATrueFPSCharacter>(TryGetPawnOwner());

		if(Character)
		{
			Mesh = Character->GetMesh();
			// call at beginning of game to be changed before any logic is run
			// add dynamic is reflection system function
			Character->CurrentWeaponChangedDelegate.AddDynamic(this, &UTrueFPSAnimInstance::CurrentWeaponChanged);
			CurrentWeaponChanged(Character->CurrentWeapon, nullptr); 
		}
		else return;
	}

	SetVars(DeltaTime);
	CalculateWeaponSway(DeltaTime);

	LastRotation = CameraTransform.Rotator();
	
}

// basically OnWeaponChanged
void UTrueFPSAnimInstance::CurrentWeaponChanged(AWeapon* NewWeapon, const AWeapon* OldWeapon)
{
	CurrentWeapon = NewWeapon;
	if (CurrentWeapon)
	{
		IKProperties = CurrentWeapon->IKProperties;
		// get anim pose set pose for character,
		// but if immediately set ik transforms, even though we have our ik pose variable set
		// will take a tick before pose will change, getting wrong offsets
		// instead, need to wait a tick

		GetWorld()->GetTimerManager().SetTimerForNextTick(this, &UTrueFPSAnimInstance::SetIKTransforms);
	}
}

void UTrueFPSAnimInstance::SetVars(const float DeltaTime)
{
	// getbaseaimrotation instead of cam transform because rotation of cam isnt replicated by default but base aim is,
	// then pass in location
	CameraTransform = FTransform(Character->GetBaseAimRotation(), Character->Camera->GetComponentLocation());

	const FTransform& RootOffset = Mesh->GetSocketTransform(FName("root"), RTS_Component).Inverse() * Mesh->GetSocketTransform(FName("ik_hand_root"));
	RelativeCameraTransform = CameraTransform.GetRelativeTransform(RootOffset);

	// every tick set to chars adsweight
	ADSWeight = Character->ADSWeight;

	/*
	 * OFFSETS
	 */
	
	// Accumulative Rotation
	constexpr float AngleClamp = 6.f; // if super spin
	const FRotator& AddRotation = CameraTransform.Rotator() - LastRotation;
	// pitch and yaw with multiplier looks a bit better because want to pitch and yaw more
	FRotator AddRotationClamped = FRotator(
		FMath::ClampAngle(AddRotation.Pitch, -AngleClamp, AngleClamp) * 1.5f,
		FMath::ClampAngle(AddRotation.Yaw, -AngleClamp, AngleClamp), 0.f);

	// when looking horizontally, add roll
	AddRotationClamped.Roll = AddRotationClamped.Yaw * 0.7f;
	
	AccumulativeRotation += AddRotationClamped;
	// set it back to base rotation - essentially our target
	// will look strange if plugged directly because its constantly changing
	AccumulativeRotation = UKismetMathLibrary::RInterpTo(AccumulativeRotation, FRotator::ZeroRotator, DeltaTime, 30.f);
	AccumulativeRotationInterp = UKismetMathLibrary::RInterpTo(AccumulativeRotationInterp, AccumulativeRotation, DeltaTime, 5.f);
	
}
void UTrueFPSAnimInstance::CalculateWeaponSway(const float DeltaTime)
{
	FVector LocationOffset = FVector::ZeroVector;
	FRotator RotationOffset = FRotator::ZeroRotator;

	// keep having additives
	// decrement it down to 0 over time if idle

	// inverse rotation - going in direction of camera, want it to lag behind
	// want to save because we will add it to location as well
	const FRotator& AccumulativeRotationInterpInverse = AccumulativeRotationInterp.GetInverse();
	RotationOffset += AccumulativeRotationInterpInverse;

	// therefore want location to lag behind too
	LocationOffset += FVector(0.f, AccumulativeRotationInterpInverse.Yaw, AccumulativeRotationInterpInverse.Pitch) / 6.f;






	// less is faster
	LocationOffset *= IKProperties.WeightScale; 
	RotationOffset.Pitch *= IKProperties.WeightScale;
	RotationOffset.Yaw *= IKProperties.WeightScale;
	RotationOffset.Roll *= IKProperties.WeightScale;
	
	OffsetTransform = FTransform(RotationOffset, LocationOffset);
}

void UTrueFPSAnimInstance::SetIKTransforms()
{
	// set sights transform to our sights socket relative transform
	RHandToSightsTransform = CurrentWeapon->GetSightsWorldTransform().GetRelativeTransform(Mesh->GetSocketTransform(FName("hand_r")));
}


