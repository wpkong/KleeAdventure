// Fill out your copyright notice in the Description page of Project Settings.


#include "KleeAdventureEnemy.h"
#include "Components/SphereComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "AIController.h"
#include "Chaos/GeometryParticlesfwd.h"


// Sets default values
AKleeAdventureEnemy::AKleeAdventureEnemy(): AKleeAdventureCharacter()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	DetectSphere = CreateDefaultSubobject<USphereComponent>(TEXT("DetectSphere"));
	DetectSphere->SetupAttachment(RootComponent);
	DetectSphere->SetSphereRadius(600.0);

	AttackSphere = CreateDefaultSubobject<USphereComponent>(TEXT("AttackSphere"));
	AttackSphere->SetupAttachment(RootComponent);
	AttackSphere->SetSphereRadius(300.0);
}

// Called when the game starts or when spawned
void AKleeAdventureEnemy::BeginPlay()
{
	Super::BeginPlay();
	AIController = Cast<AAIController>(GetController());
	
	DetectSphere->OnComponentBeginOverlap.AddDynamic(this, &AKleeAdventureEnemy::OnDetectSphereOverlapBegin);
	DetectSphere->OnComponentEndOverlap.AddDynamic(this, &AKleeAdventureEnemy::OnDetectSphereOverlapEnd);

	AttackSphere->OnComponentBeginOverlap.AddDynamic(this, &AKleeAdventureEnemy::OnAttackSphereOverlapBegin);
	AttackSphere->OnComponentEndOverlap.AddDynamic(this, &AKleeAdventureEnemy::OnAttackSphereOverlapEnd);
}

// Called every frame
void AKleeAdventureEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

// Called to bind functionality to input
void AKleeAdventureEnemy::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}


void AKleeAdventureEnemy::OnDetectSphereOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                                                     UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
                                                     bool bFromSweep, const FHitResult& SweepResult)
{
	if (TargetCharacter != nullptr) return;
	if (OtherActor == nullptr) return;
	if(OtherActor == this) return;
	
	if (AKleeAdventureCharacter * Other = Cast<AKleeAdventureCharacter>(OtherActor))
	{
		// GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, OtherActor->GetName());
		TargetCharacter = Other;
		// GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("1Overlap2"));
		if (MovementStatus != EEnemyMovementStatus::MS_Attacking)
		{
			// GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("1Overlap3"));
			MovementStatus = EEnemyMovementStatus::MS_MoveToTarget;
		}
		// return;
		MoveToTarget();
		// GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("1Overlap4"));
	}
}

void AKleeAdventureEnemy::OnDetectSphereOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                                                   UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if(OtherActor == this) return;
	if (OtherActor == nullptr) return;
	auto Other = Cast<AKleeAdventureCharacter>(OtherActor);

	if (Other != TargetCharacter) return;

	TargetCharacter = nullptr;
	if (MovementStatus == EEnemyMovementStatus::MS_Attacking) MovementStatus = EEnemyMovementStatus::MS_Idle;

	if (AIController) AIController->StopMovement();
}

void AKleeAdventureEnemy::OnAttackSphereOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                                                     UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
                                                     bool bFromSweep, const FHitResult& SweepResult)
{
	if(OtherActor == this) return;
	if (HittingCharacter != nullptr) return;
	if (OtherActor == nullptr) return;
	if(Cast<AKleeAdventureEnemy>(OtherActor)) return;
	
	HittingCharacter = Cast<AKleeAdventureCharacter>(OtherActor);
	if (HittingCharacter != nullptr)
	{
		MovementStatus = EEnemyMovementStatus::MS_Attacking;
		if (AIController) AIController->StopMovement();
	}
}

void AKleeAdventureEnemy::OnAttackSphereOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                                                   UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if(OtherActor == this) return;
	if (OtherActor == nullptr) return;
	if(Cast<AKleeAdventureEnemy>(OtherActor)) return;
	
	auto Other = Cast<AKleeAdventureCharacter>(OtherActor);
	if (Other != HittingCharacter) return;
	TargetCharacter = HittingCharacter;
	HittingCharacter = nullptr;
}

void AKleeAdventureEnemy::MoveToTarget()
{
	if (TargetCharacter)
	{
		if (AIController)
		{
			// GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, TEXT("Move"));
			
			FAIMoveRequest MoveRequest;
			MoveRequest.SetGoalActor(TargetCharacter);
			MoveRequest.SetAcceptanceRadius(10);

			FNavPathSharedPtr NavPath;
			AIController->MoveTo(MoveRequest, &NavPath);

			if(NavPath == nullptr)
			{
				// GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, TEXT("Empty"));
				return;
			}
#if 0
			auto PathPoints = NavPath->GetPathPoints();
			for (auto Point : PathPoints)
			{
				FVector Location = Point.Location;
				UKismetSystemLibrary::DrawDebugSphere(this, Location);
			}
#endif
		}
	}
	else
	{
		MovementStatus = EEnemyMovementStatus::MS_Idle;
	}
}
