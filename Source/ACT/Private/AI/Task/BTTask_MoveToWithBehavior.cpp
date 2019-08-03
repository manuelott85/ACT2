// Fill out your copyright notice in the Description page of Project Settings.

#include "BTTask_MoveToWithBehavior.h"
#include "ACTAIController.h"
#include "Runtime/Engine/Classes/Kismet/GameplayStatics.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Object.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Vector.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Runtime/Engine/Classes/GameFramework/CharacterMovementComponent.h"

#include "EngineGlobals.h"
#include "Runtime/Engine/Classes/Engine/Engine.h"

#define print(color, text) if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 10, color, text)
// print(FColor::Magenta, FString::FromInt(temp));
// print(FColor::Blue, FString::SanitizeFloat(temp));

// UMoveBehavior_Base ----------------------------------------------------------------------------------------------------------------
SteeringOutput UMoveBehavior_Base::getSteering(const MoveBehaviorParameter& moveParam)
{
	print(FColor::Blue, "Wrong Movement behavior was chosen");
	return *(new SteeringOutput);
};

SteeringOutput UMoveBehavior_Base::getSteeringReversed(const MoveBehaviorParameter& moveParam)
{ 
	print(FColor::Blue, "Wrong Movement behavior was chosen");
	return *(new SteeringOutput);
};

// UMoveBehavior_SeekFlee ----------------------------------------------------------------------------------------------------------------
SteeringOutput UMoveBehavior_SeekFlee::getSteering(const MoveBehaviorParameter& moveParam)
{
	return getSteeringCombined(moveParam, false);
}

SteeringOutput UMoveBehavior_SeekFlee::getSteeringReversed(const MoveBehaviorParameter& moveParam)
{
	return getSteeringCombined(moveParam, true);
}

SteeringOutput UMoveBehavior_SeekFlee::getSteeringCombined(const MoveBehaviorParameter& moveParam, bool bFlee)
{
	SteeringOutput steering;	// create the structure to hold our output
	bool bUseTargetVector = false;

	// check needed params
	if (moveParam.pCharacter == nullptr)
	{
		print(FColor::Red, "missing charater");
		return steering;
	}
	if (moveParam.maxAcceleration <= 0)
	{
		print(FColor::Red, "max Acceleration is too small (maxAcceleration > 0)");
		return steering;
	}
	
	// check if there is a valid vector available in case there is no target actor
	if (moveParam.pTarget == nullptr)
	{
		if (moveParam.targetLoction.Size() != 0)
			bUseTargetVector = true;
		else
		{
			print(FColor::Red, "missing target");
			return steering;
		}
	}

	// get the direction to the target
	if (!bUseTargetVector)
	{
		if (bFlee)
			steering.linear = moveParam.pCharacter->GetActorLocation() - moveParam.pTarget->GetActorLocation();	// Flee
		else
			steering.linear = moveParam.pTarget->GetActorLocation() - moveParam.pCharacter->GetActorLocation();	// seek
	}
	else
	{
		if (bFlee)
			steering.linear = moveParam.pCharacter->GetActorLocation() - moveParam.targetLoction;	// Flee
		else
			steering.linear = moveParam.targetLoction - moveParam.pCharacter->GetActorLocation();	// seek
	}

	// give full acceleration along this direction
	steering.linear.Normalize();
	steering.linear *= moveParam.maxAcceleration;

	// turning is not needed
	steering.angular = 0;

	// return steering
	return steering;
}

