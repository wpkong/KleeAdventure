// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Projectile.generated.h"

UCLASS()
class KLEEADVENTURE_API AProjectile : public AActor
{
	GENERATED_BODY()

protected:
	class AKleeAdventureCharacter* Projector;
	
public:	
	// Sets default values for this actor's properties
	AProjectile();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	class AKleeAdventureCharacter* GetProjector(){ return Projector;}

	void SetProjector(class AKleeAdventureCharacter* Other){ this->Projector = Other; }

};
