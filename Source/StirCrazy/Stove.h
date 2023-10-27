// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/BoxComponent.h"
#include "GrabableActor.h"
#include "Food.h"
#include "Stove.generated.h"

UENUM(BlueprintType)
enum CurrentHeat
{
	ELow_Heat,
	EMedium_Heat,
	EHigh_Heat
};

UCLASS()
class STIRCRAZY_API AStove : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AStove();
	//Default Scene Root
	UPROPERTY(EditAnywhere) USceneComponent* DefaultSceneRoot;

	UPROPERTY(EditAnywhere, BlueprintReadOnly) USkeletalMeshComponent* StoveMesh;

	UPROPERTY(EditAnywhere) UBoxComponent* DishCollision;
	UFUNCTION() void OnDishOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION() void OnDishEndOverlap(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	/*DEV ONLY*/
	UPROPERTY(EditAnywhere) UBoxComponent* HandleGripCollision;

	//default values
	UPROPERTY(BlueprintReadOnly, EditAnywhere) bool bDoorOpen;

	UPROPERTY(ReplicatedUsing = OnRepItemInOven) AGrabableActor* ItemInOven;
	UFUNCTION() void OnRepItemInOven();

	//Binds to OnDrop Delegate.
	UFUNCTION()
	void MoveInsideOven(AGrabableActor* Item);
	
	UFUNCTION(Server, Reliable)
	void S_MoveInsideOven(AGrabableActor* Item);
	virtual void S_MoveInsideOven_Implementation(AGrabableActor* Item);

	UFUNCTION(Server, Reliable)
	void StartCookingItem(AFood* Food);
	virtual void StartCookingItem_Implementation(AFood* Food);

	UFUNCTION(Server, Reliable)
	void RemoveFromOven(AFood* Food);
	virtual void RemoveFromOven_Implementation(AFood* Food);

	UFUNCTION()
	void UpdateItemCookState(TEnumAsByte<ECookedState> State);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
