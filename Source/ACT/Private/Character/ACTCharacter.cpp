// Fill out your copyright notice in the Description page of Project Settings.

#include "ACTCharacter.h"
#include <AIController.h>
//#include <CameraController.h>
#include <UnrealNetwork.h>
#include <Runtime/Engine/Classes/Components/AudioComponent.h>
#include "Runtime/Engine/Classes/Components/SkeletalMeshComponent.h"
#include "Runtime/Engine/Classes/Components/InputComponent.h"
#include "Runtime/Engine/Classes/Camera/CameraComponent.h"
#include "Runtime/Engine/Classes/GameFramework/CharacterMovementComponent.h"
//#include "Runtime/CoreUObject/Public/UObject/ConstructorHelpers.h"
#include "Runtime/Engine/Classes/GameFramework/Controller.h"
#include "ACTCharacterMovementComponent.h"
#include "ACTAIController.h"
#include "Weapon_Base.h"
#include "HealthComponent.h"

#include "EngineGlobals.h"
#include "Runtime/Engine/Classes/Engine/Engine.h"

#define print(color, text) if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 10, color, text)
// print(FColor::Magenta, FString::FromInt(temp));
// print(FColor::Blue, FString::SanitizeFloat(temp));

// Sets default values
AACTCharacter::AACTCharacter(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer.SetDefaultSubobjectClass<UACTCharacterMovementComponent>(CharacterMovementComponentName))	// Replace the MovementComponent with a custom one
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SetReplicates(true);
	SetReplicatingMovement(true);

	// the next two lines stop the editor from loading up!
	//static ConstructorHelpers::FClassFinder<AACTAIController> AIController(TEXT("/Game/Blueprints/AI/BP_ACTAIController"));
	//AIControllerClass = AIController.Class;
	//AIControllerClass = AACTAIController::StaticClass();
	//AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
	AutoPossessAI = EAutoPossessAI::PlacedInWorld;

	// Create a CameraComponent
	FirstPersonCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
	FirstPersonCamera->SetupAttachment(GetMesh(), "CameraSocket");
	FirstPersonCamera->SetRelativeLocation(FVector(0, 18, 0)); // Position the camera
	FirstPersonCamera->SetRelativeRotation(FRotator(0, 90, -90));	// Rotate the camera
	FirstPersonCamera->bUsePawnControlRotation = true;

	// Create a gun mesh component
	//PrimaryWeapon = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("PrimaryWeapon"));
	//PrimaryWeapon->SetupAttachment(GetMesh(), "GunSocket");

	// Create both audio components
	FirstPersonAudioSource = CreateDefaultSubobject<UAudioComponent>(TEXT("FirstPersonAudioSource"));
	ThirdPersonAudioSource = CreateDefaultSubobject<UAudioComponent>(TEXT("ThirdPersonAudioSource"));

	// Init CharacterMovementComponent
	maxWalkSpeedStored = GetCharacterMovement()->MaxWalkSpeed;	// Store the maxWalkSpeed from the movementComponent
	GetCharacterMovement()->NavAgentProps.bCanCrouch = true;	// Allow to crouch
	//GetCharacterMovement()->MinAnalogWalkSpeed = 10;
	GetCharacterMovement()->MaxAcceleration = 1024;
	GetCharacterMovement()->BrakingFrictionFactor = 0;

	WeaponAttachSocketName = "GunSocket";

	HealthComponent = CreateDefaultSubobject<UHealthComponent>(TEXT("healthComponent"));
	HealthComponent->SetIsReplicated(true);
}

// Called when the game starts or when spawned
void AACTCharacter::BeginPlay()
{
	Super::BeginPlay();

	if (GetLocalRole() == ROLE_Authority)
	{
		// Spawn a default weapon
		FActorSpawnParameters SpawnParams;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

		PrimaryWeapon = GetWorld()->SpawnActor<AWeapon_Base>(StarterWeaponClass, FVector::ZeroVector, FRotator::ZeroRotator, SpawnParams);
		if (PrimaryWeapon)
		{
			PrimaryWeapon->SetOwner(this);
			PrimaryWeapon->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, WeaponAttachSocketName);
		}
	}
}

