// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "Food.h"
#include "DA_Recipes.generated.h"

/**
 * 
 */

USTRUCT(BlueprintType)
struct FIngredientInfo
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	TSubclassOf<AFood> FoodClass;

	UPROPERTY(EditAnywhere)
	TEnumAsByte<ECookMethod> CookMethod = ECookMethod::None;

	UPROPERTY(EditAnywhere)
	TEnumAsByte<ECookedState> CookState = ECookedState::Raw;
};

UCLASS()
class STIRCRAZY_API UDA_Recipes : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly) FText RecipeName;

	UPROPERTY(EditAnywhere, BlueprintReadOnly) int Points;

	UPROPERTY(EditAnywhere, BlueprintReadOnly) TArray<FIngredientInfo> Ingredients;

	UPROPERTY(EditAnywhere, BlueprintReadOnly) TSubclassOf<AFood> PlatedFood;
	
};
