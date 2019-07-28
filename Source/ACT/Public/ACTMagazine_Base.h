// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

#include "ACTMagazine_Base.generated.h"

class AACTProjectile_Base;
class UStaticMeshComponent;

UCLASS()
class ACT_API AACTMagazine_Base : public AActor
{
	GENERATED_BODY()

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UStaticMeshComponent* magazineAsset;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float maxAmmoCount = 30;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TArray<TSubclassOf<AACTProjectile_Base>> currentAmmo;
	
public:	
	// Sets default values for this actor's properties
	AACTMagazine_Base();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION()
	bool loadProjectile(TSubclassOf<AACTProjectile_Base> projectile, int amount = 1);
	UFUNCTION()
	TSubclassOf<AACTProjectile_Base> removeProjectile();
	UFUNCTION()
	int getAmmoLeft();
};
