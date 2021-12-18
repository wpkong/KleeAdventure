// Fill out your copyright notice in the Description page of Project Settings.


#include "KleePlayerController.h"
#include "Blueprint/UserWidget.h"
#include "Kismet/GameplayStatics.h"

void AKleePlayerController::BeginPlay()
{
	Super::BeginPlay();

	if (IntroHUDAsset)
	{
		IntroHUD = CreateWidget<UUserWidget>(this, IntroHUDAsset);
	}
	if (IntroHUD)
	{
		IntroHUD->AddToViewport();
	}

	if (TouchHUDAsset)
	{
		TouchHUD = CreateWidget<UUserWidget>(this, TouchHUDAsset);
	}
	if (TouchHUD)
	{
		TouchHUD->AddToViewport();
	}

	if (ScoreHUDAsset)
	{
		ScoreHUD = CreateWidget<UUserWidget>(this, ScoreHUDAsset);
	}
	if (ScoreHUD)
	{
		ScoreHUD->AddToViewport();
	}

	if (CharacterHealthHUDAsset)
	{
		CharacterHealthHUD = CreateWidget<UUserWidget>(this, CharacterHealthHUDAsset);
	}
	if (CharacterHealthHUD)
	{
		CharacterHealthHUD->AddToViewport();
	}

	if(GameOverHUDAsset)
	{
		GameOverHUD = CreateWidget<UUserWidget>(this, GameOverHUDAsset);
	}
}

void AKleePlayerController::GameOver()
{
	if(GameOverHUD)
	{
		GameOverHUD->AddToViewport();
	}
}
