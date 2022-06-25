// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapons/Weapon.h"

AWeapon::AWeapon()
{
	// dont update yet
	PrimaryActorTick.bCanEverTick = false;

	// make sure server replicates the things to client when server spawns the things
	SetReplicates(true);

	// create root components
	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	RootComponent = Root;

	Mesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Mesh"));
	Mesh->SetupAttachment(Root);
}

void AWeapon::BeginPlay()
{
	// spawn weapon on server, replicate down to client
	// if equipped, then should be visible but in beginning, not equipped
	Super::BeginPlay();

	if (!CurrentOwner)
	{
		Mesh->SetVisibility(false);
	}
	// PRINT(TEXT("%s: Visible"), *AUTH);
}