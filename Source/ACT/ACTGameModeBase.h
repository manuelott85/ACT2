#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "ACTGameModeBase.generated.h"

UCLASS()
class ACT_API AACTGameModeBase : public AGameModeBase {
	GENERATED_BODY()

public:
	AACTGameModeBase();
	virtual void StartPlay() override;

	//virtual void PostLogin(APlayerController* NewPlayer) override;
};
