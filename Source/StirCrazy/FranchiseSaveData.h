// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "FranchiseSaveData.generated.h"

UENUM(BlueprintType)
enum EForgivenessLevel
{
	Canadian,
	Normal,
	HotHeaded,
	Karen,
};

/**
 * 
 */
UCLASS()
class STIRCRAZY_API UFranchiseSaveData : public USaveGame
{
	GENERATED_BODY()

	UPROPERTY()
	int Money;

	UPROPERTY()
	int MoneySpent;

	UPROPERTY()
	int MoneyEarned;

	UPROPERTY()
	int TotalOrders;

	UPROPERTY()
	int PerfectOrders;

	UPROPERTY()
	FName FranchiseName;

	UPROPERTY()
	int DaysCompleted;

	UPROPERTY()
	int Reputation;
	
	UPROPERTY()
	TEnumAsByte<EForgivenessLevel> Forgiveness;
	
};
