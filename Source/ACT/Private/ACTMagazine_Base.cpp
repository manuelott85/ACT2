// Fill out your copyright notice in the Description page of Project Settings.


#include "ACTMagazine_Base.h"
#include "ACTProjectile_Base.h"
#include "Runtime/Engine/Classes/Components/StaticMeshComponent.h"

// Sets default values
AACTMagazine_Base::AACTMagazine_Base()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;

	magazineAsset = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("magazineAsset"));
	magazineAsset->SetupAttachment(GetRootComponent());
}

// Called when the game starts or when spawned
void AACTMagazine_Base::BeginPlay()
{
	Super::BeginPlay();

	// prefill the magazine
	currentAmmo.Reserve(maxAmmoCount);
	loadProjectile(AACTProjectile_Base::StaticClass(), maxAmmoCount);
}

// Called every frame
void AACTMagazine_Base::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

bool AACTMagazine_Base::loadProjectile(TSubclassOf<AACTProjectile_Base> projectile, int amount)
{
	// check for valid input
	if (amount < 1 || !projectile)
		return false;
	
	// load new rounds into the magazine
	for (int i = 0; i < amount; i++)
		currentAmmo.Emplace(projectile);	// push creates the object in the heap first and copies it over into the vector, emplaces creates the object directly in the vector without copying
		//currentAmmo.Push(projectile);

	return true;
}

TSubclassOf<AACTProjectile_Base> AACTMagazine_Base::removeProjectile()
{
	if (currentAmmo.Num() < 1)
		return NULL;

	// remove the projectile and return its classname for further use
	return currentAmmo.Pop(false);
}

int AACTMagazine_Base::getAmmoLeft()
{
	return currentAmmo.Num();
}