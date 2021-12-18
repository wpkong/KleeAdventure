// Copyright Epic Games, Inc. All Rights Reserved.

#include "KleeAdventureGameMode.h"
#include "KleeAdventureCharacter.h"
#include "KleeAdventure/Public/KleeAdventureGameState.h"
#include "UObject/ConstructorHelpers.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"


AKleeAdventureGameMode::AKleeAdventureGameMode()
{
	// set default pawn class to our Blueprinted character
	// static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPersonCPP/Blueprints/BP_Klee"));
	// if (PlayerPawnBPClass.Class != NULL)
	// {
	// 	DefaultPawnClass = PlayerPawnBPClass.Class;
	// }

	GameStateClass = AKleeAdventureGameState::StaticClass();
}

FString AKleeAdventureGameMode::SpawnPlayerStart()
{
	if(!SpawnedStart)
	{
		SpawnedStart = true;
		return PlayerStarts[0];
	}else
	{
		return PlayerStarts[1];
	}
}

void AKleeAdventureGameMode::BeginPlay()
{
	Super::BeginPlay();

	CalcMaxEnemies();
}

void AKleeAdventureGameMode::CalcMaxEnemies_Implementation()
{
	if(EnemyClass)
	{
		TArray<AActor*> FoundActors;
		auto Wold = GetWorld();
		if(Wold == nullptr) return;
		UGameplayStatics::GetAllActorsOfClass(Wold, EnemyClass, FoundActors);
		AKleeAdventureGameState *GS = GetGameState<AKleeAdventureGameState>();
		if(GS)
		{
			// GS->SetMaxScore(FoundActors.Num());
			GS->SetMaxScore(FoundActors.Num());
		}
	}
}

void AKleeAdventureGameMode::GameOver_Implementation()
{
	AKleeAdventureGameState *GS = GetGameState<AKleeAdventureGameState>();
	if(GS)
	{
		GS->MulticastOnMissionComplete();
	}
}

TSubclassOf<class APawn> AKleeAdventureGameMode::SpawnPlayerPawn()
{
	if(!SpawnedPawn)
	{
		SpawnedPawn = true;
		return PlayerPawns[0];
	}else
	{
		return PlayerPawns[1];
	}
}
