// Fill out your copyright notice in the Description page of Project Settings.

#include "ACTCharacterMovementComponent.h"
#include <UnrealNetwork.h>

void UACTCharacterMovementComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	// List of every replicated variable, including a condition if needed
	DOREPLIFETIME(UACTCharacterMovementComponent, _maxSprintSpeed);
	DOREPLIFETIME(UACTCharacterMovementComponent, _maxWalkSpeedAiming);
}