// Called every frame
void AACTCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	UpdateCameraPos(DeltaTime);
	if (HasAuthority()) {
		Server_UpdateLookRotation();
	}
}

void AACTCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	// List of every replicated variable, including a condition if needed
	DOREPLIFETIME(AACTCharacter, bIsCrouching);
	DOREPLIFETIME(AACTCharacter, bIsSprinting);
	DOREPLIFETIME(AACTCharacter, bAimsDownSights);
	DOREPLIFETIME(AACTCharacter, PrimaryWeapon);
}

// Called to bind functionality to input
void AACTCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	// set up gameplay key bindings
	check(PlayerInputComponent);
	PlayerInputComponent->BindAction("Crouch", IE_Pressed, this, &AACTCharacter::RequestCrouch);
	//PlayerInputComponent->BindAction("Vault", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Sprint", IE_Pressed, this, &AACTCharacter::RequestSprint);
	PlayerInputComponent->BindAction("AimDownSights", IE_Pressed, this, &AACTCharacter::RequestAimDownSights);
	//PlayerInputComponent->BindAction("FireWeapon", IE_Pressed, this, &AACTCharacter::RequestFireWeapon);
	PlayerInputComponent->BindAction("FireWeapon", IE_Pressed, this, &AACTCharacter::RequestWeaponFire);
	PlayerInputComponent->BindAction("Reload", IE_Pressed, this, &AACTCharacter::RequestReload);

	PlayerInputComponent->BindAxis("MoveForward", this, &AACTCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AACTCharacter::MoveRight);

	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
}

float AACTCharacter::TakeDamage(float Damage, struct FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	print(FColor::Magenta, "I was hit with " + FString::SanitizeFloat(Damage) + " damage");
	HealthComponent->RequestRemoveHealth(Damage);
	return Damage;
}

void AACTCharacter::MoveForward(float Value)
{
	if (Value != 0.0f)
		AddMovementInput(GetActorForwardVector(), Value);
	if (!(Value > 0))
		if (bIsSprinting)
			ForceStopSprint();
}

void AACTCharacter::MoveRight(float Value)
{
	if (Value != 0.0f)
	{
		AddMovementInput(GetActorRightVector(), Value);

		if (bIsSprinting)
			ForceStopSprint();
	}
}

void AACTCharacter::RequestCrouch()
{
	print(FColor::Blue, "RequestCrouch");
	if (bIsCrouching)
	{
		UnCrouch();
		Server_SetIsCrouching(false);
		GetCharacterMovement()->MaxWalkSpeed = maxWalkSpeedStored;	// Reset walk speed to the stored one (owning Client)
	}
	else
	{
		if (CanCrouch())
		{
			if (bIsSprinting)
				ForceStopSprint();

			Crouch();
			Server_SetIsCrouching(true);
			GetCharacterMovement()->MaxWalkSpeed = GetCharacterMovement()->MaxWalkSpeedCrouched;	// apply crouch speed (owning Client)
		}
	}	
}

void AACTCharacter::Server_SetIsCrouching_Implementation(bool value)
{
	print(FColor::Blue, "Server_SetIsCrouching_Implementation");
	bIsCrouching = value;
	Multi_PlayCrouchEffects();	// Play some sound on all machines
	if(value)
		GetCharacterMovement()->MaxWalkSpeed = GetCharacterMovement()->MaxWalkSpeedCrouched;	// apply crouch speed (Server)
	else
		GetCharacterMovement()->MaxWalkSpeed = maxWalkSpeedStored;	// Reset walk speed to the stored one (Server)
}

bool AACTCharacter::Server_SetIsCrouching_Validate(bool value)
{
	if (value == true || value == false)
		return true;
	else
		return false;
}


