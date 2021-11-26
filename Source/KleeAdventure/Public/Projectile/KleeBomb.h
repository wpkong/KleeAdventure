// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "KleeAdventure/Public/Projectile/Projectile.h"
#include "KleeBomb.generated.h"

class UProjectileMovementComponent;
class USphereComponent;
UCLASS()
class KLEEADVENTURE_API AKleeBomb : public AProjectile
{
	GENERATED_BODY()

protected:
	/** Sphere collision component */
	UPROPERTY(VisibleDefaultsOnly, Category=Projectile)
	USphereComponent* CollisionComp;

	/** Projectile movement component */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
	UProjectileMovementComponent* ProjectileMovement;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	int LifeHop = 3;
	
public:	
	// Sets default values for this actor's properties
	AKleeBomb();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	
	UFUNCTION()
	void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);
	
	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
	void ActivateBomb();

	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
	void ApplyBombImpulsion();
};
