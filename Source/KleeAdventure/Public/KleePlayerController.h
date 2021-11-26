// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "KleePlayerController.generated.h"

/**
 * 
 */
UCLASS()
class KLEEADVENTURE_API AKleePlayerController : public APlayerController
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<UUserWidget> IntroHUDAsset;
	UUserWidget *IntroHUD;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<UUserWidget> TouchHUDAsset;
	UUserWidget *TouchHUD;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<UUserWidget> ScoreHUDAsset;
	UUserWidget *ScoreHUD;

public:
	virtual void BeginPlay() override;
};
