// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "VrPawn.h"
#include "DA_Recipes.h"
#include "StirCrazyGameModeBase.generated.h"


DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnDeliver, AVrPawn*, Player, int, Points);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnOrder, UDA_Recipes*, Recipe);

/**
 * 
 */
UCLASS()
class STIRCRAZY_API AStirCrazyGameModeBase : public AGameModeBase
{
	GENERATED_BODY()

	AStirCrazyGameModeBase();

public:
	UPROPERTY(BlueprintAssignable, BlueprintCallable)
	FOnDeliver OnDeliver;

	UPROPERTY(BlueprintAssignable, BlueprintCallable)
	FOnOrder OnOrder;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void DeliverFood(ADish* Dish, AVrPawn* Player);

	//In seconds. -1 for infinite
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	float RoundLength;

	UFUNCTION()
	void EndRound();


protected:
	virtual void BeginPlay() override;

private:
	/////////Thunks
	virtual void DeliverFood_Implementation(ADish* Dish, AVrPawn* Player);

	
	
};

