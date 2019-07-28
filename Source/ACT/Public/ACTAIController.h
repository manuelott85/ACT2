// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "ACTCharacter.h"

#include "ACTAIController.generated.h"

struct SteeringOutput;

// AACTAIController ----------------------------------------------------------------------------------------------------------------
/**
 * 
 */
UCLASS()
class ACT_API AACTAIController : public AAIController
{
	GENERATED_BODY()

public:
	AACTCharacter* pControlledACTCharacter;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	UBehaviorTree* BTAsset;

	/** Yaw input speed scaling */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	float InputYawScale = 1;

	/** Input axes values, accumulated each tick. */
	FRotator RotationInput;

private:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	//virtual void Possess(APawn* InPawn) override;
	//virtual void UnPossess() override;
	virtual void OnPossess(APawn* InPawn) override;
	virtual void OnUnPossess() override;
	
public:
	void moveCharacter(SteeringOutput steering);

	/**
	 * Updates the rotation of player, based on ControlRotation after RotationInput has been applied.
	 * This may then be modified by the PlayerCamera, and is passed to Pawn->FaceRotation().
	 */
	virtual void UpdateRotation(float DeltaTime);
};
