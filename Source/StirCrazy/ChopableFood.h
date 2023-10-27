// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GrabableActor.h"
#include "ProceduralMeshComponent.h"
#include "KismetProceduralMeshLibrary.h"
#include "ThinSlicePile.h"
#include "ChopableFood.generated.h"

class AChoppingBoard;

/**
 * 
 */

UCLASS()
class STIRCRAZY_API AChopableFood : public AGrabableActor
{
	GENERATED_BODY()

public:
	AChopableFood();


	//Multiplayer

	//if the last prediction was false, prevent further predictions until we know the client is reliable
	bool bPredictionsReliable;

	bool bInPredictedState;
	
	UFUNCTION(BlueprintCallable)
	virtual void PredictSlice(FVector Loc, FVector Normal, UProceduralMeshComponent* InComp);

	UFUNCTION(BlueprintCallable, Server, Reliable)
	void ServerSlice(FVector Loc, FVector Normal, UProceduralMeshComponent* InComp, int SliceCount);

	UFUNCTION(Client, Reliable)
	void RespondSliceCorrect();

	UFUNCTION(Client, Reliable)
	void RespondSliceIncorrect(const TArray<UProceduralMeshComponent*>& CorrectedSlices);
	
	//Procedural Mesh
	UPROPERTY(EditAnywhere) UProceduralMeshComponent* ProcMesh;
	UFUNCTION(BlueprintCallable) void Slice(FVector Loc, FVector Normal, UProceduralMeshComponent* InComp);
	UPROPERTY(EditAnywhere) UMaterialInterface* CapMaterial;

	UPROPERTY(EditAnywhere) UStaticMeshComponent* ThinlySlicedMesh;

	UPROPERTY(EditAnywhere)
	TSubclassOf<AThinSlicePile> ThinSlicePileClass;

	UPROPERTY()
	AThinSlicePile* CurrentSlicePile;

	//Default Values
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly) bool bCanBeSliced;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly) TArray<UProceduralMeshComponent*> Slices;
	UPROPERTY(BlueprintReadOnly) FTimerHandle ChopTimerGate;

	UPROPERTY(BlueprintReadOnly)
	AChoppingBoard* CurrentChoppingBoard;

protected:
	UPROPERTY() int CurrentChops;
	UFUNCTION() void ChopResetFunction();

	//Maximum size the mesh can be before being considered "diced"
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	float MaxThinSliceSize;

	virtual void BeginPlay() override;

private:
	FVector SlicePileLocation;

public:
	UPROPERTY(EditAnywhere) int MaxChops;
	
};
