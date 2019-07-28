// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "ACTGameModeBase.generated.h"

/**
 * 
 */
UCLASS()
class ACT_API AACTGameModeBase : public AGameModeBase
{
	GENERATED_BODY()

public:
	AACTGameModeBase();
	virtual void StartPlay() override;
	
	TArray<class APlayerController*> ConnectedPlayerControllerList;

	// Override the PostLogin function
	virtual void PostLogin(APlayerController* NewPlayer) override;
};
