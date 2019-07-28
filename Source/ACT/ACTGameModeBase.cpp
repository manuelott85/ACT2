// Fill out your copyright notice in the Description page of Project Settings.

#include "ACTGameModeBase.h"
#include "ACTCharacter.h"
#include "Runtime/CoreUObject/Public/UObject/ConstructorHelpers.h"

AACTGameModeBase::AACTGameModeBase()
{
	// Set default classes
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawn(TEXT("/Game/Blueprints/BP_MainCharacter"));
	//static ConstructorHelpers::FClassFinder<APawn> PlayerPawn(TEXT("/Game/AdvancedLocomotionV3/Blueprints/MannequinCharacter"));
	//static ConstructorHelpers::FClassFinder<APawn> PlayerPawn(TEXT("/Game/Blueprints/BP_MainCharacter_SWAT"));
	static ConstructorHelpers::FClassFinder<APlayerController> PlayerController(TEXT("/Game/Blueprints/BP_PlayerController"));
	DefaultPawnClass = PlayerPawn.Class;
	GameSessionClass;
	GameStateClass;
	HUDClass;
	PlayerControllerClass = PlayerController.Class;
	PlayerStateClass;
	SpectatorClass;

	// Set GameModeBase parameter
	DefaultPlayerName = FText::FromString("Combatant");
	bUseSeamlessTravel = true;
}

void AACTGameModeBase::StartPlay()
{
	Super::StartPlay();
}

void AACTGameModeBase::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);

	ConnectedPlayerControllerList.Add(NewPlayer);	// Add every connection's playerController in one list
}