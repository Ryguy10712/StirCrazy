// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "OrderSequenceData.h"
#include "SCGameStateBase.h"
#include "ArcadeGameState.generated.h"

/**
 * 
 */
UCLASS()
class STIRCRAZY_API AArcadeGameState : public ASCGameStateBase
{
	GENERATED_BODY()

public:
	
	UPROPERTY(Replicated, BlueprintReadOnly)
	TArray<FOrderData> Orders;

	UPROPERTY(Replicated)
	TArray<FOrderData> BackloggedOrders;

private:
	//replication function
	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
};
