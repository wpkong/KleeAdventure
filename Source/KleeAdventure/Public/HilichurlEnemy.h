// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "KleeAdventure/Public/KleeAdventureEnemy.h"
// #include "KleeAdventure/KleeAdventureCharacter.h"
#include "HilichurlEnemy.generated.h"

/**
 * 
 */
UCLASS()
class KLEEADVENTURE_API AHilichurlEnemy : public AKleeAdventureEnemy
{
	GENERATED_BODY()
public:
	/** AnimMontage to play each time we fire */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
	UAnimMontage* AttackAnimation;

protected:
	// APawn interface
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	// End of APawn interface
	
public:
	AHilichurlEnemy();

	virtual void BeginPlay() override;
	
	// UFUNCTION(BlueprintCallable)
	virtual void OnFire() override;
	
	// UFUNCTION(BlueprintCallable)
	virtual void OnSkill() override;

	UFUNCTION(BlueprintCallable)
	void OnClawHit();
};
