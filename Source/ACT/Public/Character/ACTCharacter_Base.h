#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "ACTCharacter_Base.generated.h"

class UCameraComponent;
class AWeapon_Base;
class UAudioComponent;
class APlayerController;

UCLASS()
class ACT_API AACTCharacter_Base : public ACharacter {
	GENERATED_BODY()

private:
	float _maxWalkSpeedStored = 0; // to store the preset maxWalkSpeed from the movementComponent
	APlayerController* _playerController = nullptr;

protected:
	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Movement")
		bool _bIsCrouching = false;
	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Movement")
		bool _bAimsDownSights = false;
	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Movement")
		bool _bIsSprinting = false;
	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Movement")
		FRotator _controllerRotation = FRotator::ZeroRotator;
		

public:
	UPROPERTY(EditAnywhere)
		USceneComponent* _weaponPositionAds = nullptr;
	UPROPERTY(EditAnywhere)
		USceneComponent* _weaponPositionHip = nullptr;
	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly)
		AWeapon_Base* _primaryWeapon = nullptr;
	UPROPERTY(EditDefaultsOnly, Category = "Player")
		TSubclassOf<AWeapon_Base> _startWeaponClass;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
		UCameraComponent* _firstPersonCamera = nullptr;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Audio")
		UAudioComponent* _audioSource = nullptr;

private:
	void SpawnAndEquipWeapon(TSubclassOf<AWeapon_Base> weapon);

protected:
	virtual void BeginPlay() override;

	void MoveForward(float value);
	void MoveRight(float value);
	void RequestCrouch();
	void RequestCrouch(bool value);
	void RequestSprint();
	void RequestSprint(bool value);
	void RequestAimDownSights();
	void RequestAimDownSights(bool value);
	void RequestWeaponFire();
	void RequestReload();

	UFUNCTION(Server, unreliable)
		void Server_SetIsCrouching(bool value);
	UFUNCTION(NetMulticast, unreliable)
		void Multi_PlayCrouchEffects();

	UFUNCTION(Server, unreliable)
		void Server_SetIsSprinting(bool value);

	UFUNCTION(Server, unreliable)
		void Server_SetAimsDownSights(bool value);

	UFUNCTION(Server, unreliable)
		void Server_WeaponFire();

	UFUNCTION(NetMulticast, unreliable)
		void Multi_WeaponFire();

	UFUNCTION(Server, unreliable)
		void Server_Reload();

	UFUNCTION(NetMulticast, unreliable)
		void Multi_ReloadEffects();

	// Reblicate the controller rotation
	UFUNCTION(Server, unreliable)
		void Server_ReplicateControllerRotation();

	UFUNCTION(NetMulticast, unreliable)
		void Multi_ReplicateControllerRotation(FRotator value);

public:
	AACTCharacter_Base(const FObjectInitializer& ObjectInitializer);
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	virtual float TakeDamage(float Damage, struct FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;
};