// CMoveBehavior_Arrive ----------------------------------------------------------------------------------------------------------------
SteeringOutput UMoveBehavior_Arrive::getSteering(const MoveBehaviorParameter& moveParam)
{
	SteeringOutput steering;	// create the structure to hold our output
	bool bUseTargetVector = false;

	// check needed params
	if (moveParam.pCharacter == nullptr)
	{
		print(FColor::Red, "missing charater");
		return steering;
	}
	if (moveParam.maxSpeed <= 0)
	{
		print(FColor::Red, "max Speed is too small (zero or less)");
		return steering;
	}
	if (moveParam.targetRadius < 0)
		return steering;
	if (moveParam.slowRadius < moveParam.targetRadius)
		return steering;
	if (moveParam.timeToTarget < 0)
		return steering;
	if (moveParam.maxAcceleration < 0)
		return steering;

	// check if there is a valid vector available in case there is no target actor
	if (moveParam.pTarget == nullptr)
	{
		if (moveParam.targetLoction.Size() != 0)
			bUseTargetVector = true;
		else
		{
			print(FColor::Red, "missing target");
			return steering;
		}
	}

	// get the direction to the target
	FVector direction;
	if (!bUseTargetVector)
		direction = moveParam.pTarget->GetActorLocation() - moveParam.pCharacter->GetActorLocation();
	else
		direction = moveParam.targetLoction - moveParam.pCharacter->GetActorLocation();

	//print(FColor::Blue, "direction: " + FString::SanitizeFloat(direction.X) + "|" + FString::SanitizeFloat(direction.Y) + "|" + FString::SanitizeFloat(direction.Z));

	float magnitude = direction.Size();
	//print(FColor::Blue, "magnitude: " + FString::SanitizeFloat(magnitude));

	// check if we allready reached the destination
	if (magnitude < moveParam.targetRadius)
	{
		steering.bDestinationReached = true;
		return steering;
	}

	// If we are outside the slowRadius, then go max speed
	// otherwise calculate a scaled speed
	float targetSpeed = 0;
	if (magnitude > moveParam.slowRadius)
		targetSpeed = moveParam.maxSpeed;
	else
		targetSpeed = moveParam.maxSpeed * magnitude / moveParam.slowRadius;

	//print(FColor::Blue, "targetSpeed: " + FString::SanitizeFloat(targetSpeed));

	// target velocity combines speed and direction
	FVector targetVelocity = direction;
	targetVelocity.Normalize();
	targetVelocity *= targetSpeed;

	//print(FColor::Yellow, "targetVelocity: " + FString::SanitizeFloat(targetVelocity.X) + "|" + FString::SanitizeFloat(targetVelocity.Y) + "|" + FString::SanitizeFloat(targetVelocity.Z));

	// acceleration tries to get to the target velocity
	steering.linear = targetVelocity - moveParam.pCharacter->GetVelocity();
	steering.linear /= moveParam.timeToTarget;

	// Check if the acceleration is too fast (will be done by AddMovementInput)
	/*if (steering.linear.Size() > moveParam.maxAcceleration)
	{
		steering.linear.Normalize();
		steering.linear *= moveParam.maxAcceleration;
	}*/

	//print(FColor::Red, "steering: " + FString::SanitizeFloat(steering.linear.X) + "|" + FString::SanitizeFloat(steering.linear.Y) + "|" + FString::SanitizeFloat(steering.linear.Z));


	// turning is not needed
	steering.angular = 0;

	// return steering
	return steering;
}

// UMoveBehavior_Align ----------------------------------------------------------------------------------------------------------------
SteeringOutput UMoveBehavior_Align::getSteering(const MoveBehaviorParameter& moveParam)
{
	return getSteeringCombined(moveParam, false);
}

SteeringOutput UMoveBehavior_Align::getSteeringReversed(const MoveBehaviorParameter& moveParam)
{
	return getSteeringCombined(moveParam, true);
}

SteeringOutput UMoveBehavior_Align::getSteeringCombined(const MoveBehaviorParameter& moveParam, bool bFlee)
{
	SteeringOutput steering;	// create the structure to hold our output
	bool bUseTargetVector = false;

	// check needed params
	if (moveParam.pCharacter == nullptr)
	{
		print(FColor::Red, "missing charater");
		return steering;
	}
	if (moveParam.maxRotation <= 0)
	{
		print(FColor::Red, "max rotation is too small (zero or less)");
		return steering;
	}
	if (moveParam.targetRadius < 0)
		return steering;
	if (moveParam.slowRadius < moveParam.targetRadius)
		return steering;
	if (moveParam.timeToTarget < 0)
		return steering;

	// check if there is a valid vector available in case there is no target actor
	if (moveParam.pTarget == nullptr)
	{
		if (moveParam.targetLoction.Size() != 0)
			bUseTargetVector = true;
		else
		{
			print(FColor::Red, "missing target");
			return steering;
		}
	}

	// get the naive direction to the target
	float rotation = moveParam.pTarget->GetViewRotation().Yaw - moveParam.pCharacter->GetViewRotation().Yaw;
	if (rotation > 180)
		rotation -= 360;
	//print(FColor::Cyan, GetName() + " rotation: " + FString::SanitizeFloat(rotation) + " || " + FString::SanitizeFloat(moveParam.pTarget->GetViewRotation().Yaw) + " || " + FString::SanitizeFloat(moveParam.pCharacter->GetViewRotation().Yaw));
	//print(FColor::Blue, GetName() + " rotation: " + FString::SanitizeFloat(rotation));

	// map the result to the (-pi, pi) interval
	//rotation = FMath::GetMappedRangeValueClamped({ -180.0f, 180.0f }, { -PI, PI }, rotation);
	//rotation = FMath::GetMappedRangeValueClamped({ 0.0f, 360.0f }, { -PI, PI }, rotation);
	//rotation = FMath::GetMappedRangeValueClamped({ 0.0f, 360.0f }, { -1, 1 }, rotation);
	//rotation = FMath::GetMappedRangeValueClamped({ 0.0f, 360.0f }, { -180.0f, 180.0f }, rotation);
	//print(FColor::Blue, GetName() + " rotationClamped: " + FString::SanitizeFloat(rotation));
	float rotationSize = FMath::Abs(rotation);
	//print(FColor::Green, GetName() + " rotationSize: " + FString::SanitizeFloat(rotationSize));

	// check if we are there, return no steering
	if (rotationSize < moveParam.targetRadius)
	{
		steering.bDestinationReached = true;
		return steering;
	}

	// if we are outside the slowRadius, then use max rotation
	float targetRotation;
	if (rotationSize > moveParam.slowRadius)
		targetRotation = moveParam.maxRotation;
	// otherwise calculate a scaled rotation
	else
		targetRotation = moveParam.maxRotation * rotationSize / moveParam.slowRadius;
	//print(FColor::Yellow, GetName() + " targetRotation(Raw): " + FString::SanitizeFloat(targetRotation));

	// the final target rotation combines speed (already in the variable) and direction
	targetRotation *= rotation / rotationSize;
	//print(FColor::Yellow, GetName() + " targetRotation(final): " + FString::SanitizeFloat(targetRotation));

	// Acceleration tries to get to the target rotation
	steering.angular = targetRotation - moveParam.pCharacter->GetViewRotation().Yaw;
	steering.angular /= moveParam.timeToTarget;

	// check if the acceleration is too fast
	float angularAcceleration = FMath::Abs(steering.angular);
	if (angularAcceleration > moveParam.maxAngularAcceleration)
	{
		steering.angular /= angularAcceleration;
		steering.angular *= moveParam.maxAngularAcceleration;
	}
	//print(FColor::Red, GetName() + " steering.angular: " + FString::SanitizeFloat(steering.angular));

	// skip linear steering
	steering.linear = (FVector)(0, 0, 0);
	//steering.angular = 0;

	// return steering
	return steering;
}

