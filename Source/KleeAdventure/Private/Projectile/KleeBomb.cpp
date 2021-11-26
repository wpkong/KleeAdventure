// Fill out your copyright notice in the Description page of Project Settings.


#include "Projectile/KleeBomb.h"

#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"

// Sets default values
AKleeBomb::AKleeBomb()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Use a sphere as a simple collision representation
	CollisionComp = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComp"));
	CollisionComp->InitSphereRadius(5.0f);
	CollisionComp->BodyInstance.SetCollisionProfileName("Projectile");
	CollisionComp->OnComponentHit.AddDynamic(this, &AKleeBomb::OnHit);		// set up a notification for when this component hits something blocking
	// CollisionComp->OnComponentBeginOverlap.AddDynamic(this, &AKleeBomb::OnBeginOverlap);
	
	// Players can't walk on it
	CollisionComp->SetWalkableSlopeOverride(FWalkableSlopeOverride(WalkableSlope_Unwalkable, 0.f));
	CollisionComp->CanCharacterStepUpOn = ECB_No;

	// Set as root component
	RootComponent = CollisionComp;

	// Use a ProjectileMovementComponent to govern this projectile's movement
	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileComp"));
	ProjectileMovement->UpdatedComponent = CollisionComp;
	ProjectileMovement->InitialSpeed = 30.f;
	ProjectileMovement->MaxSpeed = 30.f;
	ProjectileMovement->bRotationFollowsVelocity = true;
	ProjectileMovement->bShouldBounce = true;
	
	InitialLifeSpan = 10.0f;
}

// Called when the game starts or when spawned
void AKleeBomb::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AKleeBomb::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

inline void AKleeBomb::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	FVector NormalImpulse, const FHitResult& Hit)
{
	// Only add impulse and destroy projectile if we hit a physics
	// if ((OtherActor != nullptr) && (OtherActor != this) && (OtherComp != nullptr) && OtherComp->IsSimulatingPhysics())
	// {
	// 	OtherComp->AddImpulseAtLocation(GetVelocity() * 100.0f, GetActorLocation());
	// }

	if ((OtherActor != nullptr) && (OtherActor != this))
	{
		ActivateBomb();
		ApplyBombImpulsion();
		if(LifeHop-- <= 0)
		{
			Destroy();
		}
	}
}
