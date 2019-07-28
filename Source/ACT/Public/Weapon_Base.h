// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Weapon_Base.generated.h"

class USkeletalMeshComponent;
class AACTProjectile_Base;
class AACTMagazine_Base;
class UParticleSystem;
class UAnimMontage;

UCLASS()
class ACT_API AWeapon_Base : public AActor
{
	GENERATED_BODY()

protected:
	// Gun mesh
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Mesh")
	USkeletalMeshComponent* weaponAsset;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mesh")
	USceneComponent* cameraPosition;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	AACTProjectile_Base* projectileInChamber;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	AACTMagazine_Base* loadedMagazine;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<AACTMagazine_Base> StarterMagazineClass;

	UPROPERTY(EditDefaultsOnly)
	UAnimMontage* shotAnimMontage;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FName muzzleSocket = "Muzzle";
	UPROPERTY(VisibleDefaultsOnly)
	FName MagAttachSocketName = "MagSocket";

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	UParticleSystem* MuzzleEffect;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	UParticleSystem* DefaultImpactEffect;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	UParticleSystem* FleshImpactEffect;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	UParticleSystem* TracerEffect;

public:	
	// Sets default values for this actor's properties
	AWeapon_Base();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION()
	FVector getSightsOffset();

	UFUNCTION()
	bool FireWeapon(APawn* fireInstigator);

	UFUNCTION()
	void PlayFireEffects(FVector TraceEnd);

	UFUNCTION()
	bool Reload();
};
