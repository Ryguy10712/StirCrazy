// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DA_Recipes.h"
#include "Engine/DataAsset.h"
#include "OrderSequenceData.generated.h"

/**
 * 
 */
USTRUCT(BlueprintType)
struct FOrderData
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UDA_Recipes* Recipe;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float TimeLimit;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float TimeUntilNextOrder;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int Points;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int TimeReward;
};

UCLASS()
class STIRCRAZY_API UOrderSequenceData : public UPrimaryDataAsset
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere)
	TArray<FOrderData> Orders;
};
