// Fill out your copyright notice in the Description page of Project Settings.

#include "ACTCharacterALS.h"
#include "ACTCharacterMovementComponent.h"
#include "ALS_InterfaceACT.h"
#include "FunctionLibrary.h"

#include "Runtime/Engine/Classes/Components/CapsuleComponent.h"
#include "Runtime/Engine/Classes/Camera/CameraComponent.h"
#include "Runtime/Engine/Classes/GameFramework/SpringArmComponent.h"
#include "Runtime/Engine/Classes/Components/ArrowComponent.h"
#include "Runtime/Engine/Classes/GameFramework/NavMovementComponent.h"
#include "Runtime/Engine/Classes/AI/Navigation/NavAreaBase.h"
#include "Runtime/Engine/Classes/Components/SkeletalMeshComponent.h"
#include "Runtime/Engine/Classes/Kismet/KismetMathLibrary.h"
#include "Runtime/Engine/Classes/Components/TimelineComponent.h"
#include "Runtime/Engine/Classes/Curves/CurveFloat.h"
#include "Runtime/Engine/Classes/Animation/AnimInstance.h"
#include "Runtime/Engine/Classes/GameFramework/Controller.h"
#include "Runtime/Engine/Public/TimerManager.h"
#include <UnrealNetwork.h>

#include "EngineGlobals.h"
#include "Runtime/Engine/Classes/Engine/Engine.h"

#define print(color, text, duration) if (GEngine) GEngine->AddOnScreenDebugMessage(-1, duration, color, text)
// print(FColor::Magenta, FString::FromInt(temp), 10);
// print(FColor::Blue, FString::SanitizeFloat(temp, 10));

// Sets default values
AACTCharacterALS::AACTCharacterALS(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer.SetDefaultSubobjectClass<UACTCharacterMovementComponent>(CharacterMovementComponentName))	// Replace the MovementComponent with a custom one
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Capsule Component
	GetCapsuleComponent()->SetCapsuleHalfHeight(90);
	GetCapsuleComponent()->SetCapsuleRadius(30);
	GetCapsuleComponent()->bDynamicObstacle = true;
	GetCapsuleComponent()->CanCharacterStepUpOn = ECB_No;
	GetCapsuleComponent()->SetShouldUpdatePhysicsVolume(true);
	GetCapsuleComponent()->SetCanEverAffectNavigation(false);

	// Mesh
	GetMesh()->bUpdateJointsFromAnimation = true;

	// Arrow Components
	Arrows = CreateDefaultSubobject<USceneComponent>(TEXT("Arrows"));
	Arrows->SetupAttachment(GetRootComponent());
	Arrows->RelativeLocation = FVector(0, 0, -90);
	Arrows->bHiddenInGame = true;

	LookingRotationArrow = CreateDefaultSubobject<UArrowComponent>(TEXT("LookingRotationArrow"));
	LookingRotationArrow->SetupAttachment(Arrows);
	LookingRotationArrow->ArrowColor = FColor(0, 214, 255, 255);
	LookingRotationArrow->ArrowSize = 0.75;
	//LookingRotationArrow->bUseInEditorScaling = false;
	LookingRotationArrow->RelativeLocation = FVector(0, 0, 150);
	LookingRotationArrow->bAbsoluteRotation = true;

	TargetRotationArrow = CreateDefaultSubobject<UArrowComponent>(TEXT("TargetRotationArrow"));
	TargetRotationArrow->SetupAttachment(Arrows);
	TargetRotationArrow->ArrowColor = FColor(255, 156, 0, 255);
	TargetRotationArrow->ArrowSize = 0.75;
	//TargetRotationArrow->bUseInEditorScaling = false;
	TargetRotationArrow->RelativeLocation = FVector(0, 0, 90);
	TargetRotationArrow->bAbsoluteRotation = true;

	CharacterRotationArrow = CreateDefaultSubobject<UArrowComponent>(TEXT("CharacterRotationArrow"));
	CharacterRotationArrow->SetupAttachment(Arrows);
	CharacterRotationArrow->ArrowColor = FColor(0, 255, 0, 255);
	CharacterRotationArrow->ArrowSize = 0.75;
	//CharacterRotationArrow->bUseInEditorScaling = false;
	CharacterRotationArrow->RelativeLocation = FVector(0, 0, 90);
	CharacterRotationArrow->bAbsoluteRotation = true;

	MovementInputArrow = CreateDefaultSubobject<UArrowComponent>(TEXT("MovementInputArrow"));
	MovementInputArrow->SetupAttachment(Arrows);
	MovementInputArrow->ArrowColor = FColor(255, 255, 0, 255);
	//MovementInputArrow->bUseInEditorScaling = false;
	MovementInputArrow->RelativeLocation = FVector(0, 0, 0.6);
	MovementInputArrow->RelativeScale3D = FVector(1, 1.75, 0);
	MovementInputArrow->bAbsoluteRotation = true;

	LastMovementInputRotationArrow = CreateDefaultSubobject<UArrowComponent>(TEXT("LastMovementInputRotationArrow"));
	LastMovementInputRotationArrow->SetupAttachment(Arrows);
	LastMovementInputRotationArrow->ArrowColor = FColor(137, 137, 0, 255);
	//LastMovementInputRotationArrow->bUseInEditorScaling = false;
	LastMovementInputRotationArrow->RelativeLocation = FVector(0, 0, 0.4);
	LastMovementInputRotationArrow->RelativeScale3D = FVector(1, 1.75, 0);
	LastMovementInputRotationArrow->bAbsoluteRotation = true;

	VelocityArrow = CreateDefaultSubobject<UArrowComponent>(TEXT("VelocityArrow"));
	VelocityArrow->SetupAttachment(Arrows);
	VelocityArrow->ArrowColor = FColor(255, 0, 255, 255);
	//VelocityArrow->bUseInEditorScaling = false;
	VelocityArrow->RelativeLocation = FVector(0, 0, 0.2);
	VelocityArrow->RelativeScale3D = FVector(1, 4, 0);
	VelocityArrow->bAbsoluteRotation = true;
	
	LastVelocityRotationArrow = CreateDefaultSubobject<UArrowComponent>(TEXT("LastVelocityRotationArrow"));
	LastVelocityRotationArrow->SetupAttachment(Arrows);
	LastVelocityRotationArrow->ArrowColor = FColor(137, 0, 137, 255);
	//LastVelocityRotationArrow->bUseInEditorScaling = false;
	LastVelocityRotationArrow->RelativeScale3D = FVector(1, 4, 0);
	LastVelocityRotationArrow->bAbsoluteRotation = true;

	// Camera Setup
	TP_SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("TP_SpringArm"));
	TP_SpringArm->SetupAttachment(GetRootComponent());
	TP_SpringArm->TargetArmLength = 325;
	TP_SpringArm->SocketOffset = FVector(0, 0, 45);
	TP_SpringArm->bDoCollisionTest = false;
	TP_SpringArm->bUsePawnControlRotation = true;
	TP_SpringArm->bEnableCameraLag = true;
	TP_SpringArm->CameraLagSpeed = 2;
	TP_SpringArm->bAbsoluteRotation = true;

	// Third Person Camera
	TP_Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("TP_Camera"));
	TP_Camera->SetupAttachment(TP_SpringArm);
	
	// First Person Camera
	FP_Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("FP_Camera"));
	FP_Camera->SetupAttachment(GetRootComponent());
	FP_Camera->FieldOfView = 100;
	FP_Camera->bUsePawnControlRotation = true;
	FP_Camera->RelativeLocation = FVector(0, 0, 60);
	FP_Camera->bAutoActivate = false;
	FP_Camera->AttachToComponent(GetMesh(), FAttachmentTransformRules::KeepRelativeTransform, FirstPersonCameraSocket);
	
	// Character Movement Component
	GetCharacterMovement()->BrakingFrictionFactor = 0;
	GetCharacterMovement()->CrouchedHalfHeight = 60;
	GetCharacterMovement()->SetWalkableFloorAngle(50);
	GetCharacterMovement()->bCanWalkOffLedgesWhenCrouching = true;
	GetCharacterMovement()->JumpZVelocity = 350;
	GetCharacterMovement()->AirControl = 0.1;
	GetCharacterMovement()->RotationRate = FRotator(0, 0, 0);
	GetCharacterMovement()->GetNavAgentPropertiesRef().bCanCrouch = true;
	GetCharacterMovement()->GetNavAgentPropertiesRef().bCanFly = true;

	CameraTargets.VelocityDirection.Standing.Walk.TargetArmLength = 300;
	CameraTargets.VelocityDirection.Standing.Walk.CameraLagSpeed = 10;
	CameraTargets.VelocityDirection.Standing.Walk.SocketOffset = FVector(0, 0, 45);
	CameraTargets.VelocityDirection.Standing.Run.TargetArmLength = 325;
	CameraTargets.VelocityDirection.Standing.Run.CameraLagSpeed = 8;
	CameraTargets.VelocityDirection.Standing.Run.SocketOffset = FVector(0, 0, 45);
	CameraTargets.VelocityDirection.Standing.Sprint.TargetArmLength = 400;
	CameraTargets.VelocityDirection.Standing.Sprint.CameraLagSpeed = 6;
	CameraTargets.VelocityDirection.Standing.Sprint.SocketOffset = FVector(0, 0, 30);
	CameraTargets.VelocityDirection.Crouching.TargetArmLength = 275;
	CameraTargets.VelocityDirection.Crouching.CameraLagSpeed = 10;
	CameraTargets.VelocityDirection.Crouching.SocketOffset = FVector(0, 20, 45);

	CameraTargets.LookingDirection.Standing.Walk.TargetArmLength = 275;
	CameraTargets.LookingDirection.Standing.Walk.CameraLagSpeed = 15;
	CameraTargets.LookingDirection.Standing.Walk.SocketOffset = FVector(0, 70, 60);
	CameraTargets.LookingDirection.Standing.Run.TargetArmLength = 300;
	CameraTargets.LookingDirection.Standing.Run.CameraLagSpeed = 15;
	CameraTargets.LookingDirection.Standing.Run.SocketOffset = FVector(0, 60, 60);
	CameraTargets.LookingDirection.Standing.Sprint.TargetArmLength = 325;
	CameraTargets.LookingDirection.Standing.Sprint.CameraLagSpeed = 15;
	CameraTargets.LookingDirection.Standing.Sprint.SocketOffset = FVector(0, 50, 50);
	CameraTargets.LookingDirection.Crouching.TargetArmLength = 250;
	CameraTargets.LookingDirection.Crouching.CameraLagSpeed = 15;
	CameraTargets.LookingDirection.Crouching.SocketOffset = FVector(0, 60, 45);

	CameraTargets.Aiming.TargetArmLength = 200;
	CameraTargets.Aiming.CameraLagSpeed = 20;
	CameraTargets.Aiming.SocketOffset = FVector(0, 70, 45);
	CameraTargets.Ragdoll.TargetArmLength = 350;
	CameraTargets.Ragdoll.CameraLagSpeed = 50;
	CameraTargets.Ragdoll.SocketOffset = FVector(0, 0, -20);

	// Set some default values
	FRotator curActorRot = GetActorRotation();
	LastVelocityRotation = curActorRot;
	LookingRotation = curActorRot;
	LastMovementInputRotation = curActorRot;
	TargetRotation = curActorRot;
	CharacterRotation = curActorRot;
	SetArrowValues(false);
}

