// Fill out your copyright notice in the Description page of Project Settings.


#include "Seasoning.h"

#include "Food.h"
#include "VrPawn.h"

ASeasoning::ASeasoning()
{
	ZVelocity = 0;
	ShakesLeft = -1;
	bShakeLock = false;
}


void ASeasoning::Tick(float DeltaTime)
{
	const FVector Difference = GetActorLocation() - LastTickLocation;
	ZVelocity = Difference.Z;

	LastTickLocation = GetActorLocation();

	if (ZVelocity < -10 && CurrentHolder && !bShakeLock)
	{
		Shake();
		bShakeLock = true;
	}

	if(ZVelocity >= 0 && bShakeLock)
	{
		GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Orange, "Unlocking");
		bShakeLock = false;
	}
}

void ASeasoning::Shake()
{
	//Start a downwards trace from the dispense location and check if it hits a food item
	FHitResult HitResult;
	FVector const Start = DispenseLocation->GetComponentLocation();
	const FVector End = Start + FVector(0, 0, -100);
	FCollisionQueryParams TraceParams;
	TraceParams.AddIgnoredActor(this);
	TraceParams.AddIgnoredActor(CurrentHolder);

	GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, ECC_Visibility, TraceParams);

	if(AFood* Food = Cast<AFood>(HitResult.GetActor()))
	{
		Food->AddSeasoning(this->GetClass(), 1);
	}
	
}


