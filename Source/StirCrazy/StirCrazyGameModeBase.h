// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "VrPawn.h"
#include "DA_Recipes.h"
#include "OrderSequenceData.h"
#include "StirCrazyGameModeBase.generated.h"


DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnDeliver, AVrPawn*, Player, int, Points);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnOrder, UDA_Recipes*, Recipe);

/**
 * 
 */


/*May or may not implement */
// UENUM(BlueprintType)
// enum EModeState
// {
// 	WaitingForPlayers,
// 	StandBy,
// 	Started,
// 	Ending,
// 	Ended,
// 	Paused
// };

UCLASS()
class STIRCRAZY_API AStirCrazyGameModeBase : public AGameModeBase
{
	GENERATED_BODY()

public:
	AStirCrazyGameModeBase();
	
	UPROPERTY(BlueprintAssignable, BlueprintCallable)
	FOnDeliver OnDeliver;

	UPROPERTY(BlueprintAssignable, BlueprintCallable)
	FOnOrder OnOrder;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void FulfillOrder(int OrderIndex, ADish* Dish, AVrPawn* Player);

	//In seconds. -1 for infinite
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	float RoundLength;

	UFUNCTION()
	void EndRound();

	UPROPERTY()
	int ExpectedPlayers;

	UFUNCTION()
	virtual void AddOrder(FOrderData Order);

protected:
	virtual void BeginPlay() override;

	UFUNCTION()
	virtual void StartGame();

private:
	/////////Thunks
	virtual void FulfillOrder_Implementation(int OrderIndex, ADish* Dish, AVrPawn* Player);

	
	
};

