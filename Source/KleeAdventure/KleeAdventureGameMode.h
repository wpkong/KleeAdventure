// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "KleeAdventureGameMode.generated.h"

UCLASS(minimalapi)
class AKleeAdventureGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	AKleeAdventureGameMode();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FString> PlayerStarts;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<TSubclassOf<class APawn>> PlayerPawns;

	UPROPERTY(EditAnywhere, NoClear, BlueprintReadOnly, Category=Classes)
	TSubclassOf<class AActor> EnemyClass;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	bool SpawnedStart = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	bool SpawnedPawn = false;

	UFUNCTION(BlueprintCallable)
	FString SpawnPlayerStart();

	UFUNCTION(BlueprintCallable)
	TSubclassOf<class APawn> SpawnPlayerPawn();

public:
	virtual void BeginPlay() override;

public:
	UFUNCTION(Server, Reliable, BlueprintCallable)
	void GameOver();

	UFUNCTION(Server, Reliable, BlueprintCallable)
	void CalcMaxEnemies();
};