void AACTCharacterALS::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	// List of every replicated variable, including a condition if needed
	DOREPLIFETIME_CONDITION(AACTCharacterALS, MovementInput, COND_SkipOwner);
	DOREPLIFETIME_CONDITION(AACTCharacterALS, CharacterRotation, COND_SkipOwner);
	DOREPLIFETIME_CONDITION(AACTCharacterALS, LookingRotation, COND_SkipOwner);
	DOREPLIFETIME_CONDITION(AACTCharacterALS, TargetRotation, COND_SkipOwner);
	DOREPLIFETIME_CONDITION(AACTCharacterALS, RagdollLocation, COND_SkipOwner);
}

// Called to bind functionality to input
void AACTCharacterALS::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	check(PlayerInputComponent);

	// Movement Input
	PlayerInputComponent->BindAxis("MoveForward", this, &AACTCharacterALS::InputMoveFwd);
	PlayerInputComponent->BindAxis("MoveRight", this, &AACTCharacterALS::InputMoveRight);
	PlayerInputComponent->BindAxis("LookUp", this, &AACTCharacterALS::InputLookUp);
	PlayerInputComponent->BindAxis("Turn", this, &AACTCharacterALS::InputLookRight);
	PlayerInputComponent->BindAction("Crouch", IE_Pressed, this, &AACTCharacterALS::InputStanceAction);
	PlayerInputComponent->BindAction("Walk", IE_Pressed, this, &AACTCharacterALS::InputWalkAction);
	PlayerInputComponent->BindAction("Vault", IE_Pressed, this, &AACTCharacterALS::InputJumpActionPressed);
	PlayerInputComponent->BindAction("Vault", IE_Released, this, &AACTCharacterALS::InputJumpActionReleased);
	PlayerInputComponent->BindAction("Sprint", IE_Pressed, this, &AACTCharacterALS::InputSprintActionPressed);
	PlayerInputComponent->BindAction("Sprint", IE_Released, this, &AACTCharacterALS::InputSprintActionReleased);
	PlayerInputComponent->BindAction("SwitchRotationMode", IE_Pressed, this, &AACTCharacterALS::InputRotationMode);
	PlayerInputComponent->BindAction("AimDownSights", IE_Pressed, this, &AACTCharacterALS::InputAimActionPressed);
	PlayerInputComponent->BindAction("AimDownSights", IE_Released, this, &AACTCharacterALS::InputAimActionReleased);
	PlayerInputComponent->BindAction("Ragdoll", IE_Pressed, this, &AACTCharacterALS::InputRagdollAction);
	PlayerInputComponent->BindAction("Camera", IE_Pressed, this, &AACTCharacterALS::InputCameraAction);
	

	// Debug Input
	PlayerInputComponent->BindAction("DebugSettings", IE_Pressed, this, &AACTCharacterALS::InputShowSettings);
	PlayerInputComponent->BindAction("DebugTraces", IE_Pressed, this, &AACTCharacterALS::InputShowTraces);
}

// Called when the game starts or when spawned
void AACTCharacterALS::BeginPlay()
{
	Super::BeginPlay();

	// Make sure everything gets updated to reflect the default values when game starts.
	GetMesh()->AddTickPrerequisiteActor(this);
	OnALSViewModeChanged();
	OnALSRotationModeChanged();
	OnALSStanceChanged();

	if (ALS_Stance == EALS_Stance::Standing)
		UnCrouch();
	else
		Crouch();
}

// Called every frame
void AACTCharacterALS::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	CalculateEssentialVariables();
	SetArrowValues();
	PrintInfo();

	switch (ALS_MovementMode)
	{
	/*case EALS_MovementMode::None:
		break;*/
	case EALS_MovementMode::Grounded:
		if (ALS_Stance == EALS_Stance::Standing)
		{
			if (ALS_Gait != EALS_Gait::Walking)
				CustomAcceleration();
		}
		break;
	/*case EALS_MovementMode::Falling:
		break;*/
	case EALS_MovementMode::Ragdoll:
		// Set "Stiffness" of Ragdoll based on velocity. The faster the ragdoll moves, the stiffer its joints willl become.
		GetMesh()->SetAllMotorsAngularDriveParams((FMath::GetMappedRangeValueClamped(FVector2D(0, 1000), FVector2D(0, 25000), ChooseVelocity().Size())), 0, 0);
		if (IsLocallyControlled())
		{
			// Disable gravity if falling too fast, preventing the ragdoll from continuously accelerating.
			// This helps to keep ragdoll movement stable and will prevent it from falling through the floor.
			if (ChooseVelocity().Z < -4000)
				GetMesh()->SetEnableGravity(false);
			else
				GetMesh()->SetEnableGravity(true);

			// Ragdoll Velocity, Location, and Rotation are calculated locally and then sent to the server. Actor location and rotation is set to follow the ragdoll.
			RagdollVelocity = ChooseVelocity();
			RagdollLocation = GetMesh()->GetSocketLocation(PelvisBone);
			FTransform resultTransform = CalculateActorLocationAndRotationInRagdoll(GetMesh()->GetSocketRotation(PelvisBone), RagdollLocation);
			SetActorLocation(resultTransform.GetLocation());
			TargetRotation = resultTransform.GetRotation().Rotator();
			FRotator deltaRot = TargetRotation - CharacterRotation;
			deltaRot.Normalize();
			TargetCharacterRotationDifference = deltaRot.Yaw;
			CharacterRotation = TargetRotation;
			SetActorRotation(resultTransform.GetRotation());
			Server_UpdateRagdoll(RagdollVelocity, RagdollVelocity, CharacterRotation, resultTransform.GetLocation());
		}
		else
		{
			// Ragdolls not locally controlled on the client will be pushed toward the replicated 'Ragdoll Location' vector.
			// They will still simulate separately, but will end up in the same location.
			GetMesh()->AddForce((RagdollLocation - GetMesh()->GetSocketLocation(PelvisBone)) * 200, PelvisBone, true);
		}
		break;
	default:
		break;
	}

	if (!IsLocallyControlled())
		return;

	// ALS: Do every Frame
	// Check to see if the player wants to sprint, even when the character cant sprint.
	// This prevents the player from having to re-press the Sprint button whenever sprinting is interrupted.
	if (ShouldSprint)
	{
		if (CanSprint())
		{
			if (ALS_Gait != EALS_Gait::Sprinting)
				SetALS_GaitEvent(EALS_Gait::Sprinting);
		}
	}
	else
	{
		//if (ALS_Gait != EALS_Gait::Running)
		if (ALS_Gait == EALS_Gait::Sprinting)
			SetALS_GaitEvent(EALS_Gait::Running);
	}

	// ALS: Manage Character Rotation
	switch (ALS_MovementMode)
	{
	//case EALS_MovementMode::None:
	//	break;
	case EALS_MovementMode::Grounded:
		if (IsMoving)
		{
			FRotator newRota = LookingDirectionWithOffset(5, 60, -60, 120, -120, 5);
			if (ALS_RotationMode == EALS_RotationMode::VelocityDirection)
			{
				// Use Last Velocity Rotation when in 'Velocity Direction' Rotation mode
				SetCharacterRotation(FRotator(0, LastVelocityRotation.Yaw, 0), true, CalculateRotationRate(165, 5, 375, 10));
			}
			else
			{
				// Use Looking Direction with Offset when in 'Looking Direction' Rotation Mode
				if (ALS_Aiming)
					SetCharacterRotation(newRota, true, CalculateRotationRate(165, 15, 375, 15));
				else
					SetCharacterRotation(newRota, true, CalculateRotationRate(165, 10, 375, 15));
			}
		}
		else
		{
			// Limit Character's rotation when Aiming or in First Person and not moving or playing root motion to prevent the camera from spinning around the Character
			if (!IsPlayingRootMotion() && ALS_RotationMode == EALS_RotationMode::LookingDirection)
			{
				if (ALS_Aiming)
					LimitRotation(90, 15);
				else
				{
					if (ALS_ViewMode == EALS_ViewMode::FirstPerson)
						LimitRotation(90, 15);
				}
			}
		}
		break;
	case EALS_MovementMode::Falling:
		if (ALS_RotationMode == EALS_RotationMode::VelocityDirection)
		{
			// Use 'Jump Rotation' (set when character enters falling state) when in 'Velocity Direction' Rotation Mode
			if (IsMoving)
				SetCharacterRotation(FRotator(0, JumpRotation.Yaw, 0), true, 10);
		}
		else
		{
			// Use 'Looking Rotation' when in 'Looking Direction' Rotaiton Mode
			// Also set 'Jump Rotation' to 'Looking Rotation' so character doesn not rotate when switching rotation modes in mid-air
			JumpRotation = LookingRotation;
			SetCharacterRotation(FRotator(0, JumpRotation.Yaw, 0), true, 10);
		}
		break;
	//case EALS_MovementMode::Ragdoll:
	//	break;
	default:
		break;
	}
}

