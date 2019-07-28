// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "ALS_InterfaceACT.h"
#include "FunctionLibrary.h"
#include "ACTCharacterALS.generated.h"

class USceneComponent;
class UArrowComponent;
class USpringArmComponent;
class UCameraComponent;

UCLASS()
class ACT_API AACTCharacterALS : public ACharacter, public IALS_InterfaceACT
{
	GENERATED_BODY()

protected:
	// Arrows
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Arrows")
	USceneComponent* Arrows;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Arrows")
	UArrowComponent* LookingRotationArrow;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Arrows")
	UArrowComponent* TargetRotationArrow;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Arrows")
	UArrowComponent* CharacterRotationArrow;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Arrows")
	UArrowComponent* MovementInputArrow;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Arrows")
	UArrowComponent* LastMovementInputRotationArrow;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Arrows")
	UArrowComponent* VelocityArrow;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Arrows")
	UArrowComponent* LastVelocityRotationArrow;
	
	// Third person camera arm
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	USpringArmComponent* TP_SpringArm;

	// Third person camera
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	UCameraComponent* TP_Camera;

	// First person camera
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	UCameraComponent* FP_Camera;

	// Input Variables
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Advanced Locomotion System|Input")
	bool ShouldSprint = false;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Advanced Locomotion System|Input")
	float LookUpDownRate = 150;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Advanced Locomotion System|Input")
	float LookLeftRightRate = 150;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Advanced Locomotion System|Input")
	float ForwardAxisValue = 0;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Advanced Locomotion System|Input")
	float RightAxisValue = 0;

	// Essential Variables
	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category = "Advanced Locomotion System|Essential")
	FVector MovementInput = FVector::ZeroVector;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Advanced Locomotion System|Essential")
	float AimYawDelta = 0;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Advanced Locomotion System|Essential")
	float AimYawRate = 0;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Advanced Locomotion System|Essential")
	float MovementInputVelocityDifference = 0;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Advanced Locomotion System|Essential")
	float TargetCharacterRotationDifference = 0;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Advanced Locomotion System|Essential")
	float Direction = 0;
	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category = "Advanced Locomotion System|Essential")
	FRotator CharacterRotation = FRotator::ZeroRotator;
	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category = "Advanced Locomotion System|Essential")
	FRotator LookingRotation = FRotator::ZeroRotator;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Advanced Locomotion System|Essential")
	FRotator LastVelocityRotation = FRotator::ZeroRotator;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Advanced Locomotion System|Essential")
	FRotator LastMovementInputRotation = FRotator::ZeroRotator;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Advanced Locomotion System|Essential")
	bool IsMoving = false;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Advanced Locomotion System|Essential")
	bool HasMovementInput = false;

	// State Values
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Advanced Locomotion System|State Values")
	EALS_Gait ALS_Gait = EALS_Gait::Running;
	UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category = "Advanced Locomotion System|State Values")
	EALS_Stance ALS_Stance = EALS_Stance::Standing;
	UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category = "Advanced Locomotion System|State Values")
	EALS_RotationMode ALS_RotationMode = EALS_RotationMode::VelocityDirection;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Advanced Locomotion System|State Values")
	EALS_MovementMode ALS_MovementMode = EALS_MovementMode::Grounded;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Advanced Locomotion System|State Values")
	EALS_MovementMode ALS_PrevMovementMode = EALS_MovementMode::None;
	UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category = "Advanced Locomotion System|State Values")
	EALS_ViewMode ALS_ViewMode = EALS_ViewMode::ThirdPerson;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Advanced Locomotion System|State Values")
	bool ALS_Aiming = false;

	// Rotation System
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Advanced Locomotion System|Rotation System")
	ECardinalDirection CardinalDirection = ECardinalDirection::North;
	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category = "Advanced Locomotion System|Rotation System")
	FRotator TargetRotation = FRotator::ZeroRotator;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Advanced Locomotion System|Rotation System")
	FRotator JumpRotation = FRotator::ZeroRotator;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Advanced Locomotion System|Rotation System")
	float RotationOffset = 0;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Advanced Locomotion System|Rotation System")
	float RotationRateMultiplier = 1;

	// Movement System Variables
	UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category = "Advanced Locomotion System|Movement System")
	float WalkingSpeed = 165;
	UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category = "Advanced Locomotion System|Movement System")
	float RunningSpeed = 375;
	UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category = "Advanced Locomotion System|Movement System")
	float SprintingSpeed = 600;
	UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category = "Advanced Locomotion System|Movement System")
	float CrouchingSpeed = 150;
	UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category = "Advanced Locomotion System|Movement System")
	float WalkingAcceleration = 800;
	UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category = "Advanced Locomotion System|Movement System")
	float RunningAcceleration = 1000;
	UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category = "Advanced Locomotion System|Movement System")
	float WalkingDeceleration = 800;
	UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category = "Advanced Locomotion System|Movement System")
	float RunningDeceleration = 800;
	UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category = "Advanced Locomotion System|Movement System")
	float WalkingGroundFriction = 8;
	UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category = "Advanced Locomotion System|Movement System")
	float RunningGroundFriction = 6;

	// Camera System Variables
	UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category = "Advanced Locomotion System|Camera System")
	FACTALS_Camera_Targets CameraTargets;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Advanced Locomotion System|Camera System")
	FName FirstPersonCameraSocket = "CameraSocket";
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Advanced Locomotion System|Camera System")
	FACTALS_CameraSettings CurrentCameraSettings;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Advanced Locomotion System|Camera System")
	UCurveFloat* CameraLerpCurve;	// needs to be set in the editor
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Advanced Locomotion System|Camera System")
	bool RightShoulder = true;

	// Debug Variables
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Advanced Locomotion System|Debug")
	bool ShowTraces = false;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Advanced Locomotion System|Debug")
	bool ShowSettings = false;

	// Bone Name Variables
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Advanced Locomotion System|Bone Names")
	FName PelvisBone = "pelvis";

	// Ragdoll System Variables
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Advanced Locomotion System|Ragdoll System")
	FName RagdollPoseSnapshot = "RagdollPose";
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Advanced Locomotion System|Ragdoll System")
	bool RagdollOnGround = false;
	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category = "Advanced Locomotion System|Ragdoll System")
	FVector RagdollLocation = FVector::ZeroVector;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Advanced Locomotion System|Ragdoll System")
	FVector RagdollVelocity = FVector::ZeroVector;

	// Timer
	FTimerHandle onLandedTimerHandle;
	FTimerHandle onAddCharacterRotation;

