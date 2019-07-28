// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "FunctionLibrary.h"

#include "ALS_InterfaceACT.generated.h"

class UCameraShake;

// This class does not need to be modified.
UINTERFACE(MinimalAPI, Blueprintable)
class UALS_InterfaceACT : public UInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	UALS_InterfaceACT(const FObjectInitializer& ObjectInitializer);
};

/**
 * 
 */
class ACT_API IALS_InterfaceACT
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	//classes using this interface MUST implement this function
	//UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "MyCategory")
	//bool ReactToHighNoon();

	//classes using this interface may implement this function
	//UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "MyCategory")
	//bool ReactToMidnight();

	// Default Functions
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Default")
	void Play_GetUp_AnimBPI(bool FaceDown);
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Default")
	void SavePoseSnapshot_BPI(FName PoseName);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Default")
	void AddCharacterRotation_BPI(FRotator AddAmount);
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Default")
	void ShowTraces_BPI(bool ShowTraces);
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Default")
	void CameraShake_BPI(TSubclassOf<UCameraShake> ShakeClass,float Scale);
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Default")
	void SetRF_BPI(bool RF);

	// ALS Value Functions
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "ALS Values")
	void SetALS_GaitBPI(EALS_Gait ALS_Gait);
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "ALS Values")
	void SetALS_StanceBPI(EALS_Stance ALS_Stance);
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "ALS Values")
	void SetALS_RotationModeBPI(EALS_RotationMode ALS_RotationMode);
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "ALS Values")
	void SetALS_MovementModeBPI(EALS_MovementMode ALS_MovementMode);
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "ALS Values")
	void SetALS_AimingBPI(bool Aiming);
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "ALS Values")
	void SetALS_ViewModeBPI(EALS_ViewMode ViewMode);

	// ALS CharacterMovementSetting Functions
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "ALS CharacterMovementSettings")
	void SetWalkingSpeedBPI(float WalkingSpeed);
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "ALS CharacterMovementSettings")
	void SetRunningSpeedBPI(float RunningSpeed);
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "ALS CharacterMovementSettings")
	void SetSprintingSpeedBPI(float SprintingSpeed);
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "ALS CharacterMovementSettings")
	void SetCrouchingSpeedBPI(float CrouchingSpeed);

	// Anim Notify Functions
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Anim Notify")
	void AnimNotify_PivotBPI(FACTALS_PivotParams PivotParams);
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Anim Notify")
	void AnimNotify_TurningInPlaceBPI(UAnimMontage* TurnInPlaceMontage, bool ShouldTurnInPlace, bool TurningInPlace, bool TurningRight);
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Anim Notify")
	void AnimNotify_IdleEntryStateBPI(EIdleEntryState IdleEntryState);
};