// ------------------ Essential Functions

void AACTCharacterALS::CalculateEssentialVariables()
{
	// Determine if the Character is moving, then sets the 'Last Velocity Rotation' and 'Direction',
	// but only when moving to prevent them from returning to 0 when velocity is 0
	FVector velocity = ChooseVelocity();

	IsMoving = FVector(velocity.X, velocity.Y, 0) != FVector::ZeroVector;

	if (IsMoving)
	{
		LastVelocityRotation = FRotationMatrix::MakeFromX(velocity).Rotator();
		FRotator rotationDelta = LastVelocityRotation - CharacterRotation;
		rotationDelta.Normalize();
		Direction = rotationDelta.Yaw;
	}

	// Set 'Movement Input' vector locally then send is to the server to be replicated
	if (IsLocallyControlled())
	{
		MovementInput = GetCharacterMovement()->GetLastInputVector();
		Server_SetMovementInput(MovementInput);
	}

	// Determine if there is Movement Input, and if there is, set 'Last Movement Input Rotation' and 'Movement Input / Velocity Difference'
	// to prevent them from returning to 0 when Movement Input is 0
	HasMovementInput = MovementInput != FVector::ZeroVector;
	if (HasMovementInput)
	{
		LastMovementInputRotation = FRotationMatrix::MakeFromX(MovementInput).Rotator();
		FRotator rotationDelta = LastMovementInputRotation - LastVelocityRotation;
		rotationDelta.Normalize();
		MovementInputVelocityDifference = rotationDelta.Yaw;
	}

	// Set 'Looking Rotation' Locally then send it to the server to be replicated
	float PrevAimYaw;
	if (IsLocallyControlled())
	{
		PrevAimYaw = LookingRotation.Yaw;
		LookingRotation = GetControlRotation();
		Server_SetLookingRotation(LookingRotation);
	}

	// Set 'Aim Yaw Rate' (Rate the camera is turning)
	AimYawRate = (LookingRotation.Yaw - PrevAimYaw) / FApp::GetDeltaTime();

	// Set 'Aim Yaw Delta' (Yaw Angle between the Looking Rotation and Character Rotation)
	FRotator rotationDelta = LookingRotation - CharacterRotation;
	rotationDelta.Normalize();
	AimYawDelta = rotationDelta.Yaw;

	return;
}

FVector AACTCharacterALS::ChooseVelocity()
{
	// Choose velocity based on movement mode.
	switch (ALS_MovementMode)
	{
	case EALS_MovementMode::None:
	case EALS_MovementMode::Grounded:
	case EALS_MovementMode::Falling:
		return GetVelocity();
	case EALS_MovementMode::Ragdoll:
		return GetMesh()->GetPhysicsLinearVelocity(PelvisBone);
	default:
		return FVector::ZeroVector;
	}
}

// ------------------ Debug Functions

void AACTCharacterALS::SetArrowValues(bool DoIfInvisible)
{
	//Set rotation of the Arrow Components. This helps to visualize the values of the Essential variables during game - play.
	if (DoIfInvisible || Arrows->IsVisible())
	{
		CharacterRotationArrow->SetWorldRotation(CharacterRotation);
		LookingRotationArrow->SetWorldRotation(LookingRotation);
		TargetRotationArrow->SetWorldRotation(TargetRotation);
		
		MovementInputArrow->SetWorldRotation(FRotator(FRotationMatrix::MakeFromX(MovementInput).Rotator().Pitch, LastMovementInputRotation.Yaw, FRotationMatrix::MakeFromX(MovementInput).Rotator().Roll));
		MovementInputArrow->SetRelativeScale3D(FVector(FMath::Clamp((float)MovementInput.Size(), (float)0, (float)1), 1.75, 0));

		LastMovementInputRotationArrow->SetWorldRotation(LastMovementInputRotation);

		FVector loc_Velocity = ChooseVelocity();
		VelocityArrow->SetWorldRotation(FRotator(FRotationMatrix::MakeFromX(loc_Velocity).Rotator().Pitch, LastVelocityRotation.Yaw, FRotationMatrix::MakeFromX(loc_Velocity).Rotator().Roll));
		
		VelocityArrow->SetRelativeScale3D(FVector(FMath::GetMappedRangeValueClamped(FVector2D(0, GetCharacterMovement()->MaxWalkSpeed), FVector2D(0, 1), loc_Velocity.Size()), 4, 0));

		LastVelocityRotationArrow->SetWorldRotation(FRotator(0, LastVelocityRotation.Yaw, 0));
	}

	return;
}

// Toggles Visibilty of Capsule and Arrow Components
void AACTCharacterALS::ToggleCapsuleAndArrowVisibility(bool Visible)
{
	GetCapsuleComponent()->SetHiddenInGame(!Visible,false);
	Arrows->SetHiddenInGame(!Visible, true);
	return;
}

