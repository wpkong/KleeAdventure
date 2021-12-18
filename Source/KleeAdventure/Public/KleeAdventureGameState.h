// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "KleeAdventureGameState.generated.h"

/**
 * 
 */
UCLASS()
class KLEEADVENTURE_API AKleeAdventureGameState : public AGameStateBase
{
	GENERATED_BODY()
public:
	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite)
	int Score = 0;
	
	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite)
	int MaxScore = 2;

public:
	UFUNCTION(BlueprintCallable)
	void AddScore();

	UFUNCTION(Server, Reliable, BlueprintCallable)
	void ServerEndGame();
	
	UFUNCTION(NetMulticast, Reliable)
	void MulticastOnMissionComplete();

	UFUNCTION(NetMulticast, Reliable)
	void SetMaxScore(int Value);
};
