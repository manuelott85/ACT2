#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "HealthComponent.generated.h"

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class ACT_API UHealthComponent : public UActorComponent {
	GENERATED_BODY()

private:
	bool bShouldRegenerate = true;
	FTimerHandle regenerationTimer;

protected:
	UPROPERTY(EditAnywhere, BlueprintGetter = GetMaxHealthPerSegment, BlueprintSetter = SetMaxHealthPerSegment)
		float maxHealthPerSegment = 25;
	UPROPERTY(EditAnywhere, BlueprintGetter = GetNumberOfSegments, BlueprintSetter = SetNumberOfSegments)
		float numberOfSegments = 4;
	UPROPERTY(EditAnywhere, BlueprintGetter = GetCanRegenerate, BlueprintSetter = SetCanRegenerate)
		bool bCanRegenerate = true;
	UPROPERTY(EditAnywhere, BlueprintGetter = GetTimeUntilRegenerationStarts, BlueprintSetter = SetTimeUntilRegenerationStarts)
		float timeUntilRegenerationStarts = 3;
	UPROPERTY(EditAnywhere, BlueprintGetter = GetAmountToRegeneratePerSec, BlueprintSetter = SetAmountToRegeneratePerSec)
		float amountToRegeneratePerSec = 3;
	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly)
		TArray<float> currentHealthPerSegments;

private:
	virtual void BeginPlay() override;
	bool ShouldRegenerate();
	void AddHealthSegments(int amount);
	float RemoveHealthFromSegmentReturnRemaining(int index, float damage);
	float AddHealthToSegmentReturnRemaining(int index, float amountHPtoAdd);
	void RegenerateHealth(float deltaTime);

protected:
	void StopRegeneration();
	void StartRegeneration();
	void StartRegenerationWithTimer(float timeInSec);
	void DebugPrint(FColor color);

public:
	UHealthComponent();
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION(BlueprintGetter)
		float GetMaxHealthPerSegment() const { return maxHealthPerSegment; }
	UFUNCTION(BlueprintSetter)
		void SetMaxHealthPerSegment(float val) { maxHealthPerSegment = val; }
	UFUNCTION(BlueprintGetter)
		float GetNumberOfSegments() const { return numberOfSegments; }
	UFUNCTION(BlueprintSetter)
		void SetNumberOfSegments(float val) { numberOfSegments = val; }
	UFUNCTION(BlueprintGetter)
		bool GetCanRegenerate() const { return bCanRegenerate; }
	UFUNCTION(BlueprintSetter)
		void SetCanRegenerate(bool val) { bCanRegenerate = val; }
	UFUNCTION(BlueprintGetter)
		float GetTimeUntilRegenerationStarts() const { return timeUntilRegenerationStarts; }
	UFUNCTION(BlueprintSetter)
		void SetTimeUntilRegenerationStarts(float val) { timeUntilRegenerationStarts = val; }
	UFUNCTION(BlueprintGetter)
		float GetAmountToRegeneratePerSec() const { return amountToRegeneratePerSec; }
	UFUNCTION(BlueprintSetter)
		void SetAmountToRegeneratePerSec(float val) { amountToRegeneratePerSec = val; }

	UFUNCTION()
		float GetCurTotalHealth();

	UFUNCTION()
		void RequestRemoveHealth(float amountHPtoRemove);
	UFUNCTION(Server, unreliable)
		void Server_RemoveHealth(float amountHPtoRemove);
	UFUNCTION()
		void RequestAddHealth(float amountHPtoAdd);
	UFUNCTION(Server, unreliable)
		void Server_AddHealth(float amountHPtoAdd, bool bRestrictedToSegment = false);
};