void AACTCharacterALS::PrintInfo()
{
	if (!(IsPlayerControlled() && IsLocallyControlled()))
		return;

	FString Output = "";
	if (ShowSettings)
	{
		print(FColor::Magenta, "Restart Level ( R )", 0);
		print(FColor::Black, "\n-------------------- - ", 0);

		Output = "Show Traces: " + FString::FromInt(ShowTraces) + " ( T )" +
			"\n			Show Collision(V)" +
			"\n			Show Bones(B)" +
			"\n			Toggle Mesh Visibility(M)";
		print(FColor::Red, Output, 0);
		print(FColor::Black, "\n			Debug\n			-------------------- - ", 0);

		Output = "Jump Velocity: " + FString::SanitizeFloat(GetCharacterMovement()->JumpZVelocity) + " (Up / Down)" +
			"\n			Air Control : " + FString::SanitizeFloat(GetCharacterMovement()->AirControl) + " (Ctrl Up / Down)" +
			"\n			Jump Hold Time : " + FString::SanitizeFloat(JumpMaxHoldTime) + " (Shift Up / Down)";
		print(FColor::Orange, Output, 0);

		Output = "Walking Speed: " + FString::SanitizeFloat(WalkingSpeed) + " (Ctrl Right / Left)" +
			"\n 			Running Speed : " + FString::SanitizeFloat(RunningSpeed) + " (Right / Left)" +
			"\n			Sprinting Speed : " + FString::SanitizeFloat(SprintingSpeed) + " (SHIFT Right / Left)" +
			"\n			Crouching Speed : " + FString::SanitizeFloat(CrouchingSpeed) + " (ALT Right / Left)";
		print(FColor::Green, Output, 0);
		print(FColor::Black, "\n			Movement Settings\n			-------------------- - ", 0);

		Output = "Rotation Mode: " + FString::FromInt((int)ALS_RotationMode) +
			"\n			Stance: " + FString::FromInt((int)ALS_Stance) +
			"\n			Gait: " + FString::FromInt((int)ALS_Gait) + " - " + FString::SanitizeFloat(ChooseVelocity().Size()) +
			"\n			Movement Mode : " + FString::FromInt((int)ALS_MovementMode);
		print(FColor::Yellow, Output, 0);
		print(FColor::Black, "\n			ALS State Values\n			-------------------- - ", 0);

		Output = "Press 'TAB' to Hide Info (" + GetName() + ")";
		print(FColor::Black, Output, 0);
	}
	else
	{
		// print(FColor::Magenta, FString::FromInt(temp));
		// print(FColor::Blue, FString::SanitizeFloat(temp));

		Output = "Press 'TAB' to Show Info (" + GetName() + ")";
		print(FColor::Black, Output, 0);
	}

	return;
}

// ------------------ OnChanged Functions
void AACTCharacterALS::OnPawnMovementModeChanged(/*EMovementMode PrevMovementMode, */EMovementMode NewMovementMode/*, BYTE PrevCustomMode, BYTE NewCustomMove*/)
{
	// Update ALS Movement Mode whenever Pawn Movement Mode is changed
	switch (NewMovementMode)
	{
	case MOVE_None:
		return;
	case MOVE_Walking:
	case MOVE_NavWalking:
		SetALSMovementModeEvent(EALS_MovementMode::Grounded);
		return;
	case MOVE_Falling:
		SetALSMovementModeEvent(EALS_MovementMode::Falling);
		return;
	case MOVE_Swimming:
	case MOVE_Flying:
	case MOVE_Custom:
	case MOVE_MAX:
	default:
		return;
	}
}

void AACTCharacterALS::OnMovementModeChanged(EMovementMode PrevMovementMode, uint8 PreviousCustomMode)
{
	OnPawnMovementModeChanged(GetCharacterMovement()->MovementMode);

	return;
}

void AACTCharacterALS::OnALSMovementModeChanged()
{
	// Send value to AnimBP through Interface
	IALS_InterfaceACT* AnimBPInterface = Cast<IALS_InterfaceACT>(GetMesh()->GetAnimInstance());
	if (AnimBPInterface)
		AnimBPInterface->SetALS_MovementModeBPI(ALS_MovementMode);

	// Send value to PostProcessInstance (IK_AnimBP) through Interface
	IALS_InterfaceACT* PostProcessInterface = Cast<IALS_InterfaceACT>(GetMesh()->GetPostProcessInstance());
	if (PostProcessInterface)
		PostProcessInterface->SetALS_MovementModeBPI(ALS_MovementMode);

	UpdateALSCharacterMovementSettings();
	if (!IsLocallyControlled())
		return;

	switch (ALS_PrevMovementMode)
	{
	case EALS_MovementMode::None:
		break;
	case EALS_MovementMode::Grounded:
		JumpRotation = IsMoving ? LastVelocityRotation : CharacterRotation;
		RotationOffset = 0;
		break;
	case EALS_MovementMode::Falling:
		break;
	case EALS_MovementMode::Ragdoll:
		JumpRotation = CharacterRotation;
		UpdateCameraEvent();
		break;
	default:
		break;
	}

	switch (ALS_MovementMode)
	{
	case EALS_MovementMode::None:
	case EALS_MovementMode::Grounded:
	case EALS_MovementMode::Falling:
		break;
	case EALS_MovementMode::Ragdoll:
		UpdateCameraEvent();
		break;
	default:
		break;
	}

	return;
}

void AACTCharacterALS::OnALSGaitChanged()
{
	// Send value to AnimBP through Interface
	IALS_InterfaceACT* AnimBPInterface = Cast<IALS_InterfaceACT>(GetMesh()->GetAnimInstance());
	if (AnimBPInterface)
		AnimBPInterface->SetALS_GaitBPI(ALS_Gait);

	UpdateALSCharacterMovementSettings();

	if (!IsLocallyControlled())
		return;

	if (ALS_Aiming)
		UpdateCameraEvent();
	else
		UpdateCameraEvent();

	return;
}

void AACTCharacterALS::OnALSStanceChanged()
{
	// Send value to AnimBP through Interface
	IALS_InterfaceACT* AnimBPInterface = Cast<IALS_InterfaceACT>(GetMesh()->GetAnimInstance());
	if (AnimBPInterface)
		AnimBPInterface->SetALS_StanceBPI(ALS_Stance);

	// Send value to PostProcessInstance (IK_AnimBP) through Interface
	IALS_InterfaceACT* PostProcessInterface = Cast<IALS_InterfaceACT>(GetMesh()->GetPostProcessInstance());
	if (PostProcessInterface)
		PostProcessInterface->SetALS_StanceBPI(ALS_Stance);

	UpdateALSCharacterMovementSettings();
	UpdateCameraEvent();

	// Offset Arrow / Texts location
	float newZ;
	switch (ALS_Stance)
	{
	case EALS_Stance::Standing:
		newZ = GetCapsuleComponent()->GetScaledCapsuleHalfHeight();
		break;
	case EALS_Stance::Crouching:
		newZ = (GetCharacterMovement()->CrouchedHalfHeight) + 0.5;
		break;
	default:
		break;
	}

	Arrows->SetRelativeLocation(FVector(0, 0, (newZ * -1)));

	return;
}

void AACTCharacterALS::OnStartCrouch(float HalfHeightAdjust, float ScaledHalfHeightAdjust)
{
	SetALS_StanceEvent(EALS_Stance::Crouching);
	return;
}

void AACTCharacterALS::OnEndCrouch(float HalfHeightAdjust, float ScaledHalfHeightAdjust)
{
	SetALS_StanceEvent(EALS_Stance::Standing);
	return;
}

void AACTCharacterALS::OnALSRotationModeChanged()
{
	// Send value to AnimBP through Interface
	IALS_InterfaceACT* AnimBPInterface = Cast<IALS_InterfaceACT>(GetMesh()->GetAnimInstance());
	if (AnimBPInterface)
		AnimBPInterface->SetALS_RotationModeBPI(ALS_RotationMode);

	if (!IsLocallyControlled())
		return;

	UpdateCameraEvent();

	// If moving, set 'Rotation Rate Multiplier' to 0. This slows drastic changes in rotation to make rotation smoother
	if (IsMoving)
		RotationRateMultiplier = 0;

	// If new Rotation Mode is Velocity Direction while in First Person, switch the View Mode to Third Person
	if (ALS_RotationMode == EALS_RotationMode::VelocityDirection && ALS_ViewMode == EALS_ViewMode::FirstPerson)
		SetALSViewModeEvent(EALS_ViewMode::ThirdPerson);

	return;
}

void AACTCharacterALS::OnALSAimingChanged()
{
	// Send value to AnimBP through Interface
	IALS_InterfaceACT* AnimBPInterface = Cast<IALS_InterfaceACT>(GetMesh()->GetAnimInstance());
	if (AnimBPInterface)
		AnimBPInterface->SetALS_AimingBPI(ALS_Aiming);

	UpdateALSCharacterMovementSettings();
	
	if (ALS_Aiming)
		UpdateCameraEvent();
	else
		UpdateCameraEvent();

	return;
}

void AACTCharacterALS::OnALSViewModeChanged()
{
	// Send value to AnimBP through Interface
	IALS_InterfaceACT* AnimBPInterface = Cast<IALS_InterfaceACT>(GetMesh()->GetAnimInstance());
	if (AnimBPInterface)
		AnimBPInterface->SetALS_ViewModeBPI(ALS_ViewMode);

	// Set Active Camera based on View Mode.
	switch (ALS_ViewMode)
	{
	case EALS_ViewMode::ThirdPerson:
		FP_Camera->SetActive(false);
		TP_Camera->SetActive(true);
		break;
	case EALS_ViewMode::FirstPerson:
		FP_Camera->SetActive(true);
		TP_Camera->SetActive(false);
		break;
	default:
		break;
	}

	// Set Rotation mode to Looking Direction if entering First Person.
	if (ALS_RotationMode == EALS_RotationMode::VelocityDirection)
		SetALS_RotationModeEvent(EALS_RotationMode::LookingDirection);
	
	return;
}

