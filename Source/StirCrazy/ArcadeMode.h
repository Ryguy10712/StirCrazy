// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "OrderSequenceData.h"
#include "StirCrazyGameModeBase.h"
#include "ArcadeMode.generated.h"

/**
 * 
 */
UCLASS()
class STIRCRAZY_API AArcadeMode : public AStirCrazyGameModeBase
{
	GENERATED_BODY()

public:
	AArcadeMode();
	
	UPROPERTY(EditAnywhere);
	UOrderSequenceData* OrderSequenceAsset;

	UPROPERTY()
	TArray<FOrderData> OrderSequence;

	UPROPERTY()
	int CurrentSequenceIndex;

	UPROPERTY(EditAnywhere)
	int MaxParallelOrders;

protected:
	virtual void StartGame() override;

	virtual void AddOrder(FOrderData Order) override;

	UFUNCTION()
	void HandleOrderSequence();

	UFUNCTION()
	void CheckAllPlayersConnected();

	UFUNCTION()
	void EndWaitingForPlayers();

	UFUNCTION()
	bool CanStart();

	virtual void FulfillOrder_Implementation(int OrderIndex, ADish* Dish, AVrPawn* Player) override;
	
	virtual void BeginPlay() override;

	FTimerHandle OrderSequenceHandle;

	FTimerHandle InitialDelayHandle;

	FTimerHandle WaitForPlayersHandle;
	
};
