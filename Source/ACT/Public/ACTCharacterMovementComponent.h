// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "ACTCharacterMovementComponent.generated.h"

/**
 * 
 */
UCLASS()
class ACT_API UACTCharacterMovementComponent : public UCharacterMovementComponent
{
	GENERATED_BODY()

public:
	/** The maximum ground speed when walking. Also determines maximum lateral speed when falling. */
	UPROPERTY(Replicated, Category = "Character Movement: Walking", EditAnywhere, BlueprintReadWrite, meta = (ClampMin = "0", UIMin = "0"))
	float MaxSprintSpeed = 1200;

	/** The maximum ground speed when walking while aiming down the sights. */
	UPROPERTY(Category = "Character Movement: Walking", EditAnywhere, BlueprintReadWrite, meta = (ClampMin = "0", UIMin = "0"))
	float MaxWalkSpeedAiming = 400;
};
