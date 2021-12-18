// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "KleeAdventure/KleeAdventureCharacter.h"
#include "KleeAdventureEnemy.generated.h"


UENUM(BlueprintType)
enum class EEnemyMovementStatus: uint8
{
	MS_Idle UMETA(DisplayName = "Idle"),
	MS_MoveToTarget UMETA(DisplayName = "MoveToTarget"),
	MS_Attacking UMETA(DisplayName = "Attacking")
};

UCLASS()
class KLEEADVENTURE_API AKleeAdventureEnemy : public AKleeAdventureCharacter
{
	GENERATED_BODY()

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	class USphereComponent* DetectSphere;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	class USphereComponent* AttackSphere;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	EEnemyMovementStatus MovementStatus = EEnemyMovementStatus::MS_Idle;

	class AAIController* AIController;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	AKleeAdventureCharacter* TargetCharacter;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	AKleeAdventureCharacter* HittingCharacter;

public:
	UFUNCTION()
	void OnDetectSphereOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	                                UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
	                                const FHitResult& SweepResult);

	UFUNCTION()
	void OnDetectSphereOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	                              UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UFUNCTION()
	void OnAttackSphereOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	                                UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
	                                const FHitResult& SweepResult);

	UFUNCTION()
	void OnAttackSphereOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	                              UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UFUNCTION(BlueprintCallable)
	void MoveToTarget();
	
public:
	// Sets default values for this character's properties
	AKleeAdventureEnemy();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
};
