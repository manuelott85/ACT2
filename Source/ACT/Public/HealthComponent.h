// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "HealthComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ACT_API UHealthComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UHealthComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float maxHealthPerSegment = 25;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float amountOfSegments = 4;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bCanRegenerate = true;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float timeUntilRegenerationStarts = 3;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float amountToRegeneratePerSec = 3;
	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly)
	TArray<float> currentHealth;

	bool bShouldRegenerate = true;

	FTimerHandle regenerationTimer;

protected:
	UFUNCTION()
	void ResetRegeneration();
	UFUNCTION()
	void RegenerateHealth();

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION()
	void RequestRemoveHealth(float amountHPtoRemove);
	UFUNCTION(Server, unreliable, WithValidation)
	void Server_RemoveHealth(float amountHPtoRemove);
	UFUNCTION()
	void RequestAddHealth(float amountHPtoAdd);
	UFUNCTION(Server, unreliable, WithValidation)
	void Server_AddHealth(float amountHPtoAdd, bool bRestrictedToSegment = false);
};
