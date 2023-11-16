// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Food.h"
#include "GrabableActor.h"
#include "Seasoning.generated.h"

/**
 * 
 */
UCLASS()
class STIRCRAZY_API ASeasoning : public AGrabableActor
{
	GENERATED_BODY()

	ASeasoning();

public:

	UPROPERTY(EditAnywhere)
	USceneComponent* DispenseLocation;
	
	//-1 means infinity
	UPROPERTY()
	int ShakesLeft;

protected:

	UPROPERTY()
	float ZVelocity;

	UPROPERTY()
	FVector LastTickLocation;

	UFUNCTION()
	void Shake();

	UFUNCTION(Server, Reliable)
	void StartAddSeasoning(AFood* Food);

	UFUNCTION(NetMulticast, Reliable)
	void ValidateShake();

	bool bShakeLock;
	
	/***OVERRIDES***/
	
	virtual void Tick(float DeltaTime) override;
	
};
