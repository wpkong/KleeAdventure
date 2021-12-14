// Fill out your copyright notice in the Description page of Project Settings.


#include "BarbaraPlayer.h"
#include "Projectile/BarbaraMagicBall.h"

void ABarbaraPlayer::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	// PlayerInputComponent->BindAction("Fire", IE_Pressed, this, &ABarbaraPlayer::OnFire);
	// PlayerInputComponent->BindAction("Skill", IE_Pressed, this, &ABarbaraPlayer::OnSkill);
}

ABarbaraPlayer::ABarbaraPlayer(): AKleeAdventureCharacter()
{
	// Creat Projectile's Spawn Location
	MagicBall_MuzzleLocation = CreateDefaultSubobject<USceneComponent>(TEXT("Bullet_MuzzleLocation"));
	MagicBall_MuzzleLocation->SetupAttachment(GetMesh());
}

void ABarbaraPlayer::BeginPlay()
{
	Super::BeginPlay();

	RandomPlay(this->EnterSounds);
}

void ABarbaraPlayer::OnFire()
{
	if (Super::IsLockingMovement()) return;
	if (IsJumping) return;
	if (EmitMagicBallAnimation != nullptr)
	{
		// Get the animation object for the arms mesh
		// GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("Fire0"));
		UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("Fire1"));
		if (AnimInstance != nullptr)
		{
			GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("MagicBallAnimation"));
			AnimInstance->Montage_Play(EmitMagicBallAnimation, 1.f);
			LockMovement();
		}
	}
	RandomPlay(this->ShootSounds);
}

void ABarbaraPlayer::OnSkill()
{
	Super::OnSkill();
}

void ABarbaraPlayer::EmitMagicBall()
{
	// try and fire a projectile
	if (ProjectileMagicBallClass != nullptr)
	{
		ABarbaraMagicBall* Projectile;

		UWorld* const World = GetWorld();
		if (World != nullptr)
		{
			FVector GunOffset(0, 0, 0);
			// const FRotator SpawnRotation = GetControlRotation();
			const FRotator SpawnRotation = GetActorRotation();
			// MuzzleOffset is in camera space, so transform it to world space before offsetting from the character location to find the final muzzle position
			const FVector SpawnLocation = ((MagicBall_MuzzleLocation != nullptr)
											   ? MagicBall_MuzzleLocation->GetComponentLocation()
											   : GetActorLocation()) + SpawnRotation.RotateVector(GunOffset);

			//Set Spawn Collision Handling Override
			FActorSpawnParameters ActorSpawnParams;
			ActorSpawnParams.SpawnCollisionHandlingOverride =
				ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButDontSpawnIfColliding;

			// spawn the projectile at the muzzle
			Projectile = World->SpawnActor<ABarbaraMagicBall>(ProjectileMagicBallClass, SpawnLocation, SpawnRotation,
													ActorSpawnParams);

			if (IsValid(Projectile)) Projectile->SetProjector(this);
		}
	}
}
