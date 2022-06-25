// Fill out your copyright notice in the Description page of Project Settings.


#include "TrueFPSAnimInstance.h"

#include "Camera/CameraComponent.h"
#include "Character/TrueFPSCharacter.h"


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

	//	
}
void UTrueFPSAnimInstance::CalculateWeaponSway(const float DeltaTime)
{
	
}

void UTrueFPSAnimInstance::SetIKTransforms()
{
	// set sights transform to our sights socket relative transform
	RHandToSightsTransform = CurrentWeapon->GetSightsWorldTransform().GetRelativeTransform(Mesh->GetSocketTransform(FName("hand_r")));
}


