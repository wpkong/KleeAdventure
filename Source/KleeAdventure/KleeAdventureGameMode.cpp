// Copyright Epic Games, Inc. All Rights Reserved.

#include "KleeAdventureGameMode.h"
#include "KleeAdventureCharacter.h"
#include "UObject/ConstructorHelpers.h"

AKleeAdventureGameMode::AKleeAdventureGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPersonCPP/Blueprints/BP_Klee"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}
