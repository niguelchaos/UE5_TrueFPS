// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/TrueFPSCharacter.h"
#include "Camera/CameraComponent.h"
#include "Net/UnrealNetwork.h"
#include "Weapons/Weapon.h"

// Sets default values
ATrueFPSCharacter::ATrueFPSCharacter()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// camera lags behind 1 frame - tick in the mesh called later, when camera updated the tick will be called on mesh
	GetMesh()->SetTickGroup(ETickingGroup::TG_PostUpdateWork);

	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	// camera should be on head, always facing controller
	Camera->bUsePawnControlRotation = true;
	Camera->SetupAttachment(GetMesh(), FName("head"));
}

//Called when the game starts or when spawned
void ATrueFPSCharacter::BeginPlay()
{
	Super::BeginPlay();

// check if in server, begin play called in both server and client
	if (HasAuthority())
	{
		for(const TSubclassOf<AWeapon>& WeaponClass : DefaultWeapons)
		{
			// skip if not valid
			if (!WeaponClass) continue;

			// give params before spawn
			FActorSpawnParameters Params;

			// set owner to us to make sure replicates properly
			Params.Owner = this;
			// ref to weapon - spawned from weaponclass
			AWeapon* SpawnedWeapon = GetWorld()->SpawnActor<AWeapon>(WeaponClass, Params);

			const int32 Index = Weapons.Add(SpawnedWeapon);

			if (Index == CurrentIndex)
			{
				// equip
				CurrentWeapon = SpawnedWeapon;
				// server wont call this function by default - need to manually call
				// no weapon previously
				OnRep_CurrentWeapon(nullptr);

			}
		}
	}
}


// Called to bind functionality to input
void ATrueFPSCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAction(FName("NextWeapon"), EInputEvent::IE_Pressed, this, &ATrueFPSCharacter::NextWeapon);
	PlayerInputComponent->BindAction(FName("LastWeapon"), EInputEvent::IE_Pressed, this, &ATrueFPSCharacter::LastWeapon);

	PlayerInputComponent->BindAxis(FName("Move Forward / Backward"), this, &ATrueFPSCharacter::MoveForward);
	PlayerInputComponent->BindAxis(FName("Move Right / Left"), this, &ATrueFPSCharacter::MoveRight);
	PlayerInputComponent->BindAxis(FName("Look Up / Down Mouse"), this, &ATrueFPSCharacter::LookUp);
	PlayerInputComponent->BindAxis(FName("Turn Right / Left Mouse"), this, &ATrueFPSCharacter::LookRight);
	
}

void ATrueFPSCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(ATrueFPSCharacter, Weapons, COND_None);
	
	// weapon not replicated unless you create a replication policy?
	DOREPLIFETIME_CONDITION(ATrueFPSCharacter, CurrentWeapon, COND_None);
	
}

void ATrueFPSCharacter::OnRep_CurrentWeapon(const AWeapon* OldWeapon)
{
	// may want to replicate nothing if nothing equipped
	if (CurrentWeapon)
	{
		if (!CurrentWeapon->CurrentOwner)
		{
			// multiply with weapon's placement transform to align gun correctly?
			const FTransform& PlacementTransform = CurrentWeapon->PlacementTransform * GetMesh()->GetSocketTransform(FName("weaponsocket_r"));
			
			// attach to mesh's handsocket
			// teleport gun in case gun has physics
			CurrentWeapon->SetActorTransform(PlacementTransform, false, nullptr, ETeleportType::TeleportPhysics);
			CurrentWeapon->AttachToComponent(GetMesh(), FAttachmentTransformRules::KeepWorldTransform, FName("weaponsocket_r"));

			CurrentWeapon->CurrentOwner = this;
		}
		CurrentWeapon->Mesh->SetVisibility(true);
	}

	if (OldWeapon)
	{
		OldWeapon->Mesh->SetVisibility(false);
	}

	// sounds like an event - tell everybody weapon changed
	CurrentWeaponChangedDelegate.Broadcast(CurrentWeapon, OldWeapon);
}



void ATrueFPSCharacter::EquipWeapon(const int32 Index)
{
	// dont swap to same weapon
	if (!Weapons.IsValidIndex(Index) || CurrentWeapon == Weapons[Index]) return;

// Client
	if (IsLocallyControlled())
	{
		// change index to new index
		CurrentIndex = Index;
		
		const AWeapon* OldWeapon = CurrentWeapon;
		CurrentWeapon = Weapons[Index];
		OnRep_CurrentWeapon(OldWeapon);
	}
	else if (!HasAuthority())
	{
		Server_SetCurrentWeapon(Weapons[Index]);
	}

	
}


void ATrueFPSCharacter::Server_SetCurrentWeapon_Implementation(AWeapon* NewWeapon)
{
	const AWeapon* OldWeapon = CurrentWeapon;
	CurrentWeapon = NewWeapon;
	OnRep_CurrentWeapon(OldWeapon);
}









// Controls
void ATrueFPSCharacter::NextWeapon()
{
	// if switch weapons locally, need to have no latency
	// swap first, then tell server

	// if next index valid make that next else go back to first weapon
	const int32 Index = Weapons.IsValidIndex(CurrentIndex + 1) ? CurrentIndex + 1 : 0;
	EquipWeapon(Index);
}

void ATrueFPSCharacter::LastWeapon()
{
	const int32 Index = Weapons.IsValidIndex(CurrentIndex - 1) ? CurrentIndex - 1 : Weapons.Num() - 1;
	EquipWeapon(Index);
}




void ATrueFPSCharacter::MoveForward(const float Value)
{
	// Use only yaw value, get unit direction
	const FVector& Direction = FRotationMatrix(FRotator(0.f, GetControlRotation().Yaw, 0.f)).GetUnitAxis(EAxis::X);
	AddMovementInput(Direction, Value);
}

void ATrueFPSCharacter::MoveRight(const float Value)
{
	const FVector& Direction = FRotationMatrix(FRotator(0.f, GetControlRotation().Yaw, 0.f)).GetUnitAxis(EAxis::Y);
	AddMovementInput(Direction, Value);
}

void ATrueFPSCharacter::LookUp(const float Value)
{
	AddControllerPitchInput(Value);
}

void ATrueFPSCharacter::LookRight(const float Value)
{
	AddControllerYawInput(Value);
}

