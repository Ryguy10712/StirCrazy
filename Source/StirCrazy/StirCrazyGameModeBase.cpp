// Copyright Epic Games, Inc. All Rights Reserved.


#include "StirCrazyGameModeBase.h"
#include "SCGameStateBase.h"

AStirCrazyGameModeBase::AStirCrazyGameModeBase()
{
	GameStateClass = ASCGameStateBase::StaticClass();

	bReplicates = true;
}

void AStirCrazyGameModeBase::AddOrder(FOrderData Order)
{
	;
}

void AStirCrazyGameModeBase::BeginPlay()
{
	Super::BeginPlay();
	StartGame();
}

void AStirCrazyGameModeBase::StartGame()
{
	GetGameState<ASCGameStateBase>()->StartRoundTimer(this);
}

void AStirCrazyGameModeBase::FulfillOrder_Implementation(int OrderIndex, ADish* Dish, AVrPawn* Player)
{
	int DeliveryPoints = 0;
	//Determine points from this delivery
	for (AFood* Content : Dish->Contents)
	{
		if (Content->Recipe)
		{
			DeliveryPoints += Content->Recipe->Points;
		}
	}
	GetGameState<ASCGameStateBase>()->Team0Score += DeliveryPoints;
	OnDeliver.Broadcast(Player, DeliveryPoints);
	
	for (AFood* Content : Dish->Contents)
	{
		Content->Destroy();
	}

	Dish->Destroy();
	 
}

void AStirCrazyGameModeBase::EndRound()
{
	//Determine Winner
}


