// Fill out your copyright notice in the Description page of Project Settings.

#include "Darkness.h"
#include "Darkness/Public/AI/DarknessBot.h"
#include "Darkness/Public/Base/MyAIController.h"

ADarknessBot::ADarknessBot(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	AIControllerClass = AMyAIController::StaticClass();

	bUseControllerRotationYaw = false;
}

float ADarknessBot::GetViewRange() const
{
	return ViewRange;
}

float ADarknessBot::GetViewRangeSquared() const
{
	return ViewRange * ViewRange;
}