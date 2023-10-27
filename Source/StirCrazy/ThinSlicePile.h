// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GrabableActor.h"
#include "ThinSlicePile.generated.h"

/**
 * When a Chopable Food is sliced thinly, it will spawn a ThinSlicePile which behaves as a single entity. 
 * This prevents a chopable food actor from being sliced too thinly and becoming ungrabable or hard to see.
 * Thin Slice piles can also be diced further in a dicer.
 */
UCLASS()
class STIRCRAZY_API AThinSlicePile : public AGrabableActor
{
	GENERATED_BODY()

	AThinSlicePile();

public:
	UPROPERTY(EditAnywhere)
	USceneComponent* DefaultSceneRoot;

	UPROPERTY(EditAnywhere)
	int MaxSlices;

	UPROPERTY(EditAnywhere)
	UStaticMesh* SliceMesh;

	UPROPERTY(EditAnywhere)
	TArray<FTransform> SliceTransforms;

	UPROPERTY()
	TArray<UStaticMeshComponent*> SliceMeshes;
	
	UFUNCTION(BlueprintCallable)
	void IncrementSlices(int Amount);

	UFUNCTION(BlueprintCallable)
	void SetSlices(int Amount);

	//Runtime variables
	UPROPERTY(VisibleAnywhere)
	int CurrentSlices;

	UFUNCTION(BlueprintCallable)
	void Break();

protected:
	virtual void BeginPlay() override;
	
};