// UMoveBehavior_VelocityMatch ----------------------------------------------------------------------------------------------------------------
SteeringOutput UMoveBehavior_VelocityMatch::getSteering(const MoveBehaviorParameter& moveParam)
{
	SteeringOutput steering;	// create the structure to hold our output

	// check needed params
	if (moveParam.pCharacter == nullptr)
	{
		print(FColor::Red, "missing charater");
		return steering;
	}
	if (moveParam.maxAcceleration <= 0)
	{
		print(FColor::Red, "max Acceleration is too small (maxAcceleration > 0)");
		return steering;
	}

	// check if there is a valid vector available in case there is no target actor
	if (moveParam.pTarget == nullptr)
	{
		print(FColor::Red, "missing target");
		return steering;
	}

	// Acceleration tries to get to the target velocity
	steering.linear = moveParam.pTarget->GetVelocity() - moveParam.pCharacter->GetVelocity();
	steering.linear /= moveParam.timeToTarget;

	// Check if the acceleration is too fast (will be done by AddMovementInput)
	/*if (steering.linear.Size() > moveParam.maxAcceleration)
	{
		steering.linear.Normalize();
		steering.linear *= moveParam.maxAcceleration;
	}*/

	// turning is not needed
	steering.angular = 0;

	// return steering
	return steering;
}

// UMoveBehavior_PursueEvade ----------------------------------------------------------------------------------------------------------------
SteeringOutput UMoveBehavior_PursueEvade::getSteering(const MoveBehaviorParameter& moveParam)
{
	return getSteeringCombined(moveParam, false);
}

SteeringOutput UMoveBehavior_PursueEvade::getSteeringReversed(const MoveBehaviorParameter& moveParam)
{
	return getSteeringCombined(moveParam, true);
}

SteeringOutput UMoveBehavior_PursueEvade::getSteeringCombined(const MoveBehaviorParameter& moveParam, bool bFlee)
{
	SteeringOutput steering;	// create the structure to hold our output

	// check needed params
	if (moveParam.pCharacter == nullptr)
	{
		print(FColor::Red, "missing charater");
		return steering;
	}
	if (moveParam.maxPrediction <= 0)
	{
		print(FColor::Red, "max Prediction is too small (maxPrediction > 0)");
		return steering;
	}

	// check if there is a valid vector available in case there is no target actor
	if (moveParam.pTarget == nullptr)
	{
		print(FColor::Red, "missing target");
		return steering;
	}

	// calculate the distance to target
	float distance = (moveParam.pTarget->GetActorLocation() - moveParam.pCharacter->GetActorLocation()).Size();

	// calculate AI's current speed
	float speed = moveParam.pCharacter->GetVelocity().Size();

	// check if speed is too small to give a reasonable prediction time
	// otherwise calculate the prediction time
	float prediction = 0;
	if (speed <= distance / moveParam.maxPrediction)
		prediction = moveParam.maxPrediction;
	else
		prediction = distance / speed;

	// put the target together
	MoveBehaviorParameter moveParamForSeek;
	moveParamForSeek = moveParam;
	moveParamForSeek.pTarget = NULL;
	moveParamForSeek.targetLoction = moveParam.pTarget->GetActorLocation() + (moveParam.pTarget->GetVelocity() * prediction);

	// return steering
	return UMoveBehavior_SeekFlee::getSteeringCombined(moveParamForSeek, bFlee);
}

