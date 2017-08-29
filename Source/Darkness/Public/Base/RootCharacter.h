// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Character.h"
#include "RootCharacter.generated.h"

UCLASS(Abstract)
class DARKNESS_API ARootCharacter : public ACharacter
{
	GENERATED_UCLASS_BODY()

	virtual void BeginDestroy() override;

	/* Setup initial variables*/
	virtual void PostInitializeComponents() override;

	// Update loop for this class
	virtual void Tick(float DeltaTime) override;

	virtual void Destroyed() override;

	virtual void PossessedBy(class AController* C) override;

	// Used to determine if the controlled character should be looking
	// at the character controlled by the passed in controller.
	bool IsEnemyFor(AController* TestPC) const;

	void SetCrouching(bool bNewCrouching, bool bToggle);

	//==================================================================
	// Animation Montage Stuff

	/* Play Animation Montage */
	virtual float PlayAnimMontage(class UAnimMontage* AnimMontage, float InPlayRate = 1.0f, FName StartSectionName = NAME_None) override;

	/* Stop playing montage */
	virtual void StopAnimMontage(class UAnimMontage* AnimMontage) override;

	/* Stop playing all montages */
	void StopAllAnimMontages();

	//==================================================================
	// Input Handlers

	/* Setup pawn specific input handlers */
	virtual void SetupPlayerInputComponent(class UInputComponent* InputComponent) override;

	/*====================================
	Move forward/back
	Input Val = movement input to apply
	======================================*/
	void MoveForward(float Val);

	/*====================================
	Climb up/down when allowed
	Input Val = movement input to apply
	======================================*/
	void MoveUp(float Val);

	/* Frame rate independent turn */
	void TurnAtRate(float Val);

	/* Player pressed jump action. */
	void OnStartJump();

	/* Player released jump action */
	void OnStopJump();

	/* Player pressed crouch action*/
	void OnStartCrouching();

	/* Player pressed toggled crouch action*/
	void OnStartCrouchToggle();

	/* Player released crouch action*/
	void OnStopCrouching();

	//==================================================================
	// Reading data

	UFUNCTION(BlueprintCallable, Category = "Pawn")
	bool IsCrouching() const;

	UFUNCTION(BlueprintCallable, Category = "Pawn")
	bool IsAlive() const;

protected:

	/* Base turn rate, in deg/sec. */
	float BaseTurnRate;

	// Current crouching state
	uint8 bWantsToCrouch : 1;

	uint8 bWantsToCrouchToggled : 1;

private:

	// Whether or not the character is moving
	// (Based on movement input)
	bool IsMoving();

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Health")
	float Health;

	virtual void Die();

	virtual void FellOutOfWorld(const class UDamageType& dmgType) override;

protected:

	virtual void OnDeath();
};
