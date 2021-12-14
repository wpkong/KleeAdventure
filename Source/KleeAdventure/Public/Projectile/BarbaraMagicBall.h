// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Projectile/Projectile.h"
#include "BarbaraMagicBall.generated.h"

class UProjectileMovementComponent;
class USphereComponent;


UENUM(BlueprintType)
enum class EMagicBallType : uint8
{
	Damage UMETA(DisplayName="Damage"),
	Cure UMETA(DisplayName="Cure")
};


/**
 * 
 */
UCLASS()
class KLEEADVENTURE_API ABarbaraMagicBall : public AProjectile
{
	GENERATED_BODY()

public:
	UPROPERTY(VisibleDefaultsOnly, Category=Projectile)
	EMagicBallType MagicBallType = EMagicBallType::Cure;

	/** Sphere collision component */
	UPROPERTY(VisibleDefaultsOnly, Category=Projectile)
	USphereComponent* CollisionComp;

	/** Projectile movement component */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
	UProjectileMovementComponent* ProjectileMovement;

public:
	// Sets default values for this actor's properties
	ABarbaraMagicBall();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION()
	void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse,
	           const FHitResult& Hit);
};
