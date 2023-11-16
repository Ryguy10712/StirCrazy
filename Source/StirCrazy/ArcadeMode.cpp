// Fill out your copyright notice in the Description page of Project Settings.


#include "ArcadeMode.h"

#include "ArcadeGameState.h"
#include "SCGameStateBase.h"
#include "VorbisAudioInfo.h"

AArcadeMode::AArcadeMode()
{
	CurrentSequenceIndex = 0;
	OrderSequenceAsset = nullptr;
	MaxParallelOrders = 5;
	GameStateClass = AArcadeGameState::StaticClass();
}

void AArcadeMode::AddOrder(FOrderData Order)
{
	TArray<FOrderData> Orders = GetGameState<AArcadeGameState>()->Orders;
	Orders.Add(Order);
	return;
}

void AArcadeMode::FulfillOrder_Implementation(int OrderIndex, ADish* Dish, AVrPawn* Player)
{
	AArcadeGameState* State = GetGameState<AArcadeGameState>();
	TArray<FOrderData> Orders = State->Orders;

	Orders.RemoveAt(OrderIndex);

	if(State->BackloggedOrders.Num() > 0)
	{
		AddOrder(State->BackloggedOrders[0]);
		State->BackloggedOrders.RemoveAt(0);
	}
		
}


void AArcadeMode::BeginPlay()
{
	//Super::BeginPlay();
	OrderSequence = OrderSequenceAsset->Orders;
	GetWorldTimerManager().SetTimer(InitialDelayHandle, this, &AArcadeMode::CheckAllPlayersConnected, 10.f, false);
}


void AArcadeMode::StartGame()
{
	GetGameState<ASCGameStateBase>()->StartRoundTimer(this);

	//Begin going through order sequence
	HandleOrderSequence();
}

void AArcadeMode::HandleOrderSequence()
{
	TArray<FOrderData> Orders = GetGameState<AArcadeGameState>()->Orders;

	//make sure the index isnt overstepping the sequence
	if(CurrentSequenceIndex > OrderSequence.Num() - 1)
	{
		//TODO figure out why order is not ordering the right amount of times
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Sequence over"));
		return;
	}
	
	if(Orders.Num() == MaxParallelOrders)
	{
		//Add to backlog
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Backlogged"));
		GetGameState<AArcadeGameState>()->BackloggedOrders.Add(OrderSequence[CurrentSequenceIndex]);
		CurrentSequenceIndex++;
		return;
	}

	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Order added"));
	AddOrder(OrderSequence[CurrentSequenceIndex]);
	GetWorldTimerManager().SetTimer(OrderSequenceHandle, this, &AArcadeMode::HandleOrderSequence, OrderSequence[CurrentSequenceIndex].TimeUntilNextOrder, false);
	CurrentSequenceIndex++;
}

void AArcadeMode::CheckAllPlayersConnected()
{
	const int ConnectedPlayers = GetGameState<ASCGameStateBase>()->PlayerArray.Num();
	if(ConnectedPlayers >= ExpectedPlayers || ExpectedPlayers == -1)
	{
		StartGame();
	}
	else
	{
		//wait for 15 more seconds
		GetWorldTimerManager().SetTimer(WaitForPlayersHandle, this, &AArcadeMode::EndWaitingForPlayers, 15.f, false);
	}
}

void AArcadeMode::EndWaitingForPlayers()
{
	if(CanStart())
	{
		StartGame();
	}
	else
	{
		//Idk do something
	}
}


bool AArcadeMode::CanStart()
{
	const int ConnectedPlayers = GetGameState<ASCGameStateBase>()->PlayerArray.Num();
	
	return ConnectedPlayers >= ExpectedPlayers;
}

