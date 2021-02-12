#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

#include "ACTProjectile_Base.generated.h"

UCLASS()
class ACT_API AACTProjectile_Base : public AActor {
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float damage = 27;

public:
	AACTProjectile_Base();
};
