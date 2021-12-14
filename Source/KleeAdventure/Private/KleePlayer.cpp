// Fill out your copyright notice in the Description page of Project Settings.


#include "KleePlayer.h"
#include "Components/CapsuleComponent.h"
#include "KleeAdventure/Public/Projectile/KleeBullet.h"
#include "KleeAdventure/Public/Projectile/KleeBomb.h"


void AKleePlayer::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAction("Fire", IE_Pressed, this, &AKleePlayer::OnFire);
	PlayerInputComponent->BindAction("Skill", IE_Pressed, this, &AKleePlayer::OnSkill);
}

AKleePlayer::AKleePlayer():AKleeAdventureCharacter()
{
	// Create a gun mesh component
	GunMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("GunMesh"));
	GunMesh->SetOnlyOwnerSee(false); // otherwise won't be visible in the multiplayer
	GunMesh->bCastDynamicShadow = false;
	GunMesh->CastShadow = false;
	// FP_Gun->SetupAttachment(Mesh1P, TEXT("GripPoint"));
	GunMesh->SetupAttachment(RootComponent);

	// Creat Projectile's Spawn Location
	Bullet_MuzzleLocation = CreateDefaultSubobject<USceneComponent>(TEXT("Bullet_MuzzleLocation"));
	Bullet_MuzzleLocation->SetupAttachment(GunMesh);

	Bomb_MuzzleLocation = CreateDefaultSubobject<USceneComponent>(TEXT("Bomb_MuzzleLocation"));
	Bomb_MuzzleLocation->SetupAttachment(GetMesh());
}

void AKleePlayer::BeginPlay()
{
	Super::BeginPlay();
	
	GunMesh->AttachToComponent(GetMesh(), FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true),
							   TEXT("LeftHandSocket"));
	RandomPlay(this->EnterSounds);
}

void AKleePlayer::EmitBomb()
{
	// try and fire a projectile
	if (ProjectileBombClass != nullptr)
	{
		AKleeBomb* Projectile;

		UWorld* const World = GetWorld();
		if (World != nullptr)
		{
			FVector GunOffset(40, 0, 50);
			// const FRotator SpawnRotation = GetControlRotation();
			FRotator SpawnRotation = GetCapsuleComponent()->GetComponentRotation();
			SpawnRotation.Pitch += 45;

			// MuzzleOffset is in camera space, so transform it to world space before offsetting from the character location to find the final muzzle position
			const FVector SpawnLocation = ((Bomb_MuzzleLocation != nullptr)
											   ? Bomb_MuzzleLocation->GetComponentLocation()
											   : GetActorLocation()) + SpawnRotation.RotateVector(GunOffset);

			//Set Spawn Collision Handling Override
			FActorSpawnParameters ActorSpawnParams;
			ActorSpawnParams.SpawnCollisionHandlingOverride =
				ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButDontSpawnIfColliding;

			// spawn the projectile at the muzzle
			Projectile = World->SpawnActor<AKleeBomb>(ProjectileBombClass, SpawnLocation, SpawnRotation,
													  ActorSpawnParams);
			if (IsValid(Projectile)) Projectile->SetProjector(this);
			RandomPlay(this->BombSounds);
		}
	}
}


void AKleePlayer::OnFire()
{
	
	if (IsJumping) return;
	if (BlockingMovementCount != 0) return;

	// try and fire a projectile
	if (ProjectileBulletClass != nullptr)
	{
		AKleeBullet* Projectile;

		UWorld* const World = GetWorld();
		if (World != nullptr)
		{
			FVector GunOffset(0, 0, 0);
			// const FRotator SpawnRotation = GetControlRotation();
			const FRotator SpawnRotation = GetActorRotation();
			// MuzzleOffset is in camera space, so transform it to world space before offsetting from the character location to find the final muzzle position
			const FVector SpawnLocation = ((Bullet_MuzzleLocation != nullptr)
											   ? Bullet_MuzzleLocation->GetComponentLocation()
											   : GetActorLocation()) + SpawnRotation.RotateVector(GunOffset);

			//Set Spawn Collision Handling Override
			FActorSpawnParameters ActorSpawnParams;
			ActorSpawnParams.SpawnCollisionHandlingOverride =
				ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButDontSpawnIfColliding;

			// spawn the projectile at the muzzle
			Projectile = World->SpawnActor<AKleeBullet>(ProjectileBulletClass, SpawnLocation, SpawnRotation,
													ActorSpawnParams);

			if (IsValid(Projectile)) Projectile->SetProjector(this);
		}
	}
	if (BulletAnimation != nullptr)
	{
		// Get the animation object for the arms mesh
		UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
		if (AnimInstance != nullptr)
		{
			// GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("Play"));
			AnimInstance->Montage_Play(BulletAnimation, 1.f);
		}
	}
	RandomPlay(this->ShootSounds);
}

void AKleePlayer::OnSkill()
{
	if (BlockingMovementCount != 0) return;
	if (IsJumping) return;

	if (BombAnimation != nullptr)
	{
		// Get the animation object for the arms mesh
		UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
		if (AnimInstance != nullptr)
		{
			BlockingMovementCount++;
			AnimInstance->Montage_Play(BombAnimation, 2.0f);
		}
	}
}