// UBTTask_MoveToWithBehavior ----------------------------------------------------------------------------------------------------------------
UBTTask_MoveToWithBehavior::UBTTask_MoveToWithBehavior()
{
	// enable tick
	bNotifyTick = 1;
}

EBTNodeResult::Type UBTTask_MoveToWithBehavior::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	//print(FColor::Magenta, OwnerComp.GetAIOwner()->GetName() + "ExecuteTask");
	EBTNodeResult::Type NodeResult = EBTNodeResult::InProgress;	// become a latent task

	if (!TMovementBehaviorSelector)
	{
		print(FColor::Magenta, "No Movement behavior was chosen");
		return EBTNodeResult::Failed;
	}

	// create behavior
	if (pMovementBehavior == nullptr)
	{
		pMovementBehavior = NewObject<UMoveBehavior_Base>(this, TMovementBehaviorSelector);

		if (pMovementBehavior == nullptr)
		{
			print(FColor::Magenta, "Still Missing UMoveBehavior");
			return EBTNodeResult::Failed;
		}
	}

	NodeResult = executeMovement(OwnerComp);	// call to move the character
	
	return NodeResult;
}

void UBTTask_MoveToWithBehavior::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickTask(OwnerComp, NodeMemory, DeltaSeconds);	// call parent tick

	EBTNodeResult::Type NodeResult = executeMovement(OwnerComp);	// call to move the character

	// Finish Task
	if (NodeResult == EBTNodeResult::Succeeded)
		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
	else if (NodeResult == EBTNodeResult::Failed)
		FinishLatentAbort(OwnerComp);
}

EBTNodeResult::Type UBTTask_MoveToWithBehavior::executeMovement(UBehaviorTreeComponent& OwnerComp)
{
	EBTNodeResult::Type NodeResult = EBTNodeResult::InProgress;

	updateMoveParam(OwnerComp);	// update the parameter

	if (bReversed)
		steering = pMovementBehavior->getSteeringReversed(moveParam);	// get steeringReversed
	else
		steering = pMovementBehavior->getSteering(moveParam);	// get steering

	// if we reached the destination, we are done
	if (steering.bDestinationReached)
	{
		NodeResult = EBTNodeResult::Succeeded;
		return NodeResult;
	}

	// apply steering
	Cast<AACTAIController>(OwnerComp.GetAIOwner())->moveCharacter(steering);

	return NodeResult;
}

void UBTTask_MoveToWithBehavior::updateMoveParam(UBehaviorTreeComponent& OwnerComp)
{
	// update character reference
	moveParam.pCharacter = (AACTCharacter*)OwnerComp.GetAIOwner()->GetPawn();

	// update target reference
	if (BlackboardKey.SelectedKeyType == UBlackboardKeyType_Object::StaticClass())
	{
		UObject* KeyValue = OwnerComp.GetBlackboardComponent()->GetValue<UBlackboardKeyType_Object>(BlackboardKey.GetSelectedKeyID());
		moveParam.pTarget = Cast<AACTCharacter>(KeyValue);
	}
	else if (BlackboardKey.SelectedKeyType == UBlackboardKeyType_Vector::StaticClass())
	{
		moveParam.targetLoction = OwnerComp.GetBlackboardComponent()->GetValue<UBlackboardKeyType_Vector>(BlackboardKey.GetSelectedKeyID());
	}

	// update max acceleration & speed
	moveParam.maxAcceleration = moveParam.pCharacter->GetCharacterMovement()->MaxAcceleration / 100;
	moveParam.maxAngularAcceleration = moveParam.maxAcceleration;
	moveParam.maxSpeed = moveParam.pCharacter->GetCharacterMovement()->GetMaxSpeed();
	moveParam.maxRotation = moveParam.maxSpeed;
	moveParam.maxPrediction = maxPrediction;

	// update radia params
	moveParam.targetRadius = targetRadius; // radius at which the movement is considered as succesful
	moveParam.slowRadius = slowRadius;	// radius for beginning to slow down
	moveParam.timeToTarget = timeToTarget;	// timer over which to achieve target speed
}