public:
	// Sets default values for this character's properties
	AACTCharacterALS(const FObjectInitializer& ObjectInitializer);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;


	// Essential Functions
	UFUNCTION(BlueprintCallable, Category = "Advanced Locomotion System|Essential")
	void CalculateEssentialVariables();
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Advanced Locomotion System|Essential")
	FVector ChooseVelocity();

	// Debug Functions
	UFUNCTION(BlueprintCallable, Category = "Advanced Locomotion System|Debug")
	void SetArrowValues(bool DoIfInvisible = false);
	UFUNCTION(BlueprintCallable, Category = "Advanced Locomotion System|Debug")
	void ToggleCapsuleAndArrowVisibility(bool Visible = false);
	UFUNCTION(BlueprintCallable, Category = "Advanced Locomotion System|Debug")
	void PrintInfo();

	// OnChanged Functions
	UFUNCTION(BlueprintCallable, Category = "Advanced Locomotion System|On Changed")
	void OnPawnMovementModeChanged(/*EMovementMode PrevMovementMode, */EMovementMode NewMovementMode/*, BYTE PrevCustomMode, BYTE NewCustomMove*/);
	virtual void OnMovementModeChanged(EMovementMode PrevMovementMode, uint8 PreviousCustomMode) override;
	UFUNCTION(BlueprintCallable, Category = "Advanced Locomotion System|On Changed")
	void OnALSMovementModeChanged();
	UFUNCTION(BlueprintCallable, Category = "Advanced Locomotion System|On Changed")
	void OnALSGaitChanged();
	UFUNCTION(BlueprintCallable, Category = "Advanced Locomotion System|On Changed")
	void OnALSStanceChanged();
	virtual void OnStartCrouch(float HalfHeightAdjust, float ScaledHalfHeightAdjust) override;
	virtual void OnEndCrouch(float HalfHeightAdjust, float ScaledHalfHeightAdjust) override;
	UFUNCTION(BlueprintCallable, Category = "Advanced Locomotion System|On Changed")
	void OnALSRotationModeChanged();
	UFUNCTION(BlueprintCallable, Category = "Advanced Locomotion System|On Changed")
	void OnALSAimingChanged();
	UFUNCTION(BlueprintCallable, Category = "Advanced Locomotion System|On Changed")
	void OnALSViewModeChanged();
	virtual void Landed(const FHitResult & Hit) override;
	void LandedDelayed();

	// Rotation System Functions
	UFUNCTION(BlueprintCallable, Category = "Advanced Locomotion System|Rotation System")
	void SetCharacterRotation(FRotator TargetRotationInput, bool InterpRotation, float InterpSpeed);
	UFUNCTION(BlueprintCallable, Category = "Advanced Locomotion System|Rotation System")
	void AddCharacterRotation(FRotator AddAmount);
	UFUNCTION(BlueprintCallable, Category = "Advanced Locomotion System|Rotation System")
	FRotator LookingDirectionWithOffset(float OffsetInterpSpeed, float NEAngle, float NWAngle, float SEAngle, float SWAngle, float Buffer);
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Advanced Locomotion System|Rotation System")
	float CalculateRotationRate(float SlowSpeed, float SlowSpeedRate, float FastSpeed, float FastSpeedRate);
	UFUNCTION(BlueprintCallable, Category = "Advanced Locomotion System|Rotation System")
	void LimitRotation(float AimYawLimit, float InterpSpeed);
	UFUNCTION(BlueprintCallable, Category = "Advanced Locomotion System|Rotation System")
	bool GetCardinalDirection(float Value, float Min, float Max, float Buffer, ECardinalDirection loc_CardinalDirection);

	// Movement System Functions
	UFUNCTION(BlueprintCallable, Category = "Advanced Locomotion System|Movement System")
	void UpdateALSCharacterMovementSettings();
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Advanced Locomotion System|Movement System")
	float ChooseMaxWalkSpeed();
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Advanced Locomotion System|Movement System")
	float ChooseMaxAcceleration();
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Advanced Locomotion System|Movement System")
	float ChooseBrakingDeceleration();
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Advanced Locomotion System|Movement System")
	float ChooseGroundFriction();
	UFUNCTION(BlueprintCallable, Category = "Advanced Locomotion System|Movement System")
	void CustomAcceleration();
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Advanced Locomotion System|Movement System")
	bool CanSprint();

	// Camera System Functions
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Advanced Locomotion System|Camera System")
	FACTALS_CameraSettings ChooseCameraSettings();
	UFUNCTION(BlueprintCallable, Category = "Advanced Locomotion System|Camera System")
	void LerpCameraSettings(float LerpAlpha, USpringArmComponent* TargetSpringArm);

	// Ragdoll System Functions
	UFUNCTION(BlueprintCallable, Category = "Advanced Locomotion System|Ragdoll System")
	FTransform CalculateActorLocationAndRotationInRagdoll(FRotator RagdollRotation, FVector RagdollLocationInput);

	// Input Functions
	UFUNCTION(BlueprintCallable, Category = "Advanced Locomotion System|Input")
	void PlayerMovementInput(bool IsForwardAxis);

	// Network RPCs
	UFUNCTION(Server, unreliable, WithValidation)
	void Server_SetMovementInput(FVector loc_MovementInput);
	UFUNCTION(Server, unreliable, WithValidation)
	void Server_SetLookingRotation(FRotator loc_LookingRotation);
	UFUNCTION(Server, unreliable, WithValidation)
	void Server_SetALS_RotationModeEvent(EALS_RotationMode loc_ALSRotationMode);
	UFUNCTION(NetMulticast, unreliable, WithValidation)
	void Multicast_SetALS_RotationModeEvent(EALS_RotationMode loc_ALSRotationMode);
	UFUNCTION(Server, unreliable, WithValidation)
	void Server_SetCharacterRotationEvent(FRotator loc_TargetRotation, FRotator loc_CharacterRotation);
	UFUNCTION(Server, unreliable, WithValidation)
	void Server_UpdateRagdoll(FVector loc_RagdollVelocity, FVector loc_RagdollLocation, FRotator loc_ActorRotation, FVector loc_ActorLocation);
	UFUNCTION(NetMulticast, unreliable, WithValidation)
	void Multicast_UpdateRagdoll(FVector loc_ActorLocation);
	UFUNCTION(Server, unreliable, WithValidation)
	void Server_SetALSGaitEvent(EALS_Gait loc_ALS_Gait);
	UFUNCTION(NetMulticast, unreliable, WithValidation)
	void Multicast_SetALSGaitEvent(EALS_Gait loc_ALS_Gait);
	UFUNCTION(Server, unreliable, WithValidation, BlueprintCallable, Category = "Advanced Locomotion System|Network RPCs")
	void Server_PlayNetworkedMontage(UAnimMontage* MontageToPlay, float InPlayRate, float InTimeToStartMontageAt, bool bStopAllMontages);
	UFUNCTION(NetMulticast, unreliable, WithValidation)
	void Multicast_PlayNetworkedMontage(UAnimMontage* MontageToPlay, float InPlayRate, float InTimeToStartMontageAt, bool bStopAllMontages);
	UFUNCTION(Server, unreliable, WithValidation)
	void Server_SetALS_AimingEvent(bool newALS_Aiming);
	UFUNCTION(NetMulticast, unreliable, WithValidation)
	void Multicast_SetALS_AimingEvent(bool newALS_Aiming);
	UFUNCTION(Server, unreliable, WithValidation)
	void Server_ToRagdollEvent();
	UFUNCTION(NetMulticast, unreliable, WithValidation)
	void Multicast_ToRagdollEvent();
	UFUNCTION(Server, unreliable, WithValidation)
	void Server_UnRagdollEvent(bool newRagdollOnGround);
	UFUNCTION(NetMulticast, unreliable, WithValidation)
	void Multicast_UnRagdollEvent(bool newRagdollOnGround);


	// Custom Events
	UFUNCTION(BlueprintCallable)
	void SetALSMovementModeEvent(EALS_MovementMode loc_ALSMovementMode);
	UFUNCTION(BlueprintCallable)
	void UpdateCameraEvent(/*UCurveFloat* LerpCurve*/);
	UFUNCTION(BlueprintCallable)
	void SetALSViewModeEvent(EALS_ViewMode loc_ALSViewMode);
	UFUNCTION(BlueprintCallable)
	void SetALS_RotationModeEvent(EALS_RotationMode loc_ALSRotationMode);
	UFUNCTION(BlueprintCallable)
	void SetALS_GaitEvent(EALS_Gait newALS_Gait);
	UFUNCTION(BlueprintCallable)
	void SetALS_StanceEvent(EALS_Stance newALS_Stance);
	UFUNCTION(BlueprintCallable)
	void SetALS_AimingEvent(bool newALS_Aiming);
	UFUNCTION(BlueprintCallable)
	void ToRagdollEvent();
	UFUNCTION(BlueprintCallable)
	void UnRagdollEvent();

	// Interface Events
	virtual void AddCharacterRotation_BPI_Implementation(FRotator AddAmount) override;
	
	// Input Functions
	UFUNCTION(BlueprintCallable, Category = "Advanced Locomotion System|Input")
	void InputMoveFwd(float axisValue);
	UFUNCTION(BlueprintCallable, Category = "Advanced Locomotion System|Input")
	void InputMoveRight(float axisValue);
	UFUNCTION(BlueprintCallable, Category = "Advanced Locomotion System|Input")
	void InputLookUp(float axisValue);
	UFUNCTION(BlueprintCallable, Category = "Advanced Locomotion System|Input")
	void InputLookRight(float axisValue);
	UFUNCTION(BlueprintCallable, Category = "Advanced Locomotion System|Input")
	void InputJumpActionPressed();
	UFUNCTION(BlueprintCallable, Category = "Advanced Locomotion System|Input")
	void InputJumpActionReleased();
	UFUNCTION(BlueprintCallable, Category = "Advanced Locomotion System|Input")
	void InputStanceAction();
	UFUNCTION(BlueprintCallable, Category = "Advanced Locomotion System|Input")
	void InputWalkAction();
	UFUNCTION(BlueprintCallable, Category = "Advanced Locomotion System|Input")
	void InputSprintActionPressed();
	UFUNCTION(BlueprintCallable, Category = "Advanced Locomotion System|Input")
	void InputSprintActionReleased();
	UFUNCTION(BlueprintCallable, Category = "Advanced Locomotion System|Input")
	void InputRotationMode();
	UFUNCTION(BlueprintCallable, Category = "Advanced Locomotion System|Input")
	void InputAimActionPressed();
	UFUNCTION(BlueprintCallable, Category = "Advanced Locomotion System|Input")
	void InputAimActionReleased();
	UFUNCTION(BlueprintCallable, Category = "Advanced Locomotion System|Input")
	void InputRagdollAction();
	UFUNCTION(BlueprintCallable, Category = "Advanced Locomotion System|Input")
	void InputCameraAction();

	UFUNCTION(BlueprintCallable, Category = "Advanced Locomotion System|Input")
	void InputShowSettings();
	UFUNCTION(BlueprintCallable, Category = "Advanced Locomotion System|Input")
	void InputShowTraces();

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
};
