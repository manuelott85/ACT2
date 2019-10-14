// Fill out your copyright notice in the Description page of Project Settings.

#include "ACTAIController.h"
#include "AIController.h"
#include "Runtime/Engine/Classes/Kismet/GameplayStatics.h"
#include "BTTask_MoveToWithBehavior.h"

#include "EngineGlobals.h"
#include "Runtime/Engine/Classes/Engine/Engine.h"

#define print(color, text) if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 10, color, text)
// print(FColor::Magenta, FString::FromInt(temp));
// print(FColor::Blue, FString::SanitizeFloat(temp));

// AACTAIController ----------------------------------------------------------------------------------------------------------------
void AACTAIController::BeginPlay() {
	// Call the base class  
	Super::BeginPlay();

	// Run the Behavior Tree if driven by AI
	if (Role == ROLE_Authority) {
		if (BTAsset) {
			RunBehaviorTree(BTAsset);
		}
	}
	//FocusInformation.Priorities.SetNum(EAIFocusPriority::Gameplay);
	//FocusInformation.Priorities[EAIFocusPriority::Gameplay].Actor = NewFocus;
}

void AACTAIController::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);

	// Update rotation
	UpdateRotation(DeltaTime);
}

void AACTAIController::OnPossess(APawn* InPawn) {
	Super::OnPossess(InPawn);

	AACTCharacter* pControlledPawn = Cast<AACTCharacter>(InPawn);
	if (pControlledPawn != nullptr) {
		pControlledACTCharacter = pControlledPawn;
	}
	print(FColor::Magenta, GetName() + " possed " + pControlledACTCharacter->GetName());
}

void AACTAIController::OnUnPossess() {
	Super::OnUnPossess();

	pControlledACTCharacter = nullptr;
}

void AACTAIController::moveCharacter(SteeringOutput steering) {
	// Update the position and orientation
	//position += velocity * time;
	//orientation += rotation * time;

	// and the velocity and rotation
	//velocity += steering.linear * time;
	//orientation += steering.angular * time;

	// Check for speeding and clip
	//velocity = velocity.GetClampedToMaxSize(maxspeed);
	
	// check the pawn
	if (pControlledACTCharacter == nullptr) {
		return;
	}

	// move the pawn
	pControlledACTCharacter->AddMovementInput(steering.linear, steering.linear.Size());

	// Rotate the Pawn
	RotationInput.Yaw += steering.angular * InputYawScale;
	//print(FColor::Orange, GetName() + " moves " + FString::SanitizeFloat(steering.angular) + " , characterRota: " + FString::SanitizeFloat(pControlledACTCharacter->GetViewRotation().Yaw) + " , controllerRota: " + FString::SanitizeFloat(pControlledACTCharacter->GetControlRotation().Yaw));// +", InputYawScale: " + InputYawScale);
}

void AACTAIController::UpdateRotation(float DeltaTime) {
	if (!RotationInput.IsNearlyZero()) {
		pControlledACTCharacter->FaceRotation(RotationInput, DeltaTime);
	}
}

void AACTAIController::UpdateControlRotation(float DeltaTime, bool bUpdatePawn) {
	print(FColor::Blue, "Test");
	// Copied from AIController:UpdateControlRotation()
	APawn* const MyPawn = GetPawn();
	if (MyPawn) {
		FRotator NewControlRotation = GetControlRotation();

		// Look towards focus
		const FVector FocalPoint = GetFocalPoint();
		if (FAISystem::IsValidLocation(FocalPoint)) {
			NewControlRotation = (FocalPoint - MyPawn->GetPawnViewLocation()).Rotation();
		} else if (bSetControlRotationFromPawnOrientation) {
			NewControlRotation = MyPawn->GetActorRotation();
		}

		SetControlRotation(NewControlRotation);

		if (bUpdatePawn) {
			const FRotator CurrentPawnRotation = MyPawn->GetActorRotation();

			if (CurrentPawnRotation.Equals(NewControlRotation, 1e-3f) == false) {
				MyPawn->FaceRotation(NewControlRotation, DeltaTime);
			}
		}
	}
}