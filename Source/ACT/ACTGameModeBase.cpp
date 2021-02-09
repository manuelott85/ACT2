#include "ACTGameModeBase.h"
#include "ACTCharacter.h"
#include "Runtime/CoreUObject/Public/UObject/ConstructorHelpers.h"
#include "Logger.h"

AACTGameModeBase::AACTGameModeBase() {
	// Get Blueprint References
	static ConstructorHelpers::FClassFinder<APawn> _PlayerPawn(TEXT("/Game/Blueprints/BP_MainCharacter"));
	//static ConstructorHelpers::FClassFinder<APawn> PlayerPawn(TEXT("/Game/AdvancedLocomotionV3/Blueprints/MannequinCharacter"));
	//static ConstructorHelpers::FClassFinder<APawn> PlayerPawn(TEXT("/Game/Blueprints/BP_MainCharacter_SWAT"));
	static ConstructorHelpers::FClassFinder<APlayerController> _PlayerController(TEXT("/Game/Blueprints/BP_PlayerController"));

	// Set default classes
	DefaultPawnClass = _PlayerPawn.Class;
	GameSessionClass;
	GameStateClass;
	HUDClass;
	PlayerControllerClass = _PlayerController.Class;
	PlayerStateClass;
	SpectatorClass;

	// Set GameModeBase parameter
	DefaultPlayerName = FText::FromString("Combatant");
	bUseSeamlessTravel = true;
}

//void AACTGameModeBase::StartPlay() {
//	Super::StartPlay();
//}

//void AACTGameModeBase::PostLogin(APlayerController* NewPlayer) {
//	Super::PostLogin(NewPlayer);
//}
