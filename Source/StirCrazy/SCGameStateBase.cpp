// Fill out your copyright notice in the Description page of Project Settings.


#include "SCGameStateBase.h"
#include "StirCrazyGameModeBase.h"


ASCGameStateBase::ASCGameStateBase()
{
	Team0Score = 0;
	Team1Score = 0;
	Team2Score = 0;
	Team3Score = 0;

	allMovementDisabled = false;
}

void ASCGameStateBase::StartRoundTimer(AStirCrazyGameModeBase* Gamemode)
{
	if (HasAuthority())
	{
		TimeRemaining = Gamemode->RoundLength;

		FTimerDelegate Delegate;
		Delegate.BindLambda([this, Gamemode](){
			TimeRemaining -= 1;
			TimeElapsed += 1;

			if (TimeRemaining <= 0)
			{
				GetWorldTimerManager().ClearTimer(GameplayTimer);
				GameplayTimer.Invalidate();
				
				Gamemode->EndRound();
			}
		});


		GetWorldTimerManager().SetTimer(GameplayTimer, Delegate, 1.f, true, 0.f);
	}
}

void ASCGameStateBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ASCGameStateBase, Team0Score);
	DOREPLIFETIME(ASCGameStateBase, Team1Score);
	DOREPLIFETIME(ASCGameStateBase, Team2Score);
	DOREPLIFETIME(ASCGameStateBase, Team3Score);
	DOREPLIFETIME(ASCGameStateBase, OrderedRecipes);

	DOREPLIFETIME(ASCGameStateBase, allMovementDisabled);

	DOREPLIFETIME(ASCGameStateBase, GameplayTimer);
	DOREPLIFETIME(ASCGameStateBase, TimeRemaining);
	DOREPLIFETIME(ASCGameStateBase, TimeElapsed);
}