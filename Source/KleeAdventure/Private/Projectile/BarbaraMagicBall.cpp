// Fill out your copyright notice in the Description page of Project Settings.


#include "Projectile/BarbaraMagicBall.h"
#include "Components/SphereComponent.h"
#include "KleeAdventure/Public/KleeAdventureEnemy.h"
#include "KleeAdventure/Public/KleePlayer.h"
#include "GameFramework/ProjectileMovementComponent.h"

ABarbaraMagicBall::ABarbaraMagicBall(): AProjectile()
{
	PrimaryActorTick.bCanEverTick = true;

	// Use a sphere as a simple collision representation
	CollisionComp = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComp"));
	CollisionComp->InitSphereRadius(5.0f);
	CollisionComp->BodyInstance.SetCollisionProfileName("Projectile");
	// CollisionComp->OnComponentHit.AddDynamic(this, &ABarbaraMagicBall::OnHit);		// set up a notification for when this component hits something blocking
	
	// Players can't walk on it
	CollisionComp->SetWalkableSlopeOverride(FWalkableSlopeOverride(WalkableSlope_Unwalkable, 0.f));
	CollisionComp->CanCharacterStepUpOn = ECB_No;

	// Set as root component
	RootComponent = CollisionComp;

	// Use a ProjectileMovementComponent to govern this projectile's movement
	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileComp"));
	ProjectileMovement->UpdatedComponent = CollisionComp;
	ProjectileMovement->InitialSpeed = 3000.f;
	ProjectileMovement->MaxSpeed = 3000.f;
	ProjectileMovement->bRotationFollowsVelocity = true;
	ProjectileMovement->bShouldBounce = true;

	// Die after 3 seconds by default
	InitialLifeSpan = 3.0f;
}

void ABarbaraMagicBall::BeginPlay()
{
	Super::BeginPlay();

	CollisionComp->OnComponentBeginOverlap.AddDynamic(this, &ABarbaraMagicBall::OnSphereOverlapBegin);
	// CollisionComp->OnComponentEndOverlap.AddDynamic(this, &ABarbaraMagicBall::OnSphereOverlapEnd);
}

void ABarbaraMagicBall::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ABarbaraMagicBall::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	FVector NormalImpulse, const FHitResult& Hit)
{
	
}

void ABarbaraMagicBall::OnSphereOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if(OtherActor == nullptr || OtherActor == this || OtherActor == Projector) return;

	if(!Cast<AKleeAdventureCharacter>(OtherActor))
	{
		if(GetLocalRole() == ROLE_Authority)
		{
			Destroy();
		}
		return;
	}
	
	if(auto OtherEnemy = Cast<AKleeAdventureEnemy>(OtherActor))
	{
		if(GetLocalRole() == ROLE_Authority)
		{
			OtherEnemy->Damage(20);
			OtherComp->AddImpulseAtLocation(GetVelocity() * 10.0f, GetActorLocation());
			Destroy();
		}
	}else if(auto OtherPlayer = Cast<AKleePlayer>(OtherActor))
	{
		// DO nothing
		// GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, "Hit Character");
		// GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, OtherActor->GetName());
		// GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, Projector->GetName());
		if(GetLocalRole() == ROLE_Authority)
		{
			OtherPlayer->Cure(10);
			Destroy();
		}
	}else if(auto Self = Cast<AKleeAdventureCharacter>(OtherActor))
	{
		
	}else
	{
		if(GetLocalRole() == ROLE_Authority)
		{
			Destroy();
		}
	}
	
}

void ABarbaraMagicBall::OnSphereOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
}
