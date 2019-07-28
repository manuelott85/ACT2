// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "FunctionLibrary.generated.h"

UENUM(BlueprintType)
enum class EALS_Gait : uint8
{
	Walking 	UMETA(DisplayName = "Walking"),
	Running 	UMETA(DisplayName = "Running"),
	Sprinting	UMETA(DisplayName = "Sprinting")
};

UENUM(BlueprintType)
enum class EALS_MovementMode : uint8
{
	None 		UMETA(DisplayName = "None"),
	Grounded 	UMETA(DisplayName = "Grounded"),
	Falling 	UMETA(DisplayName = "Falling"),
	Ragdoll		UMETA(DisplayName = "Ragdoll")
};

UENUM(BlueprintType)
enum class EALS_RotationMode : uint8
{
	VelocityDirection 	UMETA(DisplayName = "VelocityDirection"),
	LookingDirection	UMETA(DisplayName = "LookingDirection")
};

UENUM(BlueprintType)
enum class EALS_Stance : uint8
{
	Standing 	UMETA(DisplayName = "Standing"),
	Crouching	UMETA(DisplayName = "Crouching")
};

UENUM(BlueprintType)
enum class EALS_ViewMode : uint8
{
	ThirdPerson 	UMETA(DisplayName = "ThirdPerson"),
	FirstPerson		UMETA(DisplayName = "FirstPerson")
};

UENUM(BlueprintType)
enum class ECardinalDirection : uint8
{
	North 	UMETA(DisplayName = "North"),
	East 	UMETA(DisplayName = "East"),
	West 	UMETA(DisplayName = "West"),
	South	UMETA(DisplayName = "South")
};

UENUM(BlueprintType)
enum class EFootstepType : uint8
{
	Step 	UMETA(DisplayName = "Step"),
	Walk 	UMETA(DisplayName = "Walk"),
	Run 	UMETA(DisplayName = "Run"),
	Sprint 	UMETA(DisplayName = "Sprint"),
	Pivot 	UMETA(DisplayName = "Pivot"),
	Jump 	UMETA(DisplayName = "Jump"),
	Land	UMETA(DisplayName = "Land")
};

UENUM(BlueprintType)
enum class EIdleEntryState : uint8
{
	N_Idle 		UMETA(DisplayName = "N_Idle"),
	LF_Idle		UMETA(DisplayName = "LF_Idle"),
	RF_Idle		UMETA(DisplayName = "RF_Idle"),
	CLF_Idle 	UMETA(DisplayName = "CLF_Idle"),
	CRF_Idle	UMETA(DisplayName = "CRF_Idle")
};

UENUM(BlueprintType)
enum class ELocomotionState : uint8
{
	None 		UMETA(DisplayName = "None"),
	NotMoving	UMETA(DisplayName = "NotMoving"),
	Moving		UMETA(DisplayName = "Moving"),
	Pivot 		UMETA(DisplayName = "Pivot"),
	Stopping	UMETA(DisplayName = "Stopping")
};

UENUM(BlueprintType)
enum class EMovementDirection : uint8
{
	Forwards 	UMETA(DisplayName = "Forwards"),
	Backwards	UMETA(DisplayName = "Backwards")
};

UENUM(BlueprintType)
enum class EWeaponType : uint8
{
	Unarmed 	UMETA(DisplayName = "Unarmed"),
	Pistol		UMETA(DisplayName = "Pistol"),
	Rifle		UMETA(DisplayName = "Rifle"),
	Launcher 	UMETA(DisplayName = "Launcher"),
	Hammer		UMETA(DisplayName = "Hammer")
};

USTRUCT(BlueprintType)
struct FACTALS_CameraSettings
{
	GENERATED_BODY()

		UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float TargetArmLength = 0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float CameraLagSpeed = 0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FVector SocketOffset = FVector::ZeroVector;
};

USTRUCT(BlueprintType)
struct FACTALS_CameraSettings_Gait
{
	GENERATED_BODY()

		UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FACTALS_CameraSettings Walk;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FACTALS_CameraSettings Run;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FACTALS_CameraSettings Sprint;
};

USTRUCT(BlueprintType)
struct FACTALS_CameraSettings_Stance
{
	GENERATED_BODY()

		UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FACTALS_CameraSettings_Gait Standing;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FACTALS_CameraSettings Crouching;
};

USTRUCT(BlueprintType)
struct FACTALS_Camera_Targets
{
	GENERATED_BODY()

		UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FACTALS_CameraSettings_Stance VelocityDirection;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FACTALS_CameraSettings_Stance LookingDirection;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FACTALS_CameraSettings Aiming;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FACTALS_CameraSettings Ragdoll;
};

USTRUCT(BlueprintType)
struct FACTALS_PivotParams
{
	GENERATED_BODY()

		UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float PivotDirection = 0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		EMovementDirection CompletedMovementDirection = EMovementDirection::Forwards;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float CompletedStartTime = 0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		EMovementDirection InterruptedMovementDirection = EMovementDirection::Forwards;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float InterruptedStartTime = 0;
};

USTRUCT(BlueprintType)
struct FACTTurnAnims
{
	GENERATED_BODY()

		UPROPERTY(EditAnywhere, BlueprintReadWrite)
		UAnimMontage*  Turn_L_Anim = NULL;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		UAnimMontage*  Turn_R_Anim = NULL;
};

/**
 * 
 */
UCLASS(Blueprintable)
class ACT_API UFunctionLibrary : public UObject
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintCallable, Category = "AAAManuel")
	static TArray<FColor> getAverageValueFromAllColors(UTextureRenderTarget* TextureRenderTarget);
};
