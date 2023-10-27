// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GrabableActor.h"
#include "Engine/DataTable.h"
#include "Food.h"
#include "DA_Recipes.h"
#include "Dish.generated.h"

class ADeliveryStation;

/**
 * 
 */

UCLASS()
class STIRCRAZY_API ADish : public AGrabableActor
{
	GENERATED_BODY()

	ADish();

public:
	//Runtime values

	UPROPERTY(Replicated, BlueprintReadOnly)
	TArray<AFood*> Contents;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TArray<UDA_Recipes*> IntendedRecipes;

	UPROPERTY(EditAnywhere)
	UBoxComponent* FoodCollision;

	UPROPERTY()
	ADeliveryStation* OverlappedStation;

	UFUNCTION(Server, Reliable)
	void PlateFood(AFood* Food);
	virtual void PlateFood_Implementation(AFood* Food);
	

	UFUNCTION(Server, Reliable)
	void DeliverDishServer(ADeliveryStation* Station);
	virtual void DeliverDishServer_Implementation(ADeliveryStation* Station);

	void Drop() override;


protected:

	bool ValidateContentsWithRecipe(UDA_Recipes* Recipe);

	virtual void BeginPlay() override;
	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION()
	void OnFoodBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnFoodEndOverlap(UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
	
};
