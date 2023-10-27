// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/BoxComponent.h"
#include "ChoppingBoard.generated.h"

UCLASS()
class STIRCRAZY_API AChoppingBoard : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AChoppingBoard();

	UPROPERTY(EditAnywhere)
	USceneComponent* Root;
	
	UPROPERTY(EditAnywhere)
	UStaticMeshComponent* BoardMesh;
	
	UPROPERTY(EditAnywhere)
	USceneComponent* SlicePile0;

	//In case the first slice pile is obstructed
	UPROPERTY(EditAnywhere)
	USceneComponent* SlicePile1;

	UPROPERTY(EditAnywhere)
	UBoxComponent* FoodCollision;

	UFUNCTION(BlueprintCallable)
	USceneComponent* GetAvailableSlicePile();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION()
	void OnBoardOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnBoardEndOverlap(UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
