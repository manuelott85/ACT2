// Fill out your copyright notice in the Description page of Project Settings.


#include "HealthComponent.h"

#include "UnrealNetwork.h"
#include "Runtime/Engine/Classes/Engine/World.h"
#include "Runtime/Engine/Public/TimerManager.h"

#include "EngineGlobals.h"
#include "Runtime/Engine/Classes/Engine/Engine.h"

#define print(color, text) if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 10, color, text)
// print(FColor::Magenta, FString::FromInt(temp));
// print(FColor::Blue, FString::SanitizeFloat(temp));

// Sets default values for this component's properties
UHealthComponent::UHealthComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	SetIsReplicated(true);

	for (int i = 0; i < amountOfSegments; i++)
		currentHealth.Push(maxHealthPerSegment);
}


// Called when the game starts
void UHealthComponent::BeginPlay()
{
	Super::BeginPlay();

}


// Called every frame
void UHealthComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	
	// regenerate health
	if (GetOwner()->GetLocalRole() == ROLE_Authority && bShouldRegenerate)
	{
		Server_AddHealth(amountToRegeneratePerSec * DeltaTime, true);

		bShouldRegenerate = false;
		for (int i = 0; i < currentHealth.Num(); ++i)
		{
			if (currentHealth[i] != maxHealthPerSegment && currentHealth[i] != 0)
				bShouldRegenerate = true;
		}
	}
}

void UHealthComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	// List of every replicated variable, including a condition if needed
	DOREPLIFETIME(UHealthComponent, currentHealth);
}

void UHealthComponent::ResetRegeneration()
{
	bShouldRegenerate = false;
	GetWorld()->GetTimerManager().ClearTimer(regenerationTimer);
	GetWorld()->GetTimerManager().SetTimer(regenerationTimer, this, &UHealthComponent::RegenerateHealth, timeUntilRegenerationStarts, false);

	return;
}

void UHealthComponent::RegenerateHealth()
{
	bShouldRegenerate = true;
	return;
}

void UHealthComponent::RequestRemoveHealth(float amountHPtoRemove)
{
	Server_RemoveHealth(amountHPtoRemove);
	return;
}

void UHealthComponent::Server_RemoveHealth_Implementation(float amountHPtoRemove)
{
	for (int i = currentHealth.Num() - 1; i > -1; i--)
	{
		if (currentHealth[i] > 0)
		{
			if (amountHPtoRemove > currentHealth[i])
			{
				amountHPtoRemove = amountHPtoRemove - currentHealth[i];
				currentHealth[i] = 0;
			}
			else
			{
				currentHealth[i] = currentHealth[i] - amountHPtoRemove;
				break;
			}
		}
	}

	ResetRegeneration();

	for (int i = 0; i < currentHealth.Num(); i++)
		print(FColor::Red, "Segment #" + FString::FromInt(i) + " - " + FString::SanitizeFloat(currentHealth[i]) + " HP");

	if (currentHealth[0] == 0)
		print(FColor::Red, "Player " + GetName() + " is dead!");

	return;
}

bool UHealthComponent::Server_RemoveHealth_Validate(float amountHPtoRemove)
{
	return true;
}

void UHealthComponent::RequestAddHealth(float amountHPtoAdd)
{
	Server_AddHealth(amountHPtoAdd);
	return;
}

void UHealthComponent::Server_AddHealth_Implementation(float amountHPtoAdd, bool bRestrictedToSegment)
{
	for (int i = 0; i < currentHealth.Num(); i++)
	{
		if (bRestrictedToSegment && currentHealth[i] == 0)
		{
			if (i == 0)
				bShouldRegenerate = false;
			break;
		}

		if (currentHealth[i] < maxHealthPerSegment)
		{
			float missingHP4curSegment = maxHealthPerSegment - currentHealth[i];
			if (amountHPtoAdd > missingHP4curSegment)
			{
				currentHealth[i] = maxHealthPerSegment;
				amountHPtoAdd = amountHPtoAdd - missingHP4curSegment;
			}
			else
			{
				currentHealth[i] = currentHealth[i] + amountHPtoAdd;
				break;
			}
		}
	}

	for (int i = 0; i < currentHealth.Num(); i++)
		print(FColor::Green, "Segment #" + FString::FromInt(i) + " - " + FString::SanitizeFloat(currentHealth[i]) + " HP");

	return;
}

bool UHealthComponent::Server_AddHealth_Validate(float amountHPtoAdd, bool bRestrictedToSegment)
{
	return true;
}