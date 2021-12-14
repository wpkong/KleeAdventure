// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "KleeAdventureCharacter.generated.h"

UCLASS(config=Game)
class AKleeAdventureCharacter : public ACharacter
{
	GENERATED_BODY()
public:
	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FollowCamera;

public:
	AKleeAdventureCharacter();

	/** Base turn rate, in deg/sec. Other scaling may affect final turn rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera)
	float BaseTurnRate;

	/** Base look up/down rate, in deg/sec. Other scaling may affect final rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera)
	float BaseLookUpRate;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category=Camera)
	float CameraRoomMaxArmLength = 400;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category=Camera)
	float CameraRoomMinArmLength = 250;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	float SprintingSpeed = 2.0;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	float WalkingSpeed = 0.2;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	bool IsSprinting = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	bool IsJumping = false;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	int Score = 0;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	int BlockingMovementCount = 0;
	
protected:
	virtual void BeginPlay() override;

	/** Resets HMD orientation in VR. */
	void OnResetVR();

	/** Called for forwards/backward input */
	void MoveForward(float Value);

	/** Called for side to side input */
	void MoveRight(float Value);

	/** 
	 * Called via input to turn at a given rate. 
	 * @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	 */
	void TurnAtRate(float Rate);

	/**
	 * Called via input to turn look up/down at a given rate. 
	 * @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	 */
	void LookUpAtRate(float Rate);

	/** Handler for when a touch input begins. */
	void TouchStarted(ETouchIndex::Type FingerIndex, FVector Location);

	/** Handler for when a touch input stops. */
	void TouchStopped(ETouchIndex::Type FingerIndex, FVector Location);

	UFUNCTION(BlueprintCallable)
	void Sprint();

	UFUNCTION(BlueprintCallable)
	void StopSprinting();

	virtual void Jump() override;

	virtual void StopJumping() override;

	virtual void Zoom(float Value);

	UFUNCTION(BlueprintCallable)
	virtual void OnFire();
	
	UFUNCTION(BlueprintCallable)
	virtual void OnSkill();

	UFUNCTION(BlueprintCallable)
	void LockMovement();

	UFUNCTION(BlueprintCallable)
	void UnlockMovement();

	UFUNCTION(BlueprintCallable)
	bool IsLockingMovement();
	
protected:
	// APawn interface
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent);
	// End of APawn interface

public:
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }

protected:
	void RandomPlay(TArray<class USoundBase*> &Sounds);
};
