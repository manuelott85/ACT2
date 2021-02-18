#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

#include "ACTMagazine_Base.generated.h"

class AACTProjectile_Base;
class UStaticMeshComponent;

UCLASS()
class ACT_API AACTMagazine_Base : public AActor {
	GENERATED_BODY()

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		UStaticMeshComponent* magazineAsset;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		TArray<TSubclassOf<AACTProjectile_Base>> currentAmmo;

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float maxAmmoCount = 30;

public:
	AACTMagazine_Base();

protected:
	virtual void BeginPlay() override;

public:
	UFUNCTION()
		bool LoadProjectile(TSubclassOf<AACTProjectile_Base> projectile, int amount = 1);
	UFUNCTION()
		TSubclassOf<AACTProjectile_Base> RemoveAndReturnTopMostProjectile();
	UFUNCTION()
		int GetRoundsLoaded();
};
