// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GrabableActor.h"
#include "Seasoning.h"
#include "Food.generated.h"

class UDA_Recipes;
class ADish;

/**
 * Class for non-chopable food. Can be cooked if isCookable is true.
 */

UENUM(BlueprintType)
enum ECookedState
{
	Raw,
	Medium,
	Well,
	Burnt,
	OnFire
};

UENUM(BlueprintType)
enum ECookMethod
{
	None,
	Baked,
	Fried,
	DeepFried
};

USTRUCT(BlueprintType)
struct FSeasoningInfo
{

	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly)
	TSubclassOf<ASeasoning> SeasoningType;

	//Amount in shakes
	UPROPERTY(BlueprintReadOnly)
	int Amount = 0;


};


UCLASS()
class STIRCRAZY_API AFood : public AGrabableActor
{
	GENERATED_BODY()

	AFood();

public:
	
	//Default Values
	UPROPERTY(EditAnywhere)
	FName FoodName;

	UPROPERTY(Replicated)
	AStove* CurrentCooker;
	
	//The recipe this is based off of
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UDA_Recipes* Recipe;

	UPROPERTY(EditAnywhere)
	bool bIsCookable;

	UPROPERTY(EditAnywhere, Category = CookTimes)
	float CookTimeSeconds;

	UPROPERTY(EditAnywhere, Category = CookTimes)
	float BurnTimeSeconds;

	UPROPERTY(EditAnywhere, Category = CookMats)
	UMaterialInstance* CookedMaterial;

	UPROPERTY(EditAnywhere, Category = CookMats)
	UMaterialInstance* BurntMaterial;

	UPROPERTY(BlueprintReadOnly)
	bool CanBeSeasoned;

	UPROPERTY(BlueprintReadOnly, Replicated)
	TArray<FSeasoningInfo> Seasonings;

	UFUNCTION()
	TArray<FSeasoningInfo> AddSeasoning(TSubclassOf<ASeasoning> Type, int Amount);

	UPROPERTY(EditAnywhere, Category = PlateProperties)
	FVector PlatedOffset;

	//Runtime values
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRepCookState)
	TEnumAsByte<ECookedState> CookedState;
	
	UFUNCTION()
	void OnRepCookState();

	UPROPERTY(BlueprintReadOnly, Replicated)
	TEnumAsByte<ECookMethod> CookMethod;

	UPROPERTY()
	AStove* OverlappedCooker;

	UPROPERTY()
	ADish* OverlappedDish;

	UPROPERTY(ReplicatedUsing = OnRep_CurrentDish)
	ADish* CurrentDish;

	UPROPERTY(Replicated)
	bool isPlated;

	FTimerHandle WellHandle;

	FTimerDelegate WellDelegate;
	
	FTimerHandle BurntHandle;

	FTimerDelegate BurntDelegate;

	UFUNCTION(Server, Reliable)
	void ServerRequestStartCooking(AStove* Cooker);
	virtual void ServerRequestStartCooking_Implementation(AStove* Cooker);

	UFUNCTION(Server, Reliable)
	void ServerRequestPlateFood(ADish* Dish);
	virtual void ServerRequestPlateFood_Implementation(ADish* Dish);

	void ServerHandlePickup_Implementation(AVrPawn* Player, bool isRightHand) override;

	void Drop() override;

protected:
	UFUNCTION()
	void OnRep_CurrentDish();
	
	virtual void BeginPlay() override;
	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
};
