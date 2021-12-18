// Fill out your copyright notice in the Description page of Project Settings.


#include "HilichurlEnemy.h"

#include "Components/SphereComponent.h"

void AHilichurlEnemy::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	// PlayerInputComponent->BindAction("Fire", IE_Pressed, this, &AHilichurlEnemy::OnFire);
	// PlayerInputComponent->BindAction("Skill", IE_Pressed, this, &AHilichurlEnemy::OnSkill);
}

AHilichurlEnemy::AHilichurlEnemy(): AKleeAdventureEnemy()
{
	PrimaryActorTick.bCanEverTick = true;
}

void AHilichurlEnemy::BeginPlay()
{
	Super::BeginPlay();
}

void AHilichurlEnemy::OnFire()
{
	if (IsJumping) return;
	if (IsLockingMovement()) return;
	if (AttackAnimation != nullptr)
	{
		// Get the animation object for the arms mesh
		UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
		if (AnimInstance != nullptr)
		{
			// GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("Attack"));
			AnimInstance->Montage_Play(AttackAnimation, 1.f);
		}
	}
}

void AHilichurlEnemy::OnSkill()
{
	Super::OnSkill();
}

void AHilichurlEnemy::OnClawHit()
{
	if(HittingCharacter == nullptr) return;
	// GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("Hittttttt"));
	if(FVector::Dist(HittingCharacter->GetActorLocation(), this->GetActorLocation()) < AttackSphere->GetScaledSphereRadius())
	{
		// GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("Hittttttt22222"));
		HittingCharacter->Damage(10);
	}
}
