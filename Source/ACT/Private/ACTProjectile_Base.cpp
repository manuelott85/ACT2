// Fill out your copyright notice in the Description page of Project Settings.


#include "ACTProjectile_Base.h"
#include "Runtime/Engine/Classes/GameFramework/ProjectileMovementComponent.h"
#include "Runtime/Engine/Classes/Components/CapsuleComponent.h"
#include "Runtime/Engine/Classes/Components/StaticMeshComponent.h"
#include "Runtime/Engine/Public/DrawDebugHelpers.h"

#include "EngineGlobals.h"
#include "Runtime/Engine/Classes/Engine/Engine.h"

#define print(color, text) if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 10, color, text)
// print(FColor::Magenta, FString::FromInt(temp));
// print(FColor::Blue, FString::SanitizeFloat(temp));

// Sets default values
AACTProjectile_Base::AACTProjectile_Base()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	bReplicates = true;
	bReplicateMovement = true;
	bCanBeDamaged = false;
	InitialLifeSpan = 10;

	// Capsule Root
	CapsuleComponent = CreateDefaultSubobject<UCapsuleComponent>(TEXT("CapsuleComponent"));
	CapsuleComponent->SetCapsuleRadius(0.25, true);
	CapsuleComponent->SetCapsuleHalfHeight(3, true);
	CapsuleComponent->CanCharacterStepUpOn = ECanBeCharacterBase::ECB_No;
	CapsuleComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	CapsuleComponent->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	CapsuleComponent->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Overlap);
	//CapsuleComponent->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Block);
	//CapsuleComponent->SetCollisionResponseToChannel(ECC_WorldDynamic, ECR_Block);
	CapsuleComponent->OnComponentBeginOverlap.AddDynamic(this, &AACTProjectile_Base::BeginOverlap);
	CapsuleComponent->SetSimulatePhysics(true);

	RootComponent = CapsuleComponent;

	//// Projectile Mesh
	//ProjectileAsset = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ProjectileAsset"));
	//ProjectileAsset->SetupAttachment(GetRootComponent());
	//ProjectileAsset->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	//ProjectileAsset->SetGenerateOverlapEvents(false);

	// Movement Component
	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovement"));
	ProjectileMovement->InitialSpeed = 9000;
	ProjectileMovement->MaxSpeed = 9000;
	ProjectileMovement->bRotationFollowsVelocity = true;
	ProjectileMovement->bForceSubStepping = true;
	ProjectileMovement->ProjectileGravityScale = 0.1;
	//ProjectileMovement->Velocity = FVector(0, 0, -1);
	ProjectileMovement->MaxSimulationIterations = 8;
	ProjectileMovement->MaxSimulationTimeStep = 0.005;
}

// Called when the game starts or when spawned
void AACTProjectile_Base::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void AACTProjectile_Base::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	DrawDebugPoint(GetWorld(), GetActorLocation(), 5, FColor::Red, false, 3);

}

void AACTProjectile_Base::BeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult &SweepResult)
{
	print(FColor::Blue, "I overlap!");
	
	OtherActor->TakeDamage(damage, damageType, pInstigator, pFromWeapon);

}