void AACTCharacter::Multi_PlayCrouchEffects_Implementation()
{
	print(FColor::Blue, "Multi_PlayCrouchEffects_Implementation");
	//FirstPersonAudioSource
	ThirdPersonAudioSource->Sound = CrouchSound;
	ThirdPersonAudioSource->Play();
}

bool AACTCharacter::Multi_PlayCrouchEffects_Validate()
{
	return true;
}

void AACTCharacter::RequestAimDownSights()
{
	if (bAimsDownSights)
	{
		Server_SetAimsDownSights(false);
		GetCharacterMovement()->MaxWalkSpeed = maxWalkSpeedStored;	// Reset walk speed to the stored one (owning Client)
	}
	else
	{
		// exit, if there is no primary weapon
		if (!PrimaryWeapon)
			return;

		if (bIsSprinting)
			ForceStopSprint();

		Server_SetAimsDownSights(true);
		GetCharacterMovement()->MaxWalkSpeed = ((UACTCharacterMovementComponent*)GetCharacterMovement())->_maxWalkSpeedAiming;	// apply aim speed (owning Client)
	}
}

void AACTCharacter::Server_SetAimsDownSights_Implementation(bool value)
{
	bAimsDownSights = value;
	Client_ResetCameraAnimTimer();

	//Multi_PlayCrouchEffects();	// Play some sound on all machines
	if (value)
		GetCharacterMovement()->MaxWalkSpeed = ((UACTCharacterMovementComponent*)GetCharacterMovement())->_maxWalkSpeedAiming;	// apply aim speed (Server)
	else
		GetCharacterMovement()->MaxWalkSpeed = maxWalkSpeedStored;	// Reset walk speed to the stored one (Server)
}

bool AACTCharacter::Server_SetAimsDownSights_Validate(bool value)
{
	if (value == true || value == false)
		return true;
	else
		return false;
}

void AACTCharacter::Client_ResetCameraAnimTimer_Implementation()
{
	cameraAnimTimer = 0;
	bCameraIsAnimating = true;
	cameraOffest = FirstPersonCamera->GetComponentLocation();
}

//void AACTCharacter::UpdateCameraPos(float DeltaTime)
//{
//	if (!bCameraIsAnimating)
//		return;
//
//	cameraAnimTimer += DeltaTime;	// update the timer
//	float amountOfAnim = cameraAnimTimer / animTime;	// how much of the transition should be happened
//
//	if (amountOfAnim > 1)
//	{
//		bCameraIsAnimating = false;
//		if (bAimsDownSights)
//		{
//			FirstPersonCamera->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, WeaponAttachSocketName);
//			FirstPersonCamera->RelativeLocation = PrimaryWeapon->getSightsOffset(); // Position the camera
//			FirstPersonCamera->RelativeRotation = FRotator(0, 90, -90);	// Rotate the camera
//			return;
//		}
//		else
//		{
//			FirstPersonCamera->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, "CameraSocket");
//			FirstPersonCamera->RelativeLocation = FVector(0, 18, 0); // Position the camera
//			FirstPersonCamera->RelativeRotation = FRotator(0, 90, -90);	// Rotate the camera
//			return;
//		}
//	}
//
//	// Get the target location and rotation
//	FVector location;
//	FQuat rotation;
//	if (bAimsDownSights)
//		(GetMesh())->GetSocketWorldLocationAndRotation("GunSocket", location, rotation);
//	else
//		(GetMesh())->GetSocketWorldLocationAndRotation("CameraSocket", location, rotation);
//
//	// only add the offset if there is a valid weapon pointer
//	if (PrimaryWeapon)
//		location += PrimaryWeapon->getSightsOffset();
//
//	location = FMath::VInterpTo(cameraOffest, location, 1, amountOfAnim);	// calculate the current position
//	FirstPersonCamera->SetWorldLocationAndRotationNoPhysics(location, rotation.Rotator());	// apply location and rotation to the camera
//}

