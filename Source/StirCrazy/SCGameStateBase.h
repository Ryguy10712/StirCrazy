// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "VrPawn.h"
#include "Dish.h"
#include "DA_Recipes.h"
#include "SCGameStateBase.generated.h"

class AStirCrazyGameModeBase;

/**
 * 
 */
UCLASS()
class STIRCRAZY_API ASCGameStateBase : public AGameStateBase
{
	GENERATED_BODY()

	ASCGameStateBase();

public:
	
	UPROPERTY(BlueprintReadWrite, Replicated)
	int Team0Score;

	UPROPERTY(Replicated)
	int Team1Score;

	UPROPERTY(Replicated)
	int Team2Score;

	UPROPERTY(Replicated)
	int Team3Score;

	UPROPERTY(BlueprintReadOnly, Replicated)
	float TimeRemaining;

	UPROPERTY(BlueprintReadOnly, Replicated)
	float TimeElapsed;

	UPROPERTY(BlueprintReadWrite, Replicated)
	TArray<UDA_Recipes*> OrderedRecipes;

	UPROPERTY(Replicated)
	bool allMovementDisabled;

	UFUNCTION(BlueprintCallable)
	void StartRoundTimer(AStirCrazyGameModeBase* Gamemode);


protected:
	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UPROPERTY(Replicated)
	FTimerHandle GameplayTimer;

private:
	
	
};
