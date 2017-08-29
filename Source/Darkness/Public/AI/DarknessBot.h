// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Darkness/Public/Base/RootCharacter.h"
#include "DarknessBot.generated.h"

/**
 * 
 */
UCLASS(Abstract)
class DARKNESS_API ADarknessBot : public ARootCharacter
{
	GENERATED_UCLASS_BODY()
	
	UPROPERTY(EditAnywhere, Category = Behavior)
	class UBehaviorTree* BotBehavior;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Behavior)
	float ViewRange = 750.0f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "BaseStats")
	float SpeedRate = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "BaseStats")
	float SpeedVariable = 150.0f;

public:

	float GetViewRange() const;
	float GetViewRangeSquared() const;	
	
};