void AACTCharacter::UpdateCameraPos(float DeltaTime)
{
	if (!bCameraIsAnimating)
		return;

	cameraAnimTimer += DeltaTime;	// update the timer
	aimAlpha = FMath::Clamp<float>((cameraAnimTimer / animTime), 0, 1);	// how much of the transition should be happened

	// end transition
	if (aimAlpha >= 1)
	{
		bCameraIsAnimating = false;
		return;
	}
}

void AACTCharacter::RequestSprint()
{
	// Ensure to be in a standing position and not aiming
	if (bIsCrouching)
		RequestCrouch();
	if (bAimsDownSights)
		RequestAimDownSights();

	Server_SetIsSprinting(true);
	GetCharacterMovement()->MaxWalkSpeed = Cast<UACTCharacterMovementComponent>(GetCharacterMovement())->_maxSprintSpeed;	// apply sprint speed (Client)
}

void AACTCharacter::ForceStopSprint()
{
	Server_SetIsSprinting(false);
	GetCharacterMovement()->MaxWalkSpeed = maxWalkSpeedStored;	// Reset walk speed to the stored one (Client)
}

void AACTCharacter::Server_SetIsSprinting_Implementation(bool value)
{
	bIsSprinting = value;
	//Multi_PlayCrouchEffects();	// Play some sound on all machines	
	float sprintSpeed = Cast<UACTCharacterMovementComponent>(GetCharacterMovement())->_maxSprintSpeed;
	if (value)
		GetCharacterMovement()->MaxWalkSpeed = sprintSpeed;	// apply sprint speed (Server)
	else
		GetCharacterMovement()->MaxWalkSpeed = maxWalkSpeedStored;	// Reset walk speed to the stored one (Server)
}

bool AACTCharacter::Server_SetIsSprinting_Validate(bool value)
{
	if (value == true || value == false)
		return true;
	else
		return false;
}

// Reblicate the controller rotation
void AACTCharacter::Server_UpdateLookRotation_Implementation()
{
	Multi_UpdateLookRotation(GetControlRotation());
}

bool AACTCharacter::Server_UpdateLookRotation_Validate()
{
	return true;
}

void AACTCharacter::Multi_UpdateLookRotation_Implementation(FRotator value)
{
	lookRotation = value;
}

bool AACTCharacter::Multi_UpdateLookRotation_Validate(FRotator value)
{
	return true;
}

void AACTCharacter::RequestWeaponFire()
{
	print(FColor::Magenta, "RequestWeaponFire");
	Server_WeaponFire();
}

void AACTCharacter::Server_WeaponFire_Implementation()
{
	print(FColor::Magenta, "Server_WeaponFire");
	if (PrimaryWeapon)
	{
		if (PrimaryWeapon->FireWeapon(this))
			Multi_WeaponFire();
	}
}

bool AACTCharacter::Server_WeaponFire_Validate()
{
	return true;
}

void AACTCharacter::Multi_WeaponFire_Implementation()
{
	print(FColor::Magenta, "Multi_WeaponFire");
	if (PrimaryWeapon)
	{
		PrimaryWeapon->PlayFireEffects(FVector::ZeroVector);
	}
}

bool AACTCharacter::Multi_WeaponFire_Validate()
{
	return true;
}

void AACTCharacter::RequestReload()
{
	Server_Reload();
	return;
}

void AACTCharacter::Server_Reload_Implementation() {
	if (PrimaryWeapon) {
		if (PrimaryWeapon->Reload()) {
			Multi_ReloadEffects();
		}
	}
	return;
}

bool AACTCharacter::Server_Reload_Validate() {
	return true;
}

void AACTCharacter::Multi_ReloadEffects_Implementation() {
	print(FColor::Magenta, "Multi_ReloadEffects");
	
	/*UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance != NULL && reloadAnimMontage != NULL)
	{
		AnimInstance->Montage_Play(reloadAnimMontage);
	}*/
}

bool AACTCharacter::Multi_ReloadEffects_Validate() {
	return true;
}