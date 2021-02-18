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
class ACT_API AWeapon_Base : public AActor {
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
	AWeapon_Base();

protected:
	virtual void BeginPlay() override;

public:

	UFUNCTION()
		FVector getSightsOffset();

	UFUNCTION()
		bool FireWeapon(APawn* fireInstigator);

	UFUNCTION()
		void PlayFireEffects(FVector TraceEnd);

	UFUNCTION()
		bool Reload();
};