void AACTCharacterALS::Landed(const FHitResult & Hit)
{
	if (HasMovementInput)
		GetCharacterMovement()->BrakingFrictionFactor = 0.5;
	else
		GetCharacterMovement()->BrakingFrictionFactor = 3;
	GetWorldTimerManager().SetTimer(onLandedTimerHandle, this, &AACTCharacterALS::LandedDelayed, 0.2, false);

	return;
}

void AACTCharacterALS::LandedDelayed()
{
	GetCharacterMovement()->BrakingFrictionFactor = 0;

	return;
}

// ------------------ Rotation System Functions
void AACTCharacterALS::SetCharacterRotation(FRotator TargetRotationInput, bool InterpRotation, float InterpSpeed)
{
	// Interp 'Character Rotation' to 'Target Rotation' if 'Interp Rotation' input is true. Also set 'Target / Character Rotation Difference'
	TargetRotation = TargetRotationInput;
	FRotator rotationDelta = TargetRotationInput - CharacterRotation;
	rotationDelta.Normalize();
	TargetCharacterRotationDifference = rotationDelta.Yaw;
	
	if (InterpRotation)
	{
		if (InterpSpeed != 0)
		{
			CharacterRotation = FMath::RInterpTo(CharacterRotation, TargetRotation, FApp::GetDeltaTime(), InterpSpeed);
		}
	}
	else
		CharacterRotation = TargetRotation;

	// Set Actor Rotation to 'Character Rotation' and send rotation to server (only if game is networked)
	SetActorRotation(CharacterRotation, ETeleportType::None);
	Server_SetCharacterRotationEvent(TargetRotation, CharacterRotation);

	return;
}

// Add to current 'Character Rotation' then Set Actor Rotation.
void AACTCharacterALS::AddCharacterRotation(FRotator AddAmount)
{
	TargetRotation = TargetRotation - AddAmount.GetInverse();
	TargetRotation.Normalize();

	FRotator rotationDelta = TargetRotation - CharacterRotation;
	rotationDelta.Normalize();
	TargetCharacterRotationDifference = rotationDelta.Yaw;

	CharacterRotation = CharacterRotation - AddAmount.GetInverse();
	CharacterRotation.Normalize();

	SetActorRotation(CharacterRotation, ETeleportType::None);
	Server_SetCharacterRotationEvent(TargetRotation, CharacterRotation);

	return;
}

// Determine Cardinal Direction and use it to apply an offset relative to the Looking Rotation
// This allows the character to stay rotated at set angles relative to movement while moving in an arbitrary direction
// See image for more clarification - https://photos.app.goo.gl/J2WjSyL38XJ8nmkK7
FRotator AACTCharacterALS::LookingDirectionWithOffset(float OffsetInterpSpeed, float NEAngle, float NWAngle, float SEAngle, float SWAngle, float Buffer)
{
	FRotator rotationDelta;
	if (HasMovementInput)
		rotationDelta = LastMovementInputRotation;
	else
		rotationDelta = LastVelocityRotation;
	rotationDelta = rotationDelta - LookingRotation;
	rotationDelta.Normalize();

	if (GetCardinalDirection(rotationDelta.Yaw, NWAngle, NEAngle, Buffer, ECardinalDirection::North))
		CardinalDirection = ECardinalDirection::North;
	else
	{
		if (GetCardinalDirection(rotationDelta.Yaw, NEAngle, SEAngle, Buffer, ECardinalDirection::East))
			CardinalDirection = ECardinalDirection::East;
		else
		{
			if (GetCardinalDirection(rotationDelta.Yaw, SWAngle, NWAngle, Buffer, ECardinalDirection::West))
				CardinalDirection = ECardinalDirection::West;
			else
				CardinalDirection = ECardinalDirection::South;
		}
	}

	float Target;

	if (ALS_Gait == EALS_Gait::Walking && !ALS_Aiming)
		Target = 0;
	else
	{
		switch (CardinalDirection)
		{
		case ECardinalDirection::North:
			Target = rotationDelta.Yaw;
			break;
		case ECardinalDirection::East:
			Target = rotationDelta.Yaw - 90;
			break;
		case ECardinalDirection::West:
			Target = rotationDelta.Yaw + 90;
			break;
		case ECardinalDirection::South:
			if (rotationDelta.Yaw > 0)
				Target = rotationDelta.Yaw - 180;
			else
				Target = rotationDelta.Yaw + 180;
			break;
		default:
			break;
		}
	}	
	
	RotationOffset = FMath::FInterpTo(RotationOffset, Target, FApp::GetDeltaTime(), OffsetInterpSpeed);

	return FRotator(0, (LookingRotation.Yaw + RotationOffset), 0);
}

// Dynamically set Rotation Rate based on characters movement speed. Supports 2 Rotation Rates for differnet speeds
// The Rotation Rate Multiplier is used to ramp down rotation rate whenever it is set to 0 (Automatically returns to 1)
float AACTCharacterALS::CalculateRotationRate(float SlowSpeed, float SlowSpeedRate, float FastSpeed, float FastSpeedRate)
{
	float returnVal;
	float value = FVector(ChooseVelocity().X, ChooseVelocity().Y, 0).Size();
	if (RotationRateMultiplier == 1)
	{
		if (value > SlowSpeed)
			returnVal = FMath::GetMappedRangeValueClamped(FVector2D(SlowSpeed, FastSpeed), FVector2D(SlowSpeedRate, FastSpeedRate), value);
		else
			returnVal = FMath::GetMappedRangeValueClamped(FVector2D(0, SlowSpeed), FVector2D(0, SlowSpeedRate), value);

		returnVal = returnVal * RotationRateMultiplier;
		return FMath::Clamp(returnVal, (float)0.1, (float)15);
	}
	else
	{
		RotationRateMultiplier = FMath::Clamp((float)(RotationRateMultiplier + FApp::GetDeltaTime()), (float)0, (float)1);
		return 0;
	}
}

void AACTCharacterALS::LimitRotation(float AimYawLimit, float InterpSpeed)
{
	if (!(FMath::Abs(AimYawDelta) > AimYawLimit))
		return;

	float targetRotationYaw;
	if (AimYawDelta > 0)
		targetRotationYaw = LookingRotation.Yaw - AimYawLimit;
	else
		targetRotationYaw = LookingRotation.Yaw + AimYawLimit;

	SetCharacterRotation(FRotator(0, targetRotationYaw, 0), true, InterpSpeed);

	return;
}

bool AACTCharacterALS::GetCardinalDirection(float Value, float Min, float Max, float Buffer, ECardinalDirection loc_CardinalDirection)
{
	if (CardinalDirection == loc_CardinalDirection)
		return (Value >= (Min + Buffer) && Value <= (Max - Buffer));
	else
		return (Value >= (Min - Buffer) && Value <= (Max + Buffer));
}

// ------------------ Movement System Functions

// Choose and Update Character Movement settings
void AACTCharacterALS::UpdateALSCharacterMovementSettings()
{
	UCharacterMovementComponent* movComp = GetCharacterMovement();
	if (!movComp)
		return;

	movComp->MaxWalkSpeed = ChooseMaxWalkSpeed();
	movComp->MaxWalkSpeedCrouched = movComp->MaxWalkSpeed;
	movComp->MaxAcceleration = ChooseMaxAcceleration();
	movComp->BrakingDecelerationWalking = ChooseBrakingDeceleration();
	movComp->GroundFriction = ChooseGroundFriction();

	return;
}

float AACTCharacterALS::ChooseMaxWalkSpeed()
{
	if (ALS_Stance == EALS_Stance::Standing)
	{
		if (ALS_Aiming)
		{
			switch (ALS_Gait)
			{
			case EALS_Gait::Walking:
				return WalkingSpeed;
			case EALS_Gait::Running:
				return WalkingSpeed;
			case EALS_Gait::Sprinting:
				return RunningSpeed;
			default:
				break;
			}
		}
		else
		{
			switch (ALS_Gait)
			{
			case EALS_Gait::Walking:
				return WalkingSpeed;
			case EALS_Gait::Running:
				return RunningSpeed;
			case EALS_Gait::Sprinting:
				return SprintingSpeed;
			default:
				break;
			}
		}
	}
	else
	{
		switch (ALS_Gait)
		{
		case EALS_Gait::Walking:
			return CrouchingSpeed - 50;
		case EALS_Gait::Running:
			return CrouchingSpeed;
		case EALS_Gait::Sprinting:
			return CrouchingSpeed + 50;
		default:
			break;
		}
	}

	return 0;
}

float AACTCharacterALS::ChooseMaxAcceleration()
{
	switch (ALS_Gait)
	{
	case EALS_Gait::Walking:
		return WalkingAcceleration;
	case EALS_Gait::Running:
		return RunningAcceleration;
	case EALS_Gait::Sprinting:
		return RunningAcceleration;
	default:
		break;
	}

	return 0;
}

