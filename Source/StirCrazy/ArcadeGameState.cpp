// Fill out your copyright notice in the Description page of Project Settings.


#include "ArcadeGameState.h"

void AArcadeGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AArcadeGameState, Orders);
	DOREPLIFETIME(AArcadeGameState, BackloggedOrders);
}
