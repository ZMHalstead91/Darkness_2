// Fill out your copyright notice in the Description page of Project Settings.

#include "Darkness.h"
#include "Darkness/Public/Base/MyAIController.h"
#include "Darkness/Public/AI/DarknessBot.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"

AMyAIController::AMyAIController(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	BlackboardComp = ObjectInitializer.CreateDefaultSubobject<UBlackboardComponent>(this, TEXT("BlackboardComp"));

	BrainComponent = BehaviorComp = ObjectInitializer.CreateDefaultSubobject<UBehaviorTreeComponent>(this, TEXT("BehaviorComp"));

	bWantsPlayerState = true;
}

void AMyAIController::Possess(APawn* InPawn)
{
	Super::Possess(InPawn);

	ADarknessBot* Bot = Cast<ADarknessBot>(InPawn);

	// start behavior
	if (Bot && Bot->BotBehavior)
	{
		if (Bot->BotBehavior->BlackboardAsset)
		{
			BlackboardComp->InitializeBlackboard(*Bot->BotBehavior->BlackboardAsset);
		}

		EnemyKeyID = BlackboardComp->GetKeyID("Enemy");
		BehaviorComp->StartTree(*Bot->BotBehavior);
	}
}

void AMyAIController::UnPossess()
{
	Super::UnPossess();

	BehaviorComp->StopTree();
}

void AMyAIController::FindClosestEnemy()
{
	APawn* MyBot = GetPawn();
	if (MyBot == NULL)
		return;

	const FVector MyLoc = MyBot->GetActorLocation();
	float BestDistSq = MAX_FLT;
	ARootCharacter* BestPawn = NULL;

	for (FConstPawnIterator It = GetWorld()->GetPawnIterator(); It; ++It)
	{
		ARootCharacter* TestPawn = Cast<ARootCharacter>(*It);
		if (TestPawn && TestPawn->IsAlive() && TestPawn->IsEnemyFor(this))
		{
			const float DistSq = (TestPawn->GetActorLocation() - MyLoc).SizeSquared();
			if (DistSq < BestDistSq)
			{
				BestDistSq = DistSq;
				BestPawn = TestPawn;
			}
		}
	}

	if (BestPawn)
	{
		SetEnemy(BestPawn);
	}

}

bool AMyAIController::FindClosestEnemyWithLOS(ARootCharacter* ExcludeEnemy)
{
	bool bGotEnemy = false;
	ADarknessBot* MyBot = Cast<ADarknessBot>(GetPawn());
	if (MyBot != NULL)
	{
		const FVector MyLoc = MyBot->GetActorLocation();
		float BestDistSq = MyBot->GetViewRangeSquared();
		ARootCharacter* BestPawn = NULL;

		for (FConstPawnIterator It = GetWorld()->GetPawnIterator(); It; ++It)
		{
			ARootCharacter* TestPawn = Cast<ARootCharacter>(*It);
			if (TestPawn && TestPawn != ExcludeEnemy && TestPawn->IsAlive()/* && TestPawn->IsEnemyFor(this)*/)
			{
				if (HasEyeLOSToEnemy(TestPawn, true) == true)
				{
					const float DistSq = (TestPawn->GetActorLocation() - MyLoc).SizeSquared();
					if (DistSq < BestDistSq)
					{
						BestDistSq = DistSq;
						BestPawn = TestPawn;
					}
				}
			}
		}
		if (BestPawn)
		{
			SetEnemy(BestPawn);
			bGotEnemy = true;
		}
		else
		{
			SetEnemy(NULL);
		}
	}

	return bGotEnemy;
}

bool AMyAIController::EnemyInFront(AActor * InEnemyActor) const
{
	AActor* MyBot = Cast<AActor>(GetPawn());

	FVector EnemyLocation = InEnemyActor->GetActorLocation();
	FVector BotLocation = MyBot->GetActorLocation();
	FVector Heading = (EnemyLocation - BotLocation);
	Heading.Normalize();

	float dotProduct = FVector::DotProduct(Heading, MyBot->GetActorForwardVector());

	return (dotProduct > 0.0f) ? true : false;
}

bool AMyAIController::HasEyeLOSToEnemy(AActor* InEnemyActor, const bool bAnyEnemy) const
{
	static FName LosTag = FName(TEXT("AIEyeLosTrace"));
	//GetWorld()->DebugDrawTraceTag = LosTag;

	ARootCharacter* MyBot = Cast<ARootCharacter>(GetPawn());

	bool bHasLOS = false;

	FCollisionQueryParams TraceParams(LosTag, true, GetPawn());
	TraceParams.bTraceAsyncScene = true;
	TraceParams.bReturnPhysicalMaterial = true;

	FVector StartLocation = MyBot->GetActorLocation();
	StartLocation.Z += GetPawn()->BaseEyeHeight;

	FHitResult Hit(ForceInit);
	FVector EndLocation;
	FRotator tempRotator;
	InEnemyActor->GetActorEyesViewPoint(EndLocation, tempRotator);
	GetWorld()->LineTraceSingleByChannel(Hit, StartLocation, EndLocation, ECollisionChannel::ECC_Visibility, TraceParams);
	if (Hit.bBlockingHit == true)
	{
		// There is a blocking hit - check if it is our enemy actor
		AActor* HitActor = Hit.GetActor();
		if (HitActor != NULL && EnemyInFront(HitActor))
		{
			/*if (HitActor == InEnemyActor)
			{
				bHasLOS = true;
			}
			else */if (bAnyEnemy == true)
			{
				// It's not our actor, maybe it's still an enemy?
				ACharacter* HitChar = Cast<ACharacter>(HitActor);
				if (HitChar != NULL)
				{
					bHasLOS = (HitChar->ActorHasTag(TEXT("Player")));
				}
			}
		}
	}
	return bHasLOS;
}

void AMyAIController::SetEnemy(class APawn* InPawn)
{
	if (BlackboardComp)
	{
		BlackboardComp->SetValueAsObject(BlackboardComp->GetKeyName(EnemyKeyID), InPawn);
	}
}

class ARootCharacter* AMyAIController::GetEnemy() const
{
	return NULL;
}

void AMyAIController::GameHasEnded(AActor* EndGameFocus, bool bIsWinner)
{
	BehaviorComp->StopTree();

	StopMovement();

	SetEnemy(NULL);
}