float AACTCharacterALS::ChooseBrakingDeceleration()
{
	switch (ALS_Gait)
	{
	case EALS_Gait::Walking:
		return WalkingDeceleration;
	case EALS_Gait::Running:
		return RunningDeceleration;
	case EALS_Gait::Sprinting:
		return RunningDeceleration;
	default:
		break;
	}

	return 0;
}

float AACTCharacterALS::ChooseGroundFriction()
{
	switch (ALS_Gait)
	{
	case EALS_Gait::Walking:
		return WalkingGroundFriction;
	case EALS_Gait::Running:
		return RunningGroundFriction;
	case EALS_Gait::Sprinting:
		return RunningGroundFriction;
	default:
		break;
	}

	return 0;
}

// Gets 'Movement Input / Velocity Difference' to dynamically change Acceleration and Ground Friction when changing directions, giving the chacter more "Weight".
// This is also essential to the pivot system, as it allows time for the pivot to play before accelerating in the opposite direction. 
void AACTCharacterALS::CustomAcceleration()
{
	GetCharacterMovement()->MaxAcceleration = RunningAcceleration * FMath::GetMappedRangeValueClamped(FVector2D(45, 130), FVector2D(1, 0.2), FMath::Abs(MovementInputVelocityDifference));
	GetCharacterMovement()->GroundFriction = RunningGroundFriction * FMath::GetMappedRangeValueClamped(FVector2D(45, 130), FVector2D(1, 0.4), FMath::Abs(MovementInputVelocityDifference));

	return;
}

// Determines if the Character is allowed to sprint. When in the 'Looking Direction' Rotation mode,
// the character can only sprint when Movement Input is facing forward (with a 50 degree threshhold),
// like most Traditional First person / Third person shooter games.
bool AACTCharacterALS::CanSprint()
{
	switch (ALS_MovementMode)
	{
	case EALS_MovementMode::None:
		break;
	case EALS_MovementMode::Grounded:
	case EALS_MovementMode::Falling:
		if (ALS_RotationMode == EALS_RotationMode::VelocityDirection)
			return true;
		else
		{
			if (HasMovementInput)
			{
				if (ALS_Aiming)
					return true;
				else
				{
					FRotator rotationDelta = LastMovementInputRotation - LookingRotation;
					rotationDelta.Normalize();
					return FMath::Abs(rotationDelta.Yaw) < 50;
				}
			}
			else
				return true;
		}
	case EALS_MovementMode::Ragdoll:
		return true;
	default:
		break;
	}

	return true;
}

// ------------------ Camera System Functions

// Choose Target Camera Settings based on current state
FACTALS_CameraSettings AACTCharacterALS::ChooseCameraSettings()
{
	switch (ALS_MovementMode)
	{
	case EALS_MovementMode::None:
		break;
	case EALS_MovementMode::Grounded:
	case EALS_MovementMode::Falling:
		if (ALS_Aiming)
			return CameraTargets.Aiming;
		else
		{
			if (ALS_RotationMode == EALS_RotationMode::VelocityDirection)
			{
				if (ALS_Stance == EALS_Stance::Standing)
				{
					switch (ALS_Gait)
					{
					case EALS_Gait::Walking:
						return CameraTargets.VelocityDirection.Standing.Walk;
					case EALS_Gait::Running:
						return CameraTargets.VelocityDirection.Standing.Run;
					case EALS_Gait::Sprinting:
						return CameraTargets.VelocityDirection.Standing.Sprint;
					default:
						break;
					}
				}
				else
					return CameraTargets.VelocityDirection.Crouching;
			}
			else
			{
				if (ALS_Stance == EALS_Stance::Standing)
				{
					switch (ALS_Gait)
					{
					case EALS_Gait::Walking:
						return CameraTargets.LookingDirection.Standing.Walk;
					case EALS_Gait::Running:
						return CameraTargets.LookingDirection.Standing.Run;
					case EALS_Gait::Sprinting:
						return CameraTargets.LookingDirection.Standing.Sprint;
					default:
						break;
					}
				}
				else
					return CameraTargets.LookingDirection.Crouching;
			}
		}		
	case EALS_MovementMode::Ragdoll:
		return CameraTargets.Ragdoll;
	default:
		break;
	}


	return FACTALS_CameraSettings::FACTALS_CameraSettings();
}

// Smoothly lerp Current Camera Settings to Target Camera Settings.
void AACTCharacterALS::LerpCameraSettings(float LerpAlpha, USpringArmComponent* TargetSpringArm)
{
	int i = 5;
	FACTALS_CameraSettings targetCamSettings = ChooseCameraSettings();
	TargetSpringArm->TargetArmLength = FMath::Lerp(CurrentCameraSettings.TargetArmLength, targetCamSettings.TargetArmLength, LerpAlpha);
	TargetSpringArm->CameraLagSpeed = FMath::Lerp(CurrentCameraSettings.CameraLagSpeed, targetCamSettings.CameraLagSpeed, LerpAlpha);
	float targetY = RightShoulder ? targetCamSettings.SocketOffset.Y : targetCamSettings.SocketOffset.Y * -1;
	TargetSpringArm->SocketOffset = FMath::Lerp(CurrentCameraSettings.SocketOffset, FVector(targetCamSettings.SocketOffset.X, targetY, targetCamSettings.SocketOffset.Z), LerpAlpha);

	return;
}

// ------------------ Ragdoll System Functions

// This trace determines if the Ragdoll is on the ground and will offset the Actors location, preventing the Capsule from penetrating the floor if too close.
FTransform AACTCharacterALS::CalculateActorLocationAndRotationInRagdoll(FRotator RagdollRotation, FVector RagdollLocationInput)
{
	FHitResult OutHit;
	FVector End = FVector(RagdollLocationInput.X, RagdollLocationInput.Y, RagdollLocationInput.Z - GetCapsuleComponent()->GetScaledCapsuleHalfHeight());
	RagdollOnGround = GetWorld()->LineTraceSingleByChannel(OutHit, RagdollLocationInput, End, ECollisionChannel::ECC_Visibility);	// Debugging with ShowTracesVariable is missing

	FTransform returnValue;
	float locationZ = GetCapsuleComponent()->GetScaledCapsuleHalfHeight() - FMath::Abs(OutHit.ImpactPoint.Z - OutHit.TraceStart.Z) + 2;
	locationZ = RagdollOnGround ? RagdollLocationInput.Z + locationZ : RagdollLocationInput.Z;
	returnValue.SetLocation(FVector(RagdollLocationInput.X, RagdollLocationInput.Y, locationZ));

	return FTransform::FTransform();
}

// ------------------ Input Functions
void AACTCharacterALS::PlayerMovementInput(bool IsForwardAxis)
{
	FVector moveVector;
	if (IsForwardAxis)
	{
		
		//AController* temp = GetController();
		//FVector temp2 = temp->GetActorForwardVector();684645
		switch (ALS_MovementMode)
		{
		case EALS_MovementMode::Grounded:
		case EALS_MovementMode::Falling:
			AddMovementInput(GetController()->GetRootComponent()->GetForwardVector(), ForwardAxisValue);
			break;
		case EALS_MovementMode::Ragdoll:
			moveVector = (GetController()->GetRootComponent()->GetForwardVector() * ForwardAxisValue + GetController()->GetRootComponent()->GetRightVector() * RightAxisValue);
			moveVector.Normalize();
			GetCharacterMovement()->AddInputVector(moveVector, false);
			
			switch (ALS_Gait)
			{
			case EALS_Gait::Walking:
			case EALS_Gait::Running:
				moveVector = moveVector * 500;
				break;
			case EALS_Gait::Sprinting:
				moveVector = moveVector * 800;
				break;
			default:
				break;
			}

			moveVector.Y = moveVector.Y * -1;
			GetMesh()->AddTorqueInRadians(moveVector, PelvisBone, true);
			break;
		default:
			break;
		}
	}
	else
	{
		switch (ALS_MovementMode)
		{
		case EALS_MovementMode::Grounded:
		case EALS_MovementMode::Falling:
			AddMovementInput(GetController()->GetRootComponent()->GetRightVector(), RightAxisValue);
			break;
		case EALS_MovementMode::Ragdoll:
			moveVector = (GetController()->GetRootComponent()->GetForwardVector() * ForwardAxisValue + GetController()->GetRootComponent()->GetRightVector() * RightAxisValue);
			moveVector.Normalize();
			GetCharacterMovement()->AddInputVector(moveVector, false);

			switch (ALS_Gait)
			{
			case EALS_Gait::Walking:
			case EALS_Gait::Running:
				moveVector = moveVector * 500;
				break;
			case EALS_Gait::Sprinting:
				moveVector = moveVector * 800;
				break;
			default:
				break;
			}

			moveVector.Y = moveVector.Y * -1;
			GetMesh()->AddTorqueInRadians(moveVector, PelvisBone, true);
			break;
		default:
			break;
		}
	}

	return;
}

