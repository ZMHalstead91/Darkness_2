// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "AIController.h"
#include "MyAIController.generated.h"

class UBehaviorTreeComponent;
class UBlackboardComponent;

UCLASS()
class DARKNESS_API AMyAIController : public AAIController
{
	GENERATED_BODY()

private:
	UBlackboardComponent* BlackboardComp;

	/* Cached BT component*/
	UBehaviorTreeComponent* BehaviorComp;
public:

	AMyAIController(const FObjectInitializer& ObjectInitializer);

	// Begin AController interface
	virtual void GameHasEnded(class AActor* EndGameFocus = NULL, bool bIsWinner = false) override;
	virtual void Possess(class APawn* InPawn) override;
	virtual void UnPossess() override;
	// End APlayerController interface

	void SetEnemy(class APawn* InPawn);

	class ARootCharacter* GetEnemy() const;

	UFUNCTION(BlueprintCallable, Category = Behavior)
	void FindClosestEnemy();

	UFUNCTION(BlueprintCallable, Category = Behavior)
	bool FindClosestEnemyWithLOS(ARootCharacter* ExcludeEnemy);

	bool EnemyInFront(AActor* InEnemyActor) const;

	bool HasEyeLOSToEnemy(AActor* InEnemyActor, const bool bAnyEnemy) const;

protected:

	int32 EnemyKeyID;

public:
	/** Returns BlackboardComp subobject **/
	FORCEINLINE UBlackboardComponent* GetBlackboardComp() const { return BlackboardComp; }
	/** Returns BehaviorComp subobject **/
	FORCEINLINE UBehaviorTreeComponent* GetBehaviorComp() const { return BehaviorComp; }

};
