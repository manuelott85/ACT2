#include "HealthComponent.h"

#include "UnrealNetwork.h"
#include "Runtime/Engine/Classes/Engine/World.h"
#include "Runtime/Engine/Public/TimerManager.h"

#include "EngineGlobals.h"
#include "Runtime/Engine/Classes/Engine/Engine.h"

#include "Core/Logger.h"
#include "FunctionLibrary.h"

UHealthComponent::UHealthComponent() {
	PrimaryComponentTick.bCanEverTick = true;

	SetIsReplicated(true);
	return;
}

void UHealthComponent::BeginPlay() {
	Super::BeginPlay();

	SetMaxHealthPerSegment(GetMaxHealthPerSegment());
	AddHealthSegments(GetNumberOfSegments());
	return;
}

void UHealthComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) {
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	RegenerateHealth(DeltaTime);
	return;
}

void UHealthComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const {
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	// List of every replicated variable, including a condition if needed
	DOREPLIFETIME(UHealthComponent, currentHealthPerSegments);
	return;
}

void UHealthComponent::StopRegeneration() {
	bShouldRegenerate = false;
	return;
}

void UHealthComponent::StartRegeneration() {
	bShouldRegenerate = true;
	return;
}

void UHealthComponent::StartRegenerationWithTimer(float timeInSec) {
	StopRegeneration();
	GetWorld()->GetTimerManager().ClearTimer(regenerationTimer);
	GetWorld()->GetTimerManager().SetTimer(regenerationTimer, this, &UHealthComponent::StartRegeneration, timeInSec, false);
	return;
}

void UHealthComponent::AddHealthSegments(int amount) {
	for (int i = 0; i < amount; i++) {
		currentHealthPerSegments.Push(GetMaxHealthPerSegment());
	}
	return;
}

bool UHealthComponent::ShouldRegenerate() {
	for (int i = 0; i < currentHealthPerSegments.Num(); ++i) {
		bool bIsCurrentSegmentAtMaxHealth = currentHealthPerSegments[i] == GetMaxHealthPerSegment();
		bool bIsCurrentSegmentDead = currentHealthPerSegments[i] <= 0;
		if (!bIsCurrentSegmentAtMaxHealth && !bIsCurrentSegmentDead) {
			return true;
		}
	}
	return false;
}

void UHealthComponent::DebugPrint(FColor color) {
	bool bIsDead = currentHealthPerSegments[0] == 0;
	if (bIsDead) {
		ULogger::Print("Player " + GetOwner()->GetName() + " is dead!", FColor::Red);
	} else {
		ULogger::Print("Player " + GetOwner()->GetName() + " - " + FString::SanitizeFloat(GetCurTotalHealth()) + " HP", color, 2.0f);
	}
	return;
}

float UHealthComponent::GetCurTotalHealth() {
	float totalHp = 0.0f;
	for (int i = 0; i < currentHealthPerSegments.Num(); i++) {
		totalHp = totalHp + currentHealthPerSegments[i];
	}
	return totalHp;
}

void UHealthComponent::RequestRemoveHealth(float amountHPtoRemove) {
	Server_RemoveHealth(amountHPtoRemove);
	return;
}

void UHealthComponent::Server_RemoveHealth_Implementation(float amountHPtoRemove) {
	for (int i = currentHealthPerSegments.Num() - 1; i > -1; i--) {
		amountHPtoRemove = RemoveHealthFromSegmentReturnRemaining(i, amountHPtoRemove);
		if (amountHPtoRemove == 0) {
			break;
		}
	}

	StartRegenerationWithTimer(GetTimeUntilRegenerationStarts());
	DebugPrint(FColor::Red);
	return;
}

float UHealthComponent::RemoveHealthFromSegmentReturnRemaining(int index, float damage) {
	float remainingDamage = damage;

	bool bAreHPLeftInCurrentSegment = currentHealthPerSegments[index] > 0;
	if (bAreHPLeftInCurrentSegment) {
		bool bDoesHpToRemoveExceedHpOfCurrentSegment = remainingDamage > currentHealthPerSegments[index];
		if (bDoesHpToRemoveExceedHpOfCurrentSegment) {
			remainingDamage = damage - currentHealthPerSegments[index];
			currentHealthPerSegments[index] = 0;
		} else {
			currentHealthPerSegments[index] = currentHealthPerSegments[index] - damage;
			remainingDamage = 0;
		}
	}
	return remainingDamage;
}

void UHealthComponent::RequestAddHealth(float amountHPtoAdd) {
	Server_AddHealth(amountHPtoAdd);
	return;
}

void UHealthComponent::Server_AddHealth_Implementation(float amountHPtoAdd, bool bRestrictedToSegment) {
	for (int i = 0; i < currentHealthPerSegments.Num(); i++) {
		bool bAreHPLeftInCurrentSegment = currentHealthPerSegments[i] > 0;
		if (bRestrictedToSegment && !bAreHPLeftInCurrentSegment) {
			if (i == 0) {
				StopRegeneration();
			}
			break;
		}
		amountHPtoAdd = AddHealthToSegmentReturnRemaining(i, amountHPtoAdd);
	}

	DebugPrint(FColor::Green);
	return;
}

float UHealthComponent::AddHealthToSegmentReturnRemaining(int index, float amountHPtoAdd) {
	float remainingHealth = amountHPtoAdd;

	bool bCurSegAtMaxHealth = currentHealthPerSegments[index] == GetMaxHealthPerSegment();
	if (!bCurSegAtMaxHealth) {
		float missingHp4CurSegment = GetMaxHealthPerSegment() - currentHealthPerSegments[index];
		bool bDoesAdditionalHpExceedCurSegmentsCapacity = remainingHealth > missingHp4CurSegment;
		if (bDoesAdditionalHpExceedCurSegmentsCapacity) {
			currentHealthPerSegments[index] = GetMaxHealthPerSegment();
			remainingHealth = amountHPtoAdd - missingHp4CurSegment;
		} else {
			currentHealthPerSegments[index] = currentHealthPerSegments[index] + amountHPtoAdd;
			remainingHealth = 0;
		}
	}
	return remainingHealth;
}

void UHealthComponent::RegenerateHealth(float deltaTime) {
	if (GetOwner()->GetLocalRole() == ROLE_Authority && bShouldRegenerate) {
		Server_AddHealth(GetAmountToRegeneratePerSec() * deltaTime, true);
		bShouldRegenerate = ShouldRegenerate();
	}
	return;
}
