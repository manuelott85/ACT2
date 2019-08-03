// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_BlackboardBase.h"
#include "BTTask_MoveToWithBehavior.generated.h"

class AACTCharacter;

// SteeringOutput ----------------------------------------------------------------------------------------------------------------
struct SteeringOutput
{
	FVector linear = { 0,0,0 };	// this vector needs to be initialized, otherwise the character will not move
	float angular = 0;
	bool bDestinationReached = false;
};

// SteeringOutput ----------------------------------------------------------------------------------------------------------------
struct MoveBehaviorParameter
{
	// Holds the kinematic data for the character and target
	AACTCharacter* pCharacter = nullptr;
	AACTCharacter* pTarget = nullptr;
	FVector targetLoction = { 0,0,0 };

	// character params
	float maxAcceleration;	// linear acceleration (direction)
	float maxAngularAcceleration;	// angular acceleration (rotation)
	float maxSpeed;	// max speed of the character
	float maxRotation;	// max rotation speed

	// radia params
	float targetRadius; // radius at which the movement is considered as succesful
	float slowRadius;	// radius for beginning to slow down
	float timeToTarget;	// timer over which to achieve target speed

	// examples
	// Align (targetRadius: 0.01, slowRadius: 2, TimeToTarget: 50)
};

// UMoveBehavior_Base ----------------------------------------------------------------------------------------------------------------
UCLASS()
class ACT_API UMoveBehavior_Base : public UObject
{
	GENERATED_BODY()

public:
	virtual SteeringOutput getSteering(const MoveBehaviorParameter& moveParam);
	virtual SteeringOutput getSteeringReversed(const MoveBehaviorParameter& moveParam);
};

// UMoveBehavior_SeekFlee ----------------------------------------------------------------------------------------------------------------
UCLASS()
class ACT_API UMoveBehavior_SeekFlee : public UMoveBehavior_Base
{
	GENERATED_BODY()

public:
	// returns the desired steering output
	virtual SteeringOutput getSteering(const MoveBehaviorParameter& moveParam) override;
	virtual SteeringOutput getSteeringReversed(const MoveBehaviorParameter& moveParam) override;
	SteeringOutput getSteeringCombined(const MoveBehaviorParameter& moveParam, bool bFlee = false);
};

// UMoveBehavior_Arrive ----------------------------------------------------------------------------------------------------------------
UCLASS()
class ACT_API UMoveBehavior_Arrive : public UMoveBehavior_Base
{
	GENERATED_BODY()

public:
	// returns the desired steering output
	virtual SteeringOutput getSteering(const MoveBehaviorParameter& moveParam) override;
};

// UMoveBehavior_Align ----------------------------------------------------------------------------------------------------------------
UCLASS()
class ACT_API UMoveBehavior_Align : public UMoveBehavior_Base
{
	GENERATED_BODY()

public:
	// returns the desired steering output
	virtual SteeringOutput getSteering(const MoveBehaviorParameter& moveParam) override;
	virtual SteeringOutput getSteeringReversed(const MoveBehaviorParameter& moveParam) override;
	SteeringOutput getSteeringCombined(const MoveBehaviorParameter& moveParam, bool bFlee = false);
};

// UMoveBehavior_VelocityMatch-----------------------------------------------------------------------------------------------------------------
UCLASS()
class ACT_API UMoveBehavior_VelocityMatch : public UMoveBehavior_Base
{
	GENERATED_BODY()

public:
	// returns the desired steering output
	virtual SteeringOutput getSteering(const MoveBehaviorParameter& moveParam) override;
	//virtual SteeringOutput getSteeringReversed(const MoveBehaviorParameter& moveParam) override;
	//SteeringOutput getSteeringCombined(const MoveBehaviorParameter& moveParam, bool bFlee = false);
};

// UBTTask_MoveToWithBehavior ----------------------------------------------------------------------------------------------------------------
UCLASS()
class ACT_API UBTTask_MoveToWithBehavior : public UBTTask_BlackboardBase
{
	GENERATED_BODY()

public:
	UPROPERTY(Category = Node, EditAnywhere, BlueprintReadWrite)
	TSubclassOf<UMoveBehavior_Base> TMovementBehaviorSelector;

private:
	UPROPERTY()
	UMoveBehavior_Base* pMovementBehavior;

	// radius at which the movement is considered as succesful
	UPROPERTY(Category = Node, EditAnywhere, meta = (ClampMin = "0.0", UIMin = "0.0"))
	float targetRadius = 100;
	// radius for beginning to slow down
	UPROPERTY(Category = Node, EditAnywhere, meta = (ClampMin = "0.0", UIMin = "0.0"))
	float slowRadius = 500;
	// timer over which to achieve target speed
	UPROPERTY(Category = Node, EditAnywhere, meta = (ClampMin = "0.0001", UIMin = "0.0001"))
	float timeToTarget = 50;

	// Enable to use the alternate behavior e.g. flee of seek&flee
	UPROPERTY(Category = Node, EditAnywhere)
	bool bReversed = false;

	SteeringOutput steering;
	MoveBehaviorParameter moveParam;

public:
	UBTTask_MoveToWithBehavior();
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	//virtual EBTNodeResult::Type AbortTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	//virtual void OnTaskFinished(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, EBTNodeResult::Type TaskResult) override;
	virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

	EBTNodeResult::Type executeMovement(UBehaviorTreeComponent& OwnerComp);
	void updateMoveParam(UBehaviorTreeComponent& OwnerComp);
};
