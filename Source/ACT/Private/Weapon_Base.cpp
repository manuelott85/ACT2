// Fill out your copyright notice in the Description page of Project Settings.

#include "Weapon_Base.h"

#include "Runtime/Engine/Classes/Components/SkeletalMeshComponent.h"
#include "ACTProjectile_Base.h"
#include "ACTMagazine_Base.h"
#include "Kismet/GameplayStatics.h"
#include "Runtime/Engine/Classes/Animation/AnimInstance.h"
//#include "Runtime/Engine/Classes/Engine/Player.h"

#include "EngineGlobals.h"
#include "Runtime/Engine/Classes/Engine/Engine.h"

#define print(color, text) if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 10, color, text)
// print(FColor::Magenta, FString::FromInt(temp));
// print(FColor::Blue, FString::SanitizeFloat(temp));

// Sets default values
AWeapon_Base::AWeapon_Base()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	bReplicates = true;

	// Create a gun mesh component
	weaponAsset = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("PrimaryWeapon"));
	cameraPosition = CreateDefaultSubobject<USceneComponent>(TEXT("CameraPos"));
	//PrimaryWeapon->SetupAttachment(GetMesh(), "GunSocket");
}

// Called when the game starts or when spawned
void AWeapon_Base::BeginPlay()
{
	Super::BeginPlay();
	
	Reload();
}

// Called every frame
void AWeapon_Base::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

FVector AWeapon_Base::getSightsOffset()
{
	// Failsafe code for multiplayer?!
	if (this == nullptr)
		return FVector::ZeroVector;

	return cameraPosition->RelativeLocation;
}

bool AWeapon_Base::FireWeapon(APawn* fireInstigator)
{
	if (!weaponAsset || !loadedMagazine || loadedMagazine->getAmmoLeft() < 1)
		return false;

	FVector spawnlocation = weaponAsset->GetSocketLocation(muzzleSocket);
	FRotator spawnRotation = weaponAsset->GetSocketRotation(muzzleSocket);

	//if (roundToSpawn)
	//	GetWorld()->SpawnActor<AACTProjectile_Base>(roundToSpawn, spawnlocation, spawnRotation);

	FCollisionQueryParams RV_TraceParams = FCollisionQueryParams(FName(TEXT("RV_Trace")), true, this);
	RV_TraceParams.bTraceComplex = true;
	RV_TraceParams.bReturnPhysicalMaterial = false;

	//Re-initialize hit info
	FHitResult RV_Hit(ForceInit);

	//call GetWorld() from within an actor extending class
	GetWorld()->LineTraceSingleByChannel(RV_Hit, spawnlocation, spawnlocation + spawnRotation.Vector() * 10000, ECollisionChannel::ECC_Pawn, RV_TraceParams);

	/*
	RV_Hit.bBlockingHit //did hit something? (bool)
	RV_Hit.GetActor(); //the hit actor if there is one
	RV_Hit.ImpactPoint;  //FVector
	RV_Hit.ImpactNormal;  //FVector
	*/

	TSubclassOf<AACTProjectile_Base> roundToSpawn = loadedMagazine->removeProjectile();	// consume projectile

	AACTProjectile_Base* defaultObject = (AACTProjectile_Base*)roundToSpawn->GetDefaultObject();

	FDamageEvent damageType;
	if (RV_Hit.bBlockingHit && RV_Hit.GetActor())
		RV_Hit.GetActor()->TakeDamage(defaultObject->damage, damageType, NULL, GetOwner());

	print(FColor::Magenta, "Rounds left: " + FString::FromInt(loadedMagazine->getAmmoLeft()));

	return true;
}

void AWeapon_Base::PlayFireEffects(FVector TraceEnd)
{
	print(FColor::Magenta, "PENG! PENG! PENG!");
	
	/*if (MuzzleEffect)
	{
		UGameplayStatics::SpawnEmitterAttached(MuzzleEffect, weaponAsset, muzzleSocket);
	}*/

	UAnimInstance* AnimInstance = weaponAsset->GetAnimInstance();
	if (AnimInstance != NULL && shotAnimMontage != NULL)
	{
		AnimInstance->Montage_Play(shotAnimMontage);
	}

	/*
	if (TracerEffect)
	{
		FVector MuzzleLocation = MeshComp->GetSocketLocation(MuzzleSocketName);

		UParticleSystemComponent* TracerComp = UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), TracerEffect, MuzzleLocation);
		if (TracerComp)
		{
			TracerComp->SetVectorParameter(TracerTargetName, TraceEnd);
		}
	}

	APawn* MyOwner = Cast<APawn>(GetOwner());
	if (MyOwner)
	{
		APlayerController* PC = Cast<APlayerController>(MyOwner->GetController());
		if (PC)
		{
			PC->ClientPlayCameraShake(FireCamShake);
		}
	}*/
}

bool AWeapon_Base::Reload() {
	if (Role == ROLE_Authority)	{
		if (loadedMagazine)
			loadedMagazine->Destroy();


		// Spawn a default magazine
		FActorSpawnParameters SpawnParams;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

		loadedMagazine = GetWorld()->SpawnActor<AACTMagazine_Base>(StarterMagazineClass, FVector::ZeroVector, FRotator::ZeroRotator, SpawnParams);
		if (loadedMagazine)	{
			loadedMagazine->SetOwner(this);
			loadedMagazine->AttachToComponent(weaponAsset, FAttachmentTransformRules::SnapToTargetNotIncludingScale, MagAttachSocketName);
		}

		return true;
	} else {
		return false;
	}
}