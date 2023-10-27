// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "VrPawn.h"
#include "GameFramework/Actor.h"

#include "WorldInteractable.h"

#include "ItemSpawner.generated.h"

UCLASS()
class STIRCRAZY_API AItemSpawner : public AWorldInteractable
{
	GENERATED_BODY()

public:
	AItemSpawner();
	
	UPROPERTY(EditAnywhere)
	UStaticMeshComponent* SpawnerMesh;

	UPROPERTY(EditAnywhere)
	USceneComponent* SpawnLocation;

	//Values
	UPROPERTY(EditAnywhere)
	TSubclassOf<AActor> SpawnClass;
	
	UPROPERTY(EditAnywhere)
	bool bAutoSpawn;

	UPROPERTY(EditAnywhere)
	bool bSpawnByGrasp;

	UPROPERTY(EditAnywhere)
	float SpawnInterval;

	UPROPERTY(EditAnywhere)
	int MaxSpawns;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	int CurrentSpawns;

	UFUNCTION(Server, Reliable, BlueprintCallable)
	void SpawnItem(AVrPawn* Player, bool bIsRightHand);

	void OnInteract_Implementation(AVrPawn* Player, bool bIsRightHand) override;

protected:
	virtual void BeginPlay() override;
	
	FTimerHandle SpawnHandle;
};
