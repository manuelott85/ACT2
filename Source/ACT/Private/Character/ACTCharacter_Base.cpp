#include "ACTCharacter_Base.h"
#include "Runtime/Engine/Classes/Camera/CameraComponent.h"
#include <Runtime/Engine/Classes/Components/AudioComponent.h>
#include "Runtime/Engine/Classes/Components/SkeletalMeshComponent.h"
#include "GameFramework/PlayerController.h"
#include <UnrealNetwork.h>

#include "ACTCharacterMovementComponent.h"
#include "Weapon_Base.h"
#include "Core/Logger.h"

AACTCharacter_Base::AACTCharacter_Base(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer.SetDefaultSubobjectClass<UACTCharacterMovementComponent>(CharacterMovementComponentName))	// Replace the MovementComponent with a custom one
{
	PrimaryActorTick.bCanEverTick = true;
	SetReplicates(true);
	SetReplicatingMovement(true);

	// Mesh Component
	USkeletalMeshComponent* meshComp = GetMesh();
	meshComp->SetOwnerNoSee(true);

	// Create a CameraComponent
	_firstPersonCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
	_firstPersonCamera->SetupAttachment(GetRootComponent());
	_firstPersonCamera->SetRelativeLocation(FVector(0, 0, 75)); // Position the camera
	_firstPersonCamera->bUsePawnControlRotation = true;

	// Set up Movement Component
	UCharacterMovementComponent* moveComp = GetCharacterMovement();
	moveComp->NavAgentProps.bCanCrouch = true;
	moveComp->NavAgentProps.bCanJump = true;
	moveComp->NavAgentProps.bCanWalk = true;
	moveComp->NavAgentProps.bCanSwim = false;
	moveComp->NavAgentProps.bCanFly = false;
	moveComp->MaxAcceleration = 1024;
	moveComp->BrakingFrictionFactor = 0;
	_maxWalkSpeedStored = moveComp->MaxWalkSpeed;	// Store the maxWalkSpeed from the movementComponent

	// Weapon Sockets
	_weaponPositionAds = CreateDefaultSubobject<USceneComponent>(TEXT("WeaponPositionAimDownSights"));
	_weaponPositionAds->SetupAttachment(_firstPersonCamera);
	_weaponPositionHip = CreateDefaultSubobject<USceneComponent>(TEXT("WeaponPositionHipFire"));
	_weaponPositionHip->SetupAttachment(_firstPersonCamera);

	// Audio Source
	_audioSource = CreateDefaultSubobject<UAudioComponent>(TEXT("AudioSource"));
}

void AACTCharacter_Base::SpawnAndEquipWeapon(TSubclassOf<AWeapon_Base> weapon) {
	if (HasAuthority()) {
		ULogger::Print("SpawnAndEquipWeapon");
		// only proceed if there is not a weapon currently equipped
		if (_primaryWeapon) {
			return;
		}

		FActorSpawnParameters SpawnParams;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

		_primaryWeapon = GetWorld()->SpawnActor<AWeapon_Base>(weapon, FVector::ZeroVector, FRotator::ZeroRotator, SpawnParams);
		if (_primaryWeapon) {
			_primaryWeapon->SetOwner(this);
			_primaryWeapon->AttachToComponent(_weaponPositionHip, FAttachmentTransformRules::SnapToTargetNotIncludingScale, "");
		}
	}
}

void AACTCharacter_Base::BeginPlay() {
	Super::BeginPlay();

	SpawnAndEquipWeapon(_startWeaponClass);

	if (_playerController == nullptr) {
		_playerController = Cast<APlayerController>(GetController());
	}
	if (_playerController) {
		if (_playerController->IsLocalPlayerController()) {
			ULogger::Print(GetName() + " is Local");
		}
	} else {
		ULogger::Print(GetName() + " No Controller");
	}
}

void AACTCharacter_Base::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);

	if (HasAuthority()) {
		Server_ReplicateControllerRotation();
		/*AController* ctrl = GetController();
		if (ctrl) {
			ULogger::Print(ctrl->GetName());
		}*/
	}
}

void AACTCharacter_Base::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const {
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	// List of every replicated variable, including a condition if needed
	DOREPLIFETIME(AACTCharacter_Base, _bIsCrouching);
	DOREPLIFETIME(AACTCharacter_Base, _bAimsDownSights);
	DOREPLIFETIME(AACTCharacter_Base, _bIsSprinting);
	DOREPLIFETIME(AACTCharacter_Base, _primaryWeapon);
	DOREPLIFETIME(AACTCharacter_Base, _controllerRotation);
}

