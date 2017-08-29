// Fill out your copyright notice in the Description page of Project Settings.

#include "Darkness.h"
#include "Darkness/Public/Base/RootCharacter.h"
#include "Animation/AnimMontage.h"
#include "Animation/AnimInstance.h"
#include "Darkness/Public/Base/MyPlayerController.h"

ARootCharacter::ARootCharacter(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	GetMesh()->bOnlyOwnerSee = false;
	GetMesh()->bOwnerNoSee = false;
	GetMesh()->bReceivesDecals = true;
	GetMesh()->SetCollisionObjectType(ECC_Pawn);
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	GetMesh()->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);

	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);

	BaseTurnRate = 45.0f;
}

void ARootCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();
}

void ARootCharacter::Destroyed()
{
	Super::Destroyed();
}

void ARootCharacter::PossessedBy(class AController* InController)
{
	Super::PossessedBy(InController);
}

void ARootCharacter::FellOutOfWorld(const class UDamageType& dmgType)
{
	Die();
}

void ARootCharacter::Die()
{
	OnDeath();
}

void ARootCharacter::OnDeath()
{
	DetachFromControllerPendingDestroy();
	StopAllAnimMontages();
	Destroy();
}

bool ARootCharacter::IsMoving()
{
	return FMath::Abs(GetLastMovementInputVector().Size()) > 0.0f;
}

float ARootCharacter::PlayAnimMontage(class UAnimMontage* AnimMontage, float InPlayRate, FName StartSectionName)
{
	USkeletalMeshComponent* UseMesh = GetMesh();
	if (AnimMontage && UseMesh && UseMesh->AnimScriptInstance)
	{
		return UseMesh->AnimScriptInstance->Montage_Play(AnimMontage, InPlayRate);
	}

	return 0.0f;
}

void ARootCharacter::StopAnimMontage(class UAnimMontage* AnimMontage)
{
	USkeletalMeshComponent* UseMesh = GetMesh();
	if (AnimMontage && UseMesh && UseMesh->AnimScriptInstance &&
		UseMesh->AnimScriptInstance->Montage_IsPlaying(AnimMontage))
	{
		UseMesh->AnimScriptInstance->Montage_Stop(AnimMontage->BlendOut.GetBlendTime(), AnimMontage);
	}
}

void ARootCharacter::StopAllAnimMontages()
{
	USkeletalMeshComponent* UseMesh = GetMesh();
	if (UseMesh && UseMesh->AnimScriptInstance)
	{
		UseMesh->AnimScriptInstance->Montage_Stop(0.0f);
	}
}

void ARootCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	check(PlayerInputComponent);

	PlayerInputComponent->BindAxis("MoveForward", this, &ARootCharacter::MoveForward);
	PlayerInputComponent->BindAxis("Climb", this, &ARootCharacter::MoveUp);

	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ARootCharacter::OnStartJump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ARootCharacter::OnStopJump);

	PlayerInputComponent->BindAction("Crouch", IE_Pressed, this, &ARootCharacter::OnStartCrouching);
	PlayerInputComponent->BindAction("Crouch", IE_Released, this, &ARootCharacter::OnStopCrouching);


}

void ARootCharacter::MoveForward(float Val)
{
	if (Controller && Val != 0.0f)
	{
		const bool bLimitRotation = (GetCharacterMovement()->IsMovingOnGround() || GetCharacterMovement()->IsFalling());
		const FRotator Rotation = bLimitRotation ? GetActorRotation() : Controller->GetControlRotation();
		const FVector Direction = FRotationMatrix(Rotation).GetScaledAxis(EAxis::X);
		AddMovementInput(Direction, Val);
	}
}

void ARootCharacter::MoveUp(float Val)
{
	if (Val != 0.0f)
	{
		// Not when walking or falling
		if (GetCharacterMovement()->IsMovingOnGround() || GetCharacterMovement()->IsFalling())
		{
			return;
		}

		AddMovementInput(FVector::UpVector, Val);
	}
}

void ARootCharacter::TurnAtRate(float Val)
{
	AddControllerYawInput(Val * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void ARootCharacter::OnStartJump()
{
	AMyPlayerController* MyPC = Cast<AMyPlayerController>(Controller);
	if (MyPC && MyPC->IsGameInputAllowed())
	{
		bPressedJump = true;
	}
}

void ARootCharacter::OnStopJump()
{
	bPressedJump = false;
	StopJumping();
}

void ARootCharacter::OnStartCrouchToggle()
{
	AMyPlayerController* MyPC = Cast<AMyPlayerController>(Controller);
	if (MyPC && MyPC->IsGameInputAllowed())
	{
		SetCrouching(true, true);
	}
}

void ARootCharacter::SetCrouching(bool bNewCrouching, bool bToggle)
{
	bWantsToCrouch = bNewCrouching;
	bWantsToCrouchToggled = bNewCrouching && bToggle;
}

void ARootCharacter::OnStartCrouching()
{
	AMyPlayerController* MyPC = Cast<AMyPlayerController>(Controller);
	if (MyPC && MyPC->IsGameInputAllowed())
	{
		SetCrouching(true, false);
	}
}

void ARootCharacter::OnStopCrouching()
{
	SetCrouching(false, false);
}

bool ARootCharacter::IsCrouching() const
{
	if (!GetCharacterMovement())
	{
		return false;
	}

	return (bWantsToCrouch || bWantsToCrouchToggled);
}

void ARootCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (bWantsToCrouchToggled && !IsCrouching())
	{
		SetCrouching(false, false);
	}
}

void ARootCharacter::BeginDestroy()
{
	Super::BeginDestroy();
}

bool ARootCharacter::IsAlive() const
{
	return Health > 0;
}

bool ARootCharacter::IsEnemyFor(AController* TestPC) const
{
	if (TestPC == Controller || TestPC == NULL)
	{
		return false;
	}

	bool bIsEnemy = false;

	if (this->ActorHasTag("Player") && !TestPC->ActorHasTag("Player")
		|| !this->ActorHasTag("Player") && TestPC->ActorHasTag("Player")
		|| this->ActorHasTag("Enemy") && !TestPC->ActorHasTag("Enemy")
		|| !this->ActorHasTag("Enemy") && TestPC->ActorHasTag("Enemy"))
	{
		bIsEnemy = true;
	}

	return bIsEnemy;
}