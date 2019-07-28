// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ACTProjectile_Base.generated.h"

class UProjectileMovementComponent;
class UCapsuleComponent;
class UStaticMeshComponent;

UCLASS()
class ACT_API AACTProjectile_Base : public AActor
{
	GENERATED_BODY()

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UCapsuleComponent* CapsuleComponent;
	//UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	//UStaticMeshComponent* ProjectileAsset;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UProjectileMovementComponent* ProjectileMovement;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	float damage = 27;

	AController* pInstigator;
	AActor* pFromWeapon;
	FDamageEvent damageType;
	
public:	
	// Sets default values for this actor's properties
	AACTProjectile_Base();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Overlap
	UFUNCTION()
	void BeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult &SweepResult);

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
