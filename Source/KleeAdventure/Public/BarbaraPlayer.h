// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "KleeAdventure/KleeAdventureCharacter.h"
#include "BarbaraPlayer.generated.h"

/**
 * 
 */
UCLASS()
class KLEEADVENTURE_API ABarbaraPlayer : public AKleeAdventureCharacter
{
	GENERATED_BODY()

public:
	/** Location on gun mesh where projectiles should spawn. */
	UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
	USceneComponent* MagicBall_MuzzleLocation;

	/** Projectile Bullet class to spawn */
	UPROPERTY(EditDefaultsOnly, Category=Projectile)
	TSubclassOf<class ABarbaraMagicBall> ProjectileMagicBallClass;

public:
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<class USoundBase*> EnterSounds;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<class USoundBase*> ShootSounds;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<class USoundBase*> BombSounds;

public:
	/** AnimMontage to play each time we fire */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
	UAnimMontage* EmitMagicBallAnimation;

protected:
	// APawn interface
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	// End of APawn interface
	
public:
	ABarbaraPlayer();
	
	virtual void BeginPlay() override;

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerEmitMagicBall();

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerPlayEmitMagicBallAnim();

	UFUNCTION(NetMulticast, Reliable, WithValidation)
	void MulticastPlayEmitMagicBallAnim();
	
	// UFUNCTION(BlueprintCallable)
	virtual void OnFire() override;
	
	// UFUNCTION(BlueprintCallable)
	virtual void OnSkill() override;

	UFUNCTION(BlueprintCallable)
	void EmitMagicBall();
};
