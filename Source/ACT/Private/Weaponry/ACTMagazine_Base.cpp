#include "ACTMagazine_Base.h"
#include "ACTProjectile_Base.h"
#include "Runtime/Engine/Classes/Components/StaticMeshComponent.h"

AACTMagazine_Base::AACTMagazine_Base() {
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;

	magazineAsset = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("magazineAsset"));
	magazineAsset->SetupAttachment(GetRootComponent());
}

void AACTMagazine_Base::BeginPlay() {
	Super::BeginPlay();

	// prefill the magazine
	currentAmmo.Reserve(maxAmmoCount);
	LoadProjectile(AACTProjectile_Base::StaticClass(), maxAmmoCount);
}

bool AACTMagazine_Base::LoadProjectile(TSubclassOf<AACTProjectile_Base> projectile, int amount) {
	// check for valid input
	bool bNoActualAmountProvided = amount < 1;
	bool bAmountExceedsMagSize = amount + GetRoundsLoaded() > maxAmmoCount;
	if (bNoActualAmountProvided || bAmountExceedsMagSize) {
		return false;
	}

	// load new rounds into the magazine
	for (int i = 0; i < amount; i++) {
		currentAmmo.Emplace(projectile); // push creates the object in the heap first and copies it over into the vector, emplace creates the object directly in the vector without copying
	}
	return true;
}

TSubclassOf<AACTProjectile_Base> AACTMagazine_Base::RemoveAndReturnTopMostProjectile() {
	if (currentAmmo.Num() < 1) {
		return NULL;
	}

	// remove the projectile and return its class name for further use
	return currentAmmo.Pop(false);
}

int AACTMagazine_Base::GetRoundsLoaded() {
	return currentAmmo.Num();
}