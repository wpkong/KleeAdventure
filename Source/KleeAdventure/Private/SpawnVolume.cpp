// Fill out your copyright notice in the Description page of Project Settings.


#include "SpawnVolume.h"
#include "Components/BoxComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/GameplayStatics.h"
#include <random>

// Sets default values
ASpawnVolume::ASpawnVolume()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	this->SpawningBox = CreateDefaultSubobject<UBoxComponent>(TEXT("SpawningBox"));
}

FVector ASpawnVolume::GetSpawnPoint() const
{
	const auto Extent = SpawningBox->GetScaledBoxExtent();
	const auto Origin = SpawningBox->GetComponentLocation();
	const auto Point = UKismetMathLibrary::RandomPointInBoundingBox(Origin, Extent);
	return Point;
}

// Called when the game starts or when spawned
void ASpawnVolume::BeginPlay()
{
	Super::BeginPlay();

	for (int i = 0; i < GenNumber; ++i)
	{
		GenTarget();
	}
}

// Called every frame
void ASpawnVolume::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ASpawnVolume::GenTarget()
{
	const FRotator SpawnRotation(0, 0, 0);
	const FVector SpawnLocation = GetSpawnPoint();
	AActor* Target = GetWorld()->SpawnActor<AActor>(PawnToSpawn, SpawnLocation, SpawnRotation);
}