void AACTCharacter_Base::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) {
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	// set up gameplay key bindings
	check(PlayerInputComponent);
	PlayerInputComponent->BindAction("Crouch", IE_Pressed, this, &AACTCharacter_Base::RequestCrouch);
	PlayerInputComponent->BindAction("Vault", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Sprint", IE_Pressed, this, &AACTCharacter_Base::RequestSprint);
	PlayerInputComponent->BindAction("AimDownSights", IE_Pressed, this, &AACTCharacter_Base::RequestAimDownSights);
	PlayerInputComponent->BindAction("FireWeapon", IE_Pressed, this, &AACTCharacter_Base::RequestWeaponFire);
	PlayerInputComponent->BindAction("Reload", IE_Pressed, this, &AACTCharacter_Base::RequestReload);

	PlayerInputComponent->BindAxis("MoveForward", this, &AACTCharacter_Base::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AACTCharacter_Base::MoveRight);

	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
}

float AACTCharacter_Base::TakeDamage(float Damage, struct FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) {
	ULogger::Print("I was hit with " + FString::SanitizeFloat(Damage) + " damage");
	//HealthComponent->RequestRemoveHealth(Damage);
	return Damage;
}

void AACTCharacter_Base::MoveForward(float value) {
	if (value != 0.0f) {
		AddMovementInput(GetActorForwardVector(), value);
	}

	bool shouldMoveFwd = value > 0;
	if (!shouldMoveFwd && _bIsSprinting) {
		RequestSprint(false);
	}
}

void AACTCharacter_Base::MoveRight(float value) {
	if (value != 0.0f) {
		AddMovementInput(GetActorRightVector(), value);

		if (_bIsSprinting) {
			RequestSprint(false);
		}
	}
}

void AACTCharacter_Base::RequestCrouch() {
	RequestCrouch(!_bIsCrouching);
}

void AACTCharacter_Base::RequestCrouch(bool value) {
	ULogger::Print("RequestCrouch", FColor::Blue);
	if (value) {
		if (CanCrouch()) {
			if (_bIsSprinting) {
				RequestSprint(false);
			}

			Crouch();
			Server_SetIsCrouching(true);
			GetCharacterMovement()->MaxWalkSpeed = GetCharacterMovement()->MaxWalkSpeedCrouched;	// apply crouch speed (owning Client)
		}
	} else {
		UnCrouch();
		Server_SetIsCrouching(false);
		GetCharacterMovement()->MaxWalkSpeed = _maxWalkSpeedStored;	// Reset walk speed to the stored one (owning Client)
	}
}

void AACTCharacter_Base::Server_SetIsCrouching_Implementation(bool value) {
	ULogger::Print("Server_SetIsCrouching_Implementation", FColor::Green);
	_bIsCrouching = value;
	Multi_PlayCrouchEffects();
	if (value) {
		GetCharacterMovement()->MaxWalkSpeed = GetCharacterMovement()->MaxWalkSpeedCrouched;	// apply crouch speed (Server)
	} else {
		GetCharacterMovement()->MaxWalkSpeed = _maxWalkSpeedStored;	// Reset walk speed to the stored one (Server)
	}
}


void AACTCharacter_Base::Multi_PlayCrouchEffects_Implementation() {
	ULogger::Print("Multi_PlayCrouchEffects_Implementation", FColor::Yellow);
	//FirstPersonAudioSource
	//ThirdPersonAudioSource->Sound = CrouchSound;
	//ThirdPersonAudioSource->Play();
}

void AACTCharacter_Base::RequestSprint() {
	RequestSprint(!_bIsSprinting);
}

void AACTCharacter_Base::RequestSprint(bool value) {
	ULogger::Print("RequestSprint", FColor::Blue);
	if (value) {
		// Ensure to be in a standing position and not aiming
		if (_bIsCrouching) {
			RequestCrouch(false);
		}
		if (_bAimsDownSights) {
			RequestAimDownSights(false);
		}

		Server_SetIsSprinting(true);
		GetCharacterMovement()->MaxWalkSpeed = Cast<UACTCharacterMovementComponent>(GetCharacterMovement())->_maxSprintSpeed;	// apply sprint speed (Client)
	} else {
		Server_SetIsSprinting(false);
		GetCharacterMovement()->MaxWalkSpeed = _maxWalkSpeedStored;	// Reset walk speed to the stored one (Client)
	}
}

void AACTCharacter_Base::Server_SetIsSprinting_Implementation(bool value) {
	ULogger::Print("Server_SetIsSprinting_Implementation", FColor::Green);
	_bIsSprinting = value;
	float sprintSpeed = Cast<UACTCharacterMovementComponent>(GetCharacterMovement())->_maxSprintSpeed;
	if (value) {
		GetCharacterMovement()->MaxWalkSpeed = sprintSpeed;	// apply sprint speed (Server)
	} else {
		GetCharacterMovement()->MaxWalkSpeed = _maxWalkSpeedStored;	// Reset walk speed to the stored one (Server)
	}
}

void AACTCharacter_Base::RequestAimDownSights() {
	RequestAimDownSights(!_bAimsDownSights);
}

void AACTCharacter_Base::RequestAimDownSights(bool value) {
	ULogger::Print("RequestAimDownSights", FColor::Blue);
	if (value) {
		// exit, if there is no primary weapon
		if (!_primaryWeapon) {
			return;
		}

		if (_bIsSprinting) {
			RequestSprint(false);
		}

		Server_SetAimsDownSights(true);
		GetCharacterMovement()->MaxWalkSpeed = ((UACTCharacterMovementComponent*)GetCharacterMovement())->_maxWalkSpeedAiming;	// apply aim speed (owning Client)
	} else {
		Server_SetAimsDownSights(false);
		GetCharacterMovement()->MaxWalkSpeed = _maxWalkSpeedStored;	// Reset walk speed to the stored one (owning Client)
	}
}

void AACTCharacter_Base::Server_SetAimsDownSights_Implementation(bool value) {
	ULogger::Print("Server_SetAimsDownSights_Implementation", FColor::Green);
	_bAimsDownSights = value;

	if (value) {
		GetCharacterMovement()->MaxWalkSpeed = ((UACTCharacterMovementComponent*)GetCharacterMovement())->_maxWalkSpeedAiming;	// apply aim speed (Server)
		_primaryWeapon->AttachToComponent(_weaponPositionAds, FAttachmentTransformRules::SnapToTargetNotIncludingScale, "");
	} else {
		GetCharacterMovement()->MaxWalkSpeed = _maxWalkSpeedStored;	// Reset walk speed to the stored one (Server)
		_primaryWeapon->AttachToComponent(_weaponPositionHip, FAttachmentTransformRules::SnapToTargetNotIncludingScale, "");
	}
}

// TODO: Send the local (client) camera's orientation with the request.
// This should ensure that everything matches the clients view as close as possible
// And it could potentially be set to "reliable"
void AACTCharacter_Base::RequestWeaponFire() {
	ULogger::Print("RequestWeaponFire", FColor::Blue);
	Server_WeaponFire();
}

void AACTCharacter_Base::Server_WeaponFire_Implementation() {
	ULogger::Print("Server_WeaponFire", FColor::Green);
	if (_primaryWeapon && _primaryWeapon->FireWeapon(this)) {
		Multi_WeaponFire();
	}
}
void AACTCharacter_Base::Multi_WeaponFire_Implementation() {
	ULogger::Print("Multi_WeaponFire", FColor::Yellow);
	if (_primaryWeapon) {
		_primaryWeapon->PlayFireEffects(FVector::ZeroVector);
	}
}

void AACTCharacter_Base::RequestReload() {
	ULogger::Print("RequestReload", FColor::Blue);
	Server_Reload();
	return;
}

void AACTCharacter_Base::Server_Reload_Implementation() {
	ULogger::Print("Server_Reload_Implementation", FColor::Green);
	if (_primaryWeapon && _primaryWeapon->Reload()) {
		Multi_ReloadEffects();
	}
	return;
}

void AACTCharacter_Base::Multi_ReloadEffects_Implementation() {
	ULogger::Print("Multi_ReloadEffects_Implementation", FColor::Yellow);

	/*UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance != NULL && reloadAnimMontage != NULL)
	{
		AnimInstance->Montage_Play(reloadAnimMontage);
	}*/
}

void AACTCharacter_Base::Server_ReplicateControllerRotation_Implementation() {
	Multi_ReplicateControllerRotation(GetControlRotation());
}

void AACTCharacter_Base::Multi_ReplicateControllerRotation_Implementation(FRotator value) {
	_controllerRotation = value;
	_firstPersonCamera->SetWorldRotation(_controllerRotation);
}