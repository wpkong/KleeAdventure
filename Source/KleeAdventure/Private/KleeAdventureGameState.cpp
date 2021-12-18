// Fill out your copyright notice in the Description page of Project Settings.


#include "KleeAdventureGameState.h"
#include "KleeAdventure/KleeAdventureGameMode.h"
#include "KleeAdventure/Public/KleePlayerController.h"
#include "Net/UnrealNetwork.h"
#include "Engine/World.h"


void AKleeAdventureGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AKleeAdventureGameState, Score);
	DOREPLIFETIME(AKleeAdventureGameState, MaxScore);
}

void AKleeAdventureGameState::MulticastOnMissionComplete_Implementation()
{
	for (auto It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
	{
		AKleePlayerController *PC = Cast<AKleePlayerController>(It->Get());
		if(PC && PC->IsLocalController())
		{
			PC->GameOver();
			APawn *Pawn = PC->GetPawn();
			if(Pawn)
			{
				Pawn->DisableInput(nullptr);
			}
		}
	}	
}

void AKleeAdventureGameState::AddScore()
{
	Score += 1;
	if(Score >= MaxScore)
	{
		ServerEndGame();
	}
}

void AKleeAdventureGameState::SetMaxScore_Implementation(int Value)
{
	MaxScore = Value;
}

void AKleeAdventureGameState::ServerEndGame_Implementation()
{
	if(GetLocalRole() == ROLE_Authority)
	{
		AKleeAdventureGameMode * GM = Cast<AKleeAdventureGameMode>(GetWorld()->GetAuthGameMode());
		GM->GameOver();
	}
}