// ------------------ Network RPCs

void AACTCharacterALS::Server_SetMovementInput_Implementation(FVector loc_MovementInput)
{
	MovementInput = loc_MovementInput;

	return;
}

bool AACTCharacterALS::Server_SetMovementInput_Validate(FVector loc_MovementInput)
{
	return true;
}

void AACTCharacterALS::Server_SetLookingRotation_Implementation(FRotator loc_LookingRotation)
{
	LookingRotation = loc_LookingRotation;

	return;
}

bool AACTCharacterALS::Server_SetLookingRotation_Validate(FRotator loc_LookingRotation)
{
	return true;
}

void AACTCharacterALS::Server_SetALS_RotationModeEvent_Implementation(EALS_RotationMode loc_ALSRotationMode)
{
	Multicast_SetALS_RotationModeEvent(loc_ALSRotationMode);
	return;
}

bool AACTCharacterALS::Server_SetALS_RotationModeEvent_Validate(EALS_RotationMode loc_ALSRotationMode)
{
	return true;
}

void AACTCharacterALS::Multicast_SetALS_RotationModeEvent_Implementation(EALS_RotationMode loc_ALSRotationMode)
{
	if (IsLocallyControlled())
		return;

	ALS_RotationMode = loc_ALSRotationMode;
	OnALSRotationModeChanged();

	return;
}

bool AACTCharacterALS::Multicast_SetALS_RotationModeEvent_Validate(EALS_RotationMode loc_ALSRotationMode)
{
	return true;
}

void AACTCharacterALS::Server_SetCharacterRotationEvent_Implementation(FRotator loc_TargetRotation, FRotator loc_CharacterRotation)
{
	TargetRotation = loc_TargetRotation;
	CharacterRotation = loc_CharacterRotation;
	SetActorRotation(CharacterRotation, ETeleportType::None);
	return;
}

bool AACTCharacterALS::Server_SetCharacterRotationEvent_Validate(FRotator loc_TargetRotation, FRotator loc_CharacterRotation)
{
	return true;
}

void AACTCharacterALS::Server_UpdateRagdoll_Implementation(FVector loc_RagdollVelocity, FVector loc_RagdollLocation, FRotator loc_ActorRotation, FVector loc_ActorLocation)
{
	RagdollVelocity = loc_RagdollVelocity;
	RagdollLocation = loc_RagdollLocation;
	CharacterRotation = loc_ActorRotation;
	TargetRotation = CharacterRotation;
	Multicast_UpdateRagdoll(loc_ActorLocation);
	return;
}

bool AACTCharacterALS::Server_UpdateRagdoll_Validate(FVector loc_RagdollVelocity, FVector loc_RagdollLocation, FRotator loc_ActorRotation, FVector loc_ActorLocation)
{
	return true;
}

void AACTCharacterALS::Multicast_UpdateRagdoll_Implementation(FVector loc_ActorLocation)
{
	if (!IsLocallyControlled())
		SetActorLocationAndRotation(loc_ActorLocation, CharacterRotation);
	return;
}

bool AACTCharacterALS::Multicast_UpdateRagdoll_Validate(FVector loc_ActorLocation)
{
	return true;
}

void AACTCharacterALS::Server_SetALSGaitEvent_Implementation(EALS_Gait loc_ALS_Gait)
{
	Multicast_SetALSGaitEvent(loc_ALS_Gait);
	return;
}

bool AACTCharacterALS::Server_SetALSGaitEvent_Validate(EALS_Gait loc_ALS_Gait)
{
	return true;
}

void AACTCharacterALS::Multicast_SetALSGaitEvent_Implementation(EALS_Gait loc_ALS_Gait)
{
	if (IsLocallyControlled())
		return;

	ALS_Gait = loc_ALS_Gait;
	OnALSGaitChanged();
	return;
}

bool AACTCharacterALS::Multicast_SetALSGaitEvent_Validate(EALS_Gait loc_ALS_Gait)
{
	return true;
}

void AACTCharacterALS::Server_PlayNetworkedMontage_Implementation(UAnimMontage* MontageToPlay, float InPlayRate, float InTimeToStartMontageAt, bool bStopAllMontages)
{
	Multicast_PlayNetworkedMontage(MontageToPlay, InPlayRate, InTimeToStartMontageAt, bStopAllMontages);
	return;
}

bool AACTCharacterALS::Server_PlayNetworkedMontage_Validate(UAnimMontage* MontageToPlay, float InPlayRate, float InTimeToStartMontageAt, bool bStopAllMontages)
{
	return true;
}

void AACTCharacterALS::Multicast_PlayNetworkedMontage_Implementation(UAnimMontage* MontageToPlay, float InPlayRate, float InTimeToStartMontageAt, bool bStopAllMontages)
{
	if (IsLocallyControlled())
		return;

	UAnimInstance* animInst = GetMesh()->GetAnimInstance();
	if (animInst)
		animInst->Montage_Play(MontageToPlay, InPlayRate, EMontagePlayReturnType::MontageLength, InTimeToStartMontageAt, bStopAllMontages);
}

bool AACTCharacterALS::Multicast_PlayNetworkedMontage_Validate(UAnimMontage* MontageToPlay, float InPlayRate, float InTimeToStartMontageAt, bool bStopAllMontages)
{
	return true;
}

void AACTCharacterALS::Server_SetALS_AimingEvent_Implementation(bool newALS_Aiming)
{
	Multicast_SetALS_AimingEvent(newALS_Aiming);
	return;
}

bool AACTCharacterALS::Server_SetALS_AimingEvent_Validate(bool newALS_Aiming)
{
	return true;
}

void AACTCharacterALS::Multicast_SetALS_AimingEvent_Implementation(bool newALS_Aiming)
{
	if (IsLocallyControlled())
		return;

	ALS_Aiming = newALS_Aiming;
	OnALSAimingChanged();
	return;
}

bool AACTCharacterALS::Multicast_SetALS_AimingEvent_Validate(bool newALS_Aiming)
{
	return true;
}

void AACTCharacterALS::Server_ToRagdollEvent_Implementation()
{
	Multicast_ToRagdollEvent();

	return;
}

bool AACTCharacterALS::Server_ToRagdollEvent_Validate()
{
	return true;
}

void AACTCharacterALS::Multicast_ToRagdollEvent_Implementation()
{
	// Disable Movement Replication (prevents bugs in networked game)
	SetReplicateMovement(false);

	// Set Movment Mode to None and ALS Movement Mode to Ragdoll
	GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_None, 0);
	SetALSMovementModeEvent(EALS_MovementMode::Ragdoll);

	// Disable Capsule Collision
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	// Enable Mesh Collision
	GetMesh()->SetCollisionObjectType(ECollisionChannel::ECC_PhysicsBody);
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);

	// Simulate physics on all bodies below pelvis
	GetMesh()->SetAllBodiesBelowSimulatePhysics(PelvisBone, true, true);

	return;
}

bool AACTCharacterALS::Multicast_ToRagdollEvent_Validate()
{
	return true;
}

void AACTCharacterALS::Server_UnRagdollEvent_Implementation(bool newRagdollOnGround)
{
	Multicast_UnRagdollEvent(newRagdollOnGround);
	return;
}

bool AACTCharacterALS::Server_UnRagdollEvent_Validate(bool newRagdollOnGround)
{
	return true;
}

void AACTCharacterALS::Multicast_UnRagdollEvent_Implementation(bool newRagdollOnGround)
{
	RagdollOnGround = newRagdollOnGround;
	SetReplicateMovement(true);

	// Set Movement Mode to Falling or Walking depending on if the Ragdoll is on the Ground
	if (RagdollOnGround)
		GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Walking, 0);
	else
		GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Falling, 0);

	// Set Velocity to Ragdoll Velocity
	GetCharacterMovement()->Velocity = RagdollVelocity;

	// Save Pose Snapshot (used for blending) and play Get Up Animation (if ragdoll is on the ground)
	// Send value to AnimBP through Interface
	IALS_InterfaceACT* AnimBPInterface = Cast<IALS_InterfaceACT>(GetMesh()->GetAnimInstance());
	if (AnimBPInterface)
		AnimBPInterface->SavePoseSnapshot_BPI(RagdollPoseSnapshot);

	if (RagdollOnGround && AnimBPInterface)
		AnimBPInterface->Play_GetUp_AnimBPI((GetMesh()->GetSocketRotation(PelvisBone)).Roll > 0);

	// Enable Capsule Collision
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);

	// Disable Mesh Collision
	GetMesh()->SetCollisionObjectType(ECollisionChannel::ECC_Pawn);
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::QueryOnly);

	// Un-Simulate all bodies
	GetMesh()->SetAllBodiesSimulatePhysics(false);

	return;
}

