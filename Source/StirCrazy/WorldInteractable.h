// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/BoxComponent.h"
#include "GameFramework/Actor.h"

#include "WorldInteractable.generated.h"

class AVrPawn;

UCLASS()
class STIRCRAZY_API AWorldInteractable : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AWorldInteractable();

	UPROPERTY(EditAnywhere)
	USceneComponent* DefaultSceneRoot;

	UPROPERTY(EditAnywhere)
	UBoxComponent* HandCollision;

	UFUNCTION(BlueprintNativeEvent)
	void OnInteract(AVrPawn* Player, bool bIsRightHand);
	virtual void OnInteract_Implementation(AVrPawn* Player, bool bIsRightHand);

	bool bInteractNeedsOwnership;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
};
