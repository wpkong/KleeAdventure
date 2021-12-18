// Copyright Epic Games, Inc. All Rights Reserved.

#include "KleeAdventureCharacter.h"
#include "Components/SphereComponent.h"
#include <algorithm>
#include <random>
#include "HeadMountedDisplayFunctionLibrary.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "Sound/SoundCue.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/SpringArmComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"


void AKleeAdventureCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AKleeAdventureCharacter, CurrentHealth);
	DOREPLIFETIME(AKleeAdventureCharacter, MaxHealth);
}

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

	VisionSphere = CreateDefaultSubobject<USphereComponent>(TEXT("VisionSphere"));
	VisionSphere->SetupAttachment(RootComponent);
	VisionSphere->SetSphereRadius(800.0);

	bReplicates = true;
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
	PlayerInputComponent->BindAction("Skill", IE_Pressed, this, &AKleeAdventureCharacter::OnSkill);

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

void AKleeAdventureCharacter::OnVisionSphereOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                                                         UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
                                                         bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor == this || OtherActor == nullptr) return;

	if (auto Other = Cast<AKleeAdventureCharacter>(OtherActor))
	{
		CharactersInSight.Remove(Other);
	}
}

void AKleeAdventureCharacter::OnVisionSphereOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                                                       UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor == this || OtherActor == nullptr) return;

	if (auto Other = Cast<AKleeAdventureCharacter>(OtherActor))
	{
		CharactersInSight.Add(Other);
	}
}

void AKleeAdventureCharacter::RandomPlay(TArray<USoundBase*>& Sounds)
{
	if (Sounds.Num() == 0) return;

	auto RandomEngine = std::mt19937(std::random_device()());
	auto RandomDistribution = std::uniform_int_distribution<int>(0, Sounds.Num() - 1);
	int RandomValue = RandomDistribution(RandomEngine);
	USoundBase* Sound = Sounds[RandomValue];

	if (Sound != nullptr)
	{
		UGameplayStatics::PlaySoundAtLocation(this, Sound, GetActorLocation());
	}
}


void AKleeAdventureCharacter::BeginPlay()
{
	Super::BeginPlay();

	VisionSphere->OnComponentBeginOverlap.AddDynamic(this, &AKleeAdventureCharacter::OnVisionSphereOverlapBegin);
	VisionSphere->OnComponentEndOverlap.AddDynamic(this, &AKleeAdventureCharacter::OnVisionSphereOverlapEnd);
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
	if (BlockingMovementCount != 0) return;
	IsSprinting = true;
}

void AKleeAdventureCharacter::StopSprinting()
{
	if (BlockingMovementCount != 0) return;
	IsSprinting = false;
}

void AKleeAdventureCharacter::Jump()
{
	if (BlockingMovementCount != 0) return;
	IsJumping = true;
	Super::Jump();
}

void AKleeAdventureCharacter::StopJumping()
{
	if (BlockingMovementCount != 0) return;
	IsJumping = false;
	Super::StopJumping();
}

void AKleeAdventureCharacter::Zoom(float Value)
{
	if (Value != 0)
	{
		float ArmLength = CameraBoom->TargetArmLength + Value;
		CameraBoom->TargetArmLength = std::min(std::max(ArmLength, CameraRoomMinArmLength), CameraRoomMaxArmLength);
	}
}

void AKleeAdventureCharacter::OnFire()
{
}

void AKleeAdventureCharacter::OnSkill()
{
}

void AKleeAdventureCharacter::LockMovement()
{
	this->BlockingMovementCount++;
}

void AKleeAdventureCharacter::UnlockMovement()
{
	this->BlockingMovementCount = std::max(0, this->BlockingMovementCount - 1);
}

bool AKleeAdventureCharacter::IsLockingMovement()
{
	return (this->BlockingMovementCount != 0);
}

void AKleeAdventureCharacter::Cure(int32 Value)
{
	if (GetLocalRole() == ROLE_Authority)
	{
		CurrentHealth = std::min(CurrentHealth + Value, MaxHealth);
	}
	// ServerCure(Value);
}

void AKleeAdventureCharacter::Damage(int32 Value)
{
	if (GetLocalRole() == ROLE_Authority)
	{
		CurrentHealth = std::max(CurrentHealth - Value, 0);
		if (CurrentHealth <= 0) LockMovement();
	}
	// ServerDamage(Value);
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
	if (BlockingMovementCount != 0) return;
	if ((Controller != nullptr) && (Value != 0.0f))
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		AddMovementInput(Direction, Value * (IsSprinting ? SprintingSpeed : WalkingSpeed));
	}
}

void AKleeAdventureCharacter::MoveRight(float Value)
{
	if (BlockingMovementCount != 0) return;
	if ((Controller != nullptr) && (Value != 0.0f))
	{
		// find out which way is right
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get right vector 
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		// add movement in that direction
		AddMovementInput(Direction, Value * (IsSprinting ? SprintingSpeed : WalkingSpeed));
	}
}
