// Fill out your copyright notice in the Description page of Project Settings.


#include "Food.h"
#include "Stove.h"
#include "Dish.h"
#include "DA_Recipes.h"

AFood::AFood()
{
	bReplicates = true;
	
	bIsCookable = false;
	CookTimeSeconds = 5.f;
	BurnTimeSeconds = 10.f;

	CookedState = Raw;
	CookMethod = None;

	isPlated = false;
	CanBeSeasoned = true;

	//nullptr
	OverlappedCooker = nullptr;
	OverlappedDish = nullptr;
	BurntMaterial = nullptr;
	CookedMaterial = nullptr;
	CurrentDish = nullptr;
	Recipe = nullptr;
}

void AFood::BeginPlay()
{
	Super::BeginPlay();

	//OnDrop.AddDynamic(this, &AFood::EventOnDrop);
}

#pragma region Pickup/Drop

void AFood::ServerHandlePickup_Implementation(AVrPawn* Player, bool isRightHand)
{
	if (CurrentCooker)
	{
		CurrentCooker->RemoveFromOven(this);
	}
	Super::ServerHandlePickup_Implementation(Player, isRightHand);
}


void AFood::Drop()
{
	DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
	CurrentHolder = nullptr;
	
	if (OverlappedDish)
	{
		ServerRequestPlateFood(OverlappedDish);
	}
	else if (OverlappedCooker && bIsCookable)
	{
		ServerRequestStartCooking(OverlappedCooker);
	}
	else
	{
		isSimulatingPhysics = true;
		OnRep_isSimPhysics();
		ServerDrop();
		SetOwner(nullptr);
	}

	
}

void AFood::ServerRequestPlateFood_Implementation(ADish* Dish)
{
	SetOwner(Dish);
	Dish->PlateFood(this);
}

#pragma region CookerLogic

void AFood::ServerRequestStartCooking_Implementation(AStove* Cooker)
{
	CurrentHolder = nullptr;
	isSimulatingPhysics = false;
	OnRep_isSimPhysics();
	Mesh->SetCollisionEnabled(ECollisionEnabled::QueryOnly);

	SetOwner(Cooker);
	Cooker->StartCookingItem(this);
}

#pragma endregion

#pragma region Getters & Setters

TArray<FSeasoningInfo> AFood::AddSeasoning(TSubclassOf<ASeasoning> Type, int Amount)
{
	FSeasoningInfo* Seasoning = Seasonings.FindByPredicate([Type](const FSeasoningInfo Seasoning) {
		return Seasoning.SeasoningType == Type;
	});
	
	if (Seasoning)
	{
		Seasoning->Amount += Amount;
	}
	else
	{
		Seasonings.Add({Type, Amount});
	}
	
	return Seasonings;
}

#pragma endregion


#pragma region Replication

void AFood::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AFood, CookedState)

	DOREPLIFETIME(AFood, CookMethod);

	DOREPLIFETIME(AFood, isPlated);

	DOREPLIFETIME(AFood, CurrentDish);

	DOREPLIFETIME(AFood, CurrentCooker);

	DOREPLIFETIME(AFood, Seasonings)
}

void AFood::OnRepCookState()
{
	GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Magenta, "OnRepCookState");
	switch (CookedState)
	{
		case Well:
			if (CookedMaterial)
			{
				Mesh->SetMaterial(0, CookedMaterial);
			}
			break;
		case Burnt:
			if (BurntMaterial)
			{
				Mesh->SetMaterial(0, BurntMaterial);
			}
			break;
	}
}

void AFood::OnRep_CurrentDish()
{
	if (CurrentDish == nullptr)
	{
		DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
	}
	else
	{
		Mesh->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
		SetOwner(CurrentDish);
		AttachToActor(CurrentDish, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
	}
}

#pragma endregion