bool AACTCharacterALS::Multicast_UnRagdollEvent_Validate(bool newRagdollOnGround)
{
	return true;
}

// ------------------ Custom Events
void AACTCharacterALS::SetALSMovementModeEvent(EALS_MovementMode loc_ALSMovementMode)
{
	if (ALS_MovementMode == loc_ALSMovementMode)
		return;

	ALS_PrevMovementMode = ALS_MovementMode;
	ALS_MovementMode = loc_ALSMovementMode;
	OnALSMovementModeChanged();

	return;
}

// Set Target Camera Settings based on current state and use a Timeline and Custom Curve to lerp between Current and Target Camera Settings
void AACTCharacterALS::UpdateCameraEvent(/*UCurveFloat* LerpCurve*/)
{
	/*if (!LerpCurve)
		return;*/

	CurrentCameraSettings.TargetArmLength = TP_SpringArm->TargetArmLength;
	CurrentCameraSettings.CameraLagSpeed = TP_SpringArm->CameraLagSpeed;
	CurrentCameraSettings.SocketOffset = TP_SpringArm->SocketOffset;

	// Timeline was used to animate camera, is it really neccesary?
	// https://wiki.unrealengine.com/Timeline_in_c%2B%2B
	//UTimelineComponent* CameraTargetTimeline;
	//CameraTargetTimeline = NewObject<UTimelineComponent>(this, FName("TimelineAnimation"));
	//CameraTargetTimeline->CreationMethod = EComponentCreationMethod::UserConstructionScript; // Indicate it comes from a blueprint so it gets cleared when we rerun construction scripts
	//this->BlueprintCreatedComponents.Add(CameraTargetTimeline); // Add to array so it gets saved
	//CameraTargetTimeline->SetNetAddressable();	// This component has a stable name that can be referenced for replication
	//CameraTargetTimeline->SetPropertySetObject(this);	 // Set which object the timeline should drive properties on
	//CameraTargetTimeline->SetDirectionPropertyName(FName("TimelineDirection"));
	//
	//CameraTargetTimeline->SetFloatCurve(LerpCurve,"LerpAlpha");
	////CameraTargetTimeline->SetTimelineLength(LerpCurve->GetTimeRange())

	LerpCameraSettings(1, TP_SpringArm);

	return;
}

void AACTCharacterALS::SetALSViewModeEvent(EALS_ViewMode loc_ALSViewMode)
{
	if (ALS_ViewMode == loc_ALSViewMode)
		return;

	ALS_ViewMode = loc_ALSViewMode;
	OnALSViewModeChanged();

	return;
}

void AACTCharacterALS::SetALS_RotationModeEvent(EALS_RotationMode loc_ALSRotationMode)
{
	if (ALS_RotationMode == loc_ALSRotationMode)
		return;

	ALS_RotationMode = loc_ALSRotationMode;

	Server_SetALS_RotationModeEvent(loc_ALSRotationMode);
	OnALSRotationModeChanged();

	return;
}

void AACTCharacterALS::SetALS_GaitEvent(EALS_Gait newALS_Gait)
{
	if (ALS_Gait != newALS_Gait)
		ALS_Gait = newALS_Gait;
	Server_SetALSGaitEvent(ALS_Gait);
	OnALSGaitChanged();

	return;
}

void AACTCharacterALS::SetALS_StanceEvent(EALS_Stance newALS_Stance)
{
	if (ALS_Stance != newALS_Stance)
		ALS_Stance = newALS_Stance;
	OnALSStanceChanged();

	return;
}

void AACTCharacterALS::SetALS_AimingEvent(bool newALS_Aiming)
{
	if (ALS_Aiming != newALS_Aiming)
	{
		ALS_Aiming = newALS_Aiming;
		Server_SetALS_AimingEvent(newALS_Aiming);
		OnALSAimingChanged();
	}

	return;
}

void AACTCharacterALS::ToRagdollEvent()
{
	Server_ToRagdollEvent();
	return;
}

void AACTCharacterALS::UnRagdollEvent()
{
	Server_UnRagdollEvent(RagdollOnGround);
	return;
}

// Interface Events
void AACTCharacterALS::AddCharacterRotation_BPI_Implementation(FRotator AddAmount)
{
	if (!IsLocallyControlled())
		return;

	print(FColor::Magenta, "AddCharacterRotation_BPI_Implementation", 10);

	// Delay rotation so it rotates on next frame, allowing AnimBP to update afterwards.
	// GetWorldTimerManager().SetTimer(onAddCharacterRotation, this, &AACTCharacterALS::LandedDelayed, 0.2, false);
	// Delay
	// Delay
	AddCharacterRotation(AddAmount);

	return;
}

// Input Functions
void AACTCharacterALS::InputMoveFwd(float axisValue)
{
	ForwardAxisValue = axisValue;
	PlayerMovementInput(true);
	return;
}

void AACTCharacterALS::InputMoveRight(float axisValue)
{
	RightAxisValue = axisValue;
	PlayerMovementInput(false);
	return;
}

void AACTCharacterALS::InputLookUp(float axisValue)
{
	AddControllerPitchInput(axisValue * LookUpDownRate * FApp::GetDeltaTime());
	return;
}

void AACTCharacterALS::InputLookRight(float axisValue)
{
	AddControllerYawInput(axisValue * LookLeftRightRate * FApp::GetDeltaTime());
	return;
}

void AACTCharacterALS::InputJumpActionPressed()
{
	if (ALS_Stance == EALS_Stance::Standing)
	{
		if (!IsPlayingRootMotion())
			Jump();
	}
	else
	{
		UnCrouch();
	}
	return;
}

void AACTCharacterALS::InputJumpActionReleased()
{
	StopJumping();
	return;
}

void AACTCharacterALS::InputStanceAction()
{
	if (ALS_MovementMode == EALS_MovementMode::Grounded)
	{
		if (ALS_Stance == EALS_Stance::Standing)
			Crouch();
		else
			UnCrouch();
	}

	return;
}

void AACTCharacterALS::InputWalkAction()
{
	if (ALS_Gait == EALS_Gait::Running)
		SetALS_GaitEvent(EALS_Gait::Walking);
	else
		SetALS_GaitEvent(EALS_Gait::Running);

	return;
}

void AACTCharacterALS::InputSprintActionPressed()
{
	ShouldSprint = true;
	if (ALS_Gait == EALS_Gait::Walking)
		SetALS_GaitEvent(EALS_Gait::Running);

	return;
}

void AACTCharacterALS::InputSprintActionReleased()
{
	ShouldSprint = false;

	return;
}

void AACTCharacterALS::InputRotationMode()
{
	if (ALS_RotationMode == EALS_RotationMode::VelocityDirection)
		SetALS_RotationModeEvent(EALS_RotationMode::LookingDirection);
	else
		SetALS_RotationModeEvent(EALS_RotationMode::VelocityDirection);

	return;
}

void AACTCharacterALS::InputAimActionPressed()
{
	if (ALS_RotationMode == EALS_RotationMode::VelocityDirection)
		SetALS_RotationModeEvent(EALS_RotationMode::LookingDirection);
		SetALS_AimingEvent(true);
	return;
}

void AACTCharacterALS::InputAimActionReleased()
{
	SetALS_AimingEvent(false);
	return;
}

void AACTCharacterALS::InputRagdollAction()
{
	switch (ALS_MovementMode)
	{
	case EALS_MovementMode::Grounded:
	case EALS_MovementMode::Falling:
		ToRagdollEvent();
		break;
	case EALS_MovementMode::Ragdoll:
		UnRagdollEvent();
		break;
	default:
		break;
	}

	return;
}

void AACTCharacterALS::InputCameraAction()
{
	if (ALS_ViewMode == EALS_ViewMode::ThirdPerson)
		SetALSViewModeEvent(EALS_ViewMode::FirstPerson);
	else
		SetALSViewModeEvent(EALS_ViewMode::ThirdPerson);
	UpdateCameraEvent();

	return;
}

void AACTCharacterALS::InputShowSettings()
{
	ShowSettings = !ShowSettings;
	ToggleCapsuleAndArrowVisibility(ShowSettings);
	// Server Func missing
	return;
}

void AACTCharacterALS::InputShowTraces()
{
	ShowTraces = !ShowTraces;

	// Send value to AnimBP through Interface
	IALS_InterfaceACT* AnimBPInterface = Cast<IALS_InterfaceACT>(GetMesh()->GetAnimInstance());
	if (AnimBPInterface)
		AnimBPInterface->ShowTraces_BPI(ShowTraces);

	// Send value to PostProcessInstance (IK_AnimBP) through Interface
	IALS_InterfaceACT* PostProcessInterface = Cast<IALS_InterfaceACT>(GetMesh()->GetPostProcessInstance());
	if (PostProcessInterface)
		PostProcessInterface->ShowTraces_BPI(ShowTraces);

	return;
}