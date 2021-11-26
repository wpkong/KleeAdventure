// Copyright Epic Games, Inc. All Rights Reserved.

#include "KleeAdventureCharacter.h"

#include <algorithm>
#include <random>
#include "HeadMountedDisplayFunctionLibrary.h"
#include "KleeAdventure/Public/Projectile/Bullet.h"
#include "KleeAdventure/Public/Projectile/KleeBomb.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "Sound/SoundCue.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/SpringArmComponent.h"
#include "Kismet/GameplayStatics.h"

//////////////////////////////////////////////////////////////////////////
// AKleeAdventureCharacter

AKleeAdventureCharacter::AKleeAdventureCharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	// set our turn rates for input
	BaseTurnRate = 45.f;
	BaseLookUpRate = 45.f;

	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 540.0f, 0.0f); // ...at this rotation rate
	GetCharacterMovement()->JumpZVelocity = 600.f;
	GetCharacterMovement()->AirControl = 0.2f;

	
	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 300.0f; // The camera follows at this distance behind the character	
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	// Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named MyCharacter (to avoid direct content references in C++)

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

//////////////////////////////////////////////////////////////////////////
// Input

void AKleeAdventureCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// Set up gameplay key bindings
	check(PlayerInputComponent);
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &AKleeAdventureCharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &AKleeAdventureCharacter::StopJumping);

	PlayerInputComponent->BindAction("Sprint", IE_Pressed, this, &AKleeAdventureCharacter::Sprint);
	PlayerInputComponent->BindAction("Sprint", IE_Released, this, &AKleeAdventureCharacter::StopSprinting);

	PlayerInputComponent->BindAction("Fire", IE_Pressed, this, &AKleeAdventureCharacter::OnFire);
	PlayerInputComponent->BindAction("Skill", IE_Pressed, this, &AKleeAdventureCharacter::PlayProjectingBomb);

	PlayerInputComponent->BindAxis("MoveForward", this, &AKleeAdventureCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AKleeAdventureCharacter::MoveRight);

	// We have 2 versions of the rotation bindings to handle different kinds of devices differently
	// "turn" handles devices that provide an absolute delta, such as a mouse.
	// "turnrate" is for devices that we choose to treat as a rate of change, such as an analog joystick
	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("TurnRate", this, &AKleeAdventureCharacter::TurnAtRate);
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("LookUpRate", this, &AKleeAdventureCharacter::LookUpAtRate);
	PlayerInputComponent->BindAxis("Zoom", this, &AKleeAdventureCharacter::Zoom);

	// handle touch devices
	PlayerInputComponent->BindTouch(IE_Pressed, this, &AKleeAdventureCharacter::TouchStarted);
	PlayerInputComponent->BindTouch(IE_Released, this, &AKleeAdventureCharacter::TouchStopped);

	// VR headset functionality
	PlayerInputComponent->BindAction("ResetVR", IE_Pressed, this, &AKleeAdventureCharacter::OnResetVR);
}

void AKleeAdventureCharacter::RandomPlay(TArray<USoundBase*>& Sounds)
{
	if(Sounds.Num() == 0) return;
	
	auto RandomEngine = std::mt19937(std::random_device()());
	auto RandomDistribution = std::uniform_int_distribution<int>(0, Sounds.Num() - 1);
	int RandomValue = RandomDistribution(RandomEngine);
	USoundBase *Sound = Sounds[RandomValue];
	
	if(Sound != nullptr)
	{
		UGameplayStatics::PlaySoundAtLocation(this, Sound, GetActorLocation());
	}
}


void AKleeAdventureCharacter::BeginPlay()
{
	Super::BeginPlay();
	
	GunMesh->AttachToComponent(GetMesh(), FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true),
							  TEXT("LeftHandSocket"));
	RandomPlay(this->EnterSounds);
}

void AKleeAdventureCharacter::OnResetVR()
{
	// If KleeAdventure is added to a project via 'Add Feature' in the Unreal Editor the dependency on HeadMountedDisplay in KleeAdventure.Build.cs is not automatically propagated
	// and a linker error will result.
	// You will need to either:
	//		Add "HeadMountedDisplay" to [YourProject].Build.cs PublicDependencyModuleNames in order to build successfully (appropriate if supporting VR).
	// or:
	//		Comment or delete the call to ResetOrientationAndPosition below (appropriate if not supporting VR)
	UHeadMountedDisplayFunctionLibrary::ResetOrientationAndPosition();
}

void AKleeAdventureCharacter::TouchStarted(ETouchIndex::Type FingerIndex, FVector Location)
{
	Jump();
}

void AKleeAdventureCharacter::TouchStopped(ETouchIndex::Type FingerIndex, FVector Location)
{
	StopJumping();
}

void AKleeAdventureCharacter::Sprint()
{
	if(IsProjectingBomb) return;
	IsSprinting = true;
}

void AKleeAdventureCharacter::StopSprinting()
{
	if(IsProjectingBomb) return;
	IsSprinting = false;
}

void AKleeAdventureCharacter::Jump()
{
	if(IsProjectingBomb) return;
	IsJumping = true;
	Super::Jump();
}

void AKleeAdventureCharacter::StopJumping()
{
	if(IsProjectingBomb) return;
	IsJumping = false;
	Super::StopJumping();
}

void AKleeAdventureCharacter::Zoom(float Value)
{
	if(Value != 0)
	{
		float ArmLength = CameraBoom->TargetArmLength + Value;
		CameraBoom->TargetArmLength = std::min(std::max(ArmLength, CameraRoomMinArmLength), CameraRoomMaxArmLength);
	}
}

void AKleeAdventureCharacter::OnFire()
{
	EmitBullet();
}

void AKleeAdventureCharacter::EmitBullet()
{
	if(IsJumping) return;
	if(IsProjectingBomb) return;
	
	// try and fire a projectile
	if (ProjectileBulletClass != nullptr)
	{
		ABullet * Projectile;
		
		UWorld* const World = GetWorld();
		if (World != nullptr)
		{
				FVector GunOffset(0,0,0);
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
				Projectile = World->SpawnActor<ABullet>(ProjectileBulletClass, SpawnLocation, SpawnRotation, ActorSpawnParams);
			
			if(IsValid(Projectile)) Projectile->SetProjector(this);
		}
	}
	if(BulletAnimation != nullptr)
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

void AKleeAdventureCharacter::PlayProjectingBomb()
{
	if(IsProjectingBomb) return;
	if(IsJumping) return;
	
	if(BombAnimation != nullptr)
	{
		// Get the animation object for the arms mesh
		UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
		if (AnimInstance != nullptr)
		{
			IsProjectingBomb = true;
			AnimInstance->Montage_Play(BombAnimation, 2.0f);
		}
	}
}

void AKleeAdventureCharacter::EmitBomb()
{
	// try and fire a projectile
	if (ProjectileBombClass != nullptr)
	{
		AKleeBomb * Projectile;
		
		UWorld* const World = GetWorld();
		if (World != nullptr)
		{
			FVector GunOffset(40,0,50);
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
			Projectile = World->SpawnActor<AKleeBomb>(ProjectileBombClass, SpawnLocation, SpawnRotation, ActorSpawnParams);
			if(IsValid(Projectile)) Projectile->SetProjector(this);
			RandomPlay(this->BombSounds);
		}
	}
}

void AKleeAdventureCharacter::TurnAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void AKleeAdventureCharacter::LookUpAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}

void AKleeAdventureCharacter::MoveForward(float Value)
{
	if(IsProjectingBomb) return;
	if ((Controller != nullptr) && (Value != 0.0f))
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		AddMovementInput(Direction, Value * (IsSprinting ?  SprintingSpeed : WalkingSpeed));
	}
}

void AKleeAdventureCharacter::MoveRight(float Value)
{
	if(IsProjectingBomb) return;
	if ((Controller != nullptr) && (Value != 0.0f))
	{
		// find out which way is right
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get right vector 
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		// add movement in that direction
		AddMovementInput(Direction, Value * (IsSprinting ?  SprintingSpeed : WalkingSpeed));
	}
}
