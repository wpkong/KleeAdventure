// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "KleeAdventure/KleeAdventureCharacter.h"
#include "KleePlayer.generated.h"

/**
 * 
 */
UCLASS()
class KLEEADVENTURE_API AKleePlayer : public AKleeAdventureCharacter
{
	GENERATED_BODY()
public:
	UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
	USkeletalMeshComponent* GunMesh;

	/** Location on gun mesh where projectiles should spawn. */
	UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
	USceneComponent* Bullet_MuzzleLocation;

	UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
	USceneComponent* Bomb_MuzzleLocation;

	/** Projectile KleeBomb class to spawn */
	UPROPERTY(EditDefaultsOnly, Category=Projectile)
	TSubclassOf<class AKleeBomb> ProjectileBombClass;

	/** Projectile Bullet class to spawn */
	UPROPERTY(EditDefaultsOnly, Category=Projectile)
	TSubclassOf<class AKleeBullet> ProjectileBulletClass;

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<class USoundBase*> EnterSounds;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<class USoundBase*> ShootSounds;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<class USoundBase*> BombSounds;

protected:
	/** AnimMontage to play each time we fire */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
	UAnimMontage* BulletAnimation;

	/** AnimMontage to play each time we fire */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
	UAnimMontage* BombAnimation;

// public:
// 	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
// 	bool IsProjectingBomb = false;
//

protected:
	// APawn interface
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	// End of APawn interface
	
public:
	AKleePlayer();

	virtual void BeginPlay() override;
	
	// UFUNCTION(BlueprintCallable)
	virtual void OnFire() override;
	
	// UFUNCTION(BlueprintCallable)
	virtual void OnSkill() override;

	UFUNCTION(BlueprintCallable)
	void EmitBomb();
};
