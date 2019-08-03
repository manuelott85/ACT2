// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"

#include "ACTCharacter.generated.h"

class UCameraComponent;
class UAudioComponent;
class UACTCharacterMovementComponent;
class AWeapon_Base;
class UHealthComponent;
class AController;

UCLASS()
class ACT_API AACTCharacter : public ACharacter
{
	GENERATED_BODY()

private:
	float maxWalkSpeedStored = 0;	// to store the preset maxWalkSpeed from the movementComponent
	float cameraAnimTimer = 0;
	FVector cameraOffest;
	FQuat cameraRot;
	bool bCameraIsAnimating = false;

protected:
	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly)
	AWeapon_Base* PrimaryWeapon;

	UPROPERTY(EditDefaultsOnly, Category = "Player")
	TSubclassOf<AWeapon_Base> StarterWeaponClass;

	UPROPERTY(VisibleDefaultsOnly, Category = "Player")
	FName WeaponAttachSocketName;

	// First person camera
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	UCameraComponent* FirstPersonCamera;

	// How much time should the camera need to reach a new position (used for aiming down the sights)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera")
	float animTime = 0.25;

	// FP Audio Component
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Audio")
	UAudioComponent* FirstPersonAudioSource;

	// TP Audio Component
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Audio")
	UAudioComponent* ThirdPersonAudioSource;

	// Sounds
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio")
	USoundBase* CrouchSound;

	// Code-Hook to drive animations
	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Movement")
	bool bIsCrouching = false;

	// Code-Hook to drive animations
	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Movement")
	bool bAimsDownSights = false;

	// Code-Hook to drive animations
	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Movement")
	bool bIsSprinting = false;

	// Code-Hook to drive animations, is updated via multicast
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement")
	FRotator lookRotation;

	// Aim Alpha is a timeline value from 0.0 to 1.0
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio")
	float aimAlpha = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Health")
	UHealthComponent* HealthComponent;

	UPROPERTY(EditDefaultsOnly)
	UAnimMontage* reloadAnimMontage;

public:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Handles input for moving forward and backward.
	UFUNCTION()
	void MoveForward(float Value);

	// Handles input for moving right and left.
	UFUNCTION()
	void MoveRight(float Value);

	// Handles input for crouching
	UFUNCTION()
	void RequestCrouch();

	UFUNCTION(Server, unreliable, WithValidation)
	void Server_SetIsCrouching(bool value);

	UFUNCTION(NetMulticast, unreliable, WithValidation)
	void Multi_PlayCrouchEffects();

	// Handles input for AimDownSights
	UFUNCTION()
	void RequestAimDownSights();

	UFUNCTION(Server, unreliable, WithValidation)
	void Server_SetAimsDownSights(bool value);

	UFUNCTION(Client, unreliable)
	void Client_ResetCameraAnimTimer();

	UFUNCTION()
	void UpdateCameraPos(float DeltaTime);

	// Handles input for sprinting
	UFUNCTION()
	void RequestSprint();

	UFUNCTION()
	void ForceStopSprint();

	UFUNCTION(Server, unreliable, WithValidation)
	void Server_SetIsSprinting(bool value);

	// Reblicate the controller rotation
	UFUNCTION(Server, unreliable, WithValidation)
	void Server_UpdateLookRotation();

	UFUNCTION(NetMulticast, unreliable, WithValidation)
	void Multi_UpdateLookRotation(FRotator value);

	// Weapon Actions
	UFUNCTION(BlueprintCallable)
	void RequestWeaponFire();

	UFUNCTION(Server, unreliable, WithValidation)
	void Server_WeaponFire();

	UFUNCTION(NetMulticast, unreliable, WithValidation)
	void Multi_WeaponFire();

	UFUNCTION()
	void RequestReload();

	UFUNCTION(Server, unreliable, WithValidation)
	void Server_Reload();

	UFUNCTION(NetMulticast, unreliable, WithValidation)
	void Multi_ReloadEffects();

public:
	// Sets default values for this character's properties
	AACTCharacter(const FObjectInitializer& ObjectInitializer);

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual float TakeDamage(float Damage, struct FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;
};
