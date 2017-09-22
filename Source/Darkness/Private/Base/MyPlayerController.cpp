// Fill out your copyright notice in the Description page of Project Settings.

#include "Darkness.h"
#include "Darkness/Public/Base/MyPlayerController.h"
#include "Logging/MessageLog.h"

DEFINE_LOG_CATEGORY(LogPlayerController);
#define LOCTEXT_NAMESPACE "MyPlayerController"

AMyPlayerController::AMyPlayerController(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	bAllowGameActions = true;
	bGameEndedFrame = false;
	LastDeathLocation = FVector::ZeroVector;
}

void AMyPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();
}

void AMyPlayerController::PostInitializeComponents()
{
	Super::PostInitializeComponents();
}

void AMyPlayerController::BeginPlay()
{
	Super::BeginPlay();
}

void AMyPlayerController::TickActor(float DeltaTime, enum ELevelTick TickType, FActorTickFunction& ThisTickFunction)
{
	Super::TickActor(DeltaTime, TickType, ThisTickFunction);

	// Is this the first frame after the game ended
	if (bGameEndedFrame)
	{
		bGameEndedFrame = false;


	}
}

void AMyPlayerController::BeginDestroy()
{
	Super::BeginDestroy();
}

void AMyPlayerController::GameHasEnded(class AActor* EndGameFocus, bool bIsWinner)
{
	UpdateSaveFileOnGameEnd(bIsWinner);

	Super::GameHasEnded(EndGameFocus, bIsWinner);
}

void AMyPlayerController::SimulateInputKey(FKey Key, bool bPressed)
{
	InputKey(Key, bPressed ? IE_Pressed : IE_Released, 1, false);
}

//void AMyPlayerController::OnToggleInGameMenu()
//{
//	if (GEngine->GameViewport == nullptr)
//	{
//		return;
//	}
//	UWorld* GameWorld = GEngine->GameViewport->GetWorld();
//
//	for (auto It = GameWorld->GetControllerIterator(); It; ++It)
//	{
//		AMyPlayerController* Con = Cast<AMyPlayerController>(*It);
//		if (Con && Con->IsPaused())
//		{
//			return;
//		}
//	}
//
//	// if no one's paused, pause - TODO: Send off event for opening the pause menu
//	//ToggleInGameMenu();
//}

void AMyPlayerController::SetGodMode(bool bEnable)
{
	bGodMode = bEnable;
}

bool AMyPlayerController::IsMoveInputIgnored() const
{
	if (IsInState(NAME_Spectating))
	{
		return false;
	}
	else
	{
		return Super::IsMoveInputIgnored();
	}
}

void AMyPlayerController::InitInputSystem()
{
	Super::InitInputSystem();
}

bool AMyPlayerController::HasGodMode() const
{
	return bGodMode;
}

bool AMyPlayerController::IsGameInputAllowed() const
{
	return bAllowGameActions;
}

bool AMyPlayerController::SetPause(bool bPause, FCanUnpause CanUnpauseDelegate  /*= FCanUnpause()*/)
{
	const bool Result = APlayerController::SetPause(bPause, CanUnpauseDelegate);

	return Result;
}

void AMyPlayerController::SetPlayer(UPlayer * InPlayer)
{
	Super::SetPlayer(InPlayer);

	FInputModeGameOnly InputMode;
	SetInputMode(InputMode);
}

void AMyPlayerController::UpdateSaveFileOnGameEnd(bool bIsWinner)
{

}

void AMyPlayerController::Possess(APawn * PawnToPossess)
{
	if (!HasAuthority())
	{
		FMessageLog("PIE").Warning(FText::Format(
			LOCTEXT("PlayerControllerPossessAuthorityOnly", "Possess function should only be used by the network authority for {0}"),
			FText::FromName(GetFName())
		));
		UE_LOG(LogPlayerController, Warning, TEXT("Trying to possess %s without network authority! Request will be ignored."), *GetNameSafe(PawnToPossess));
		return;
	}

	if (PawnToPossess != NULL &&
		(PlayerState == NULL || !PlayerState->bOnlySpectator))
	{
		if (GetPawn() && GetPawn() != PawnToPossess)
		{
			UnPossess();
		}

		if (PawnToPossess->Controller != NULL)
		{
			PawnToPossess->Controller->UnPossess();
		}

		PawnToPossess->PossessedBy(this);

		// update rotation to match possessed pawn's rotation
		SetControlRotation(PawnToPossess->GetActorRotation());

		SetPawn(PawnToPossess);
		check(GetPawn() != NULL);

		if (GetPawn() && GetPawn()->PrimaryActorTick.bStartWithTickEnabled)
		{
			GetPawn()->SetActorTickEnabled(true);
		}

		INetworkPredictionInterface* NetworkPredictionInterface = GetPawn() ? Cast<INetworkPredictionInterface>(GetPawn()->GetMovementComponent()) : NULL;
		if (NetworkPredictionInterface)
		{
			NetworkPredictionInterface->ResetPredictionData_Server();
		}

		AcknowledgedPawn = NULL;

		// Local PCs will have the Restart() triggered right away in ClientRestart (via PawnClientRestart()), but the server should call Restart() locally for remote PCs.
		// We're really just trying to avoid calling Restart() multiple times.
		if (!IsLocalPlayerController())
		{
			GetPawn()->Restart();
		}

		ClientRestart(GetPawn());

		ChangeState(NAME_Playing);
		if (bAutoManageActiveCameraTarget)
		{
			AutoManageActiveCameraTarget(GetPawn());
			ResetCameraMode();
		}
		UpdateNavigationComponents();
	}
}