// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/BoxComponent.h"
#include "Net/UnrealNetwork.h"
#include "MotionControllerComponent.h"

#include "GrabableActor.generated.h"

//Decalre event dispatchers
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnDropped, AGrabableActor*, Item);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPicked, AGrabableActor*, Item);

//Forward Declarations
class AVrPawn;

UCLASS()
class STIRCRAZY_API AGrabableActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AGrabableActor();

	//Event dispatchers other actors may bind to 
	UPROPERTY(BlueprintAssignable) FOnDropped OnDrop;
	UPROPERTY(BlueprintAssignable) FOnPicked OnPicked;

	//Grab Volume
	UPROPERTY(EditAnywhere, BlueprintReadWrite) UBoxComponent* GrabVolume;

	UPROPERTY(EditAnywhere, BlueprintReadWrite) UStaticMeshComponent* Mesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite) USkeletalMeshComponent* SkeletalMesh;
	
	//Every item will fit inside a hand differently, can be set here in blueprint
	UPROPERTY(EditAnywhere) FVector InHandOffset;
	UPROPERTY(EditAnywhere) FRotator InHandRotation;
	
	//Default Values
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_CurrentHolder)
	AVrPawn* CurrentHolder;

	UPROPERTY(BlueprintReadOnly, Replicated)
	AVrPawn* LastHolder;

	UPROPERTY(EditAnywhere)
	bool bIsSkeletal;

	//If the item is "locked" or not
	UPROPERTY(BlueprintReadOnly)
	bool NetLocked;

	UPROPERTY(ReplicatedUsing = OnRep_isSimPhysics)
	bool isSimulatingPhysics;

	UFUNCTION()
	void OnRep_isSimPhysics();

	UFUNCTION()
	virtual void PredictPickup(AVrPawn* Player, bool isRightHand);

	UFUNCTION(Server, Reliable)
	virtual void ServerHandlePickup(AVrPawn* Player, bool isRightHand);
	virtual void ServerHandlePickup_Implementation(AVrPawn* Player, bool isRightHand);

	UFUNCTION(Client, Reliable)
	void ClientRespondPickup(bool wasPredictionCorrect);
	virtual void ClientRespondPickup_Implementation(bool wasPredictionCorrect);

	UFUNCTION()
	virtual void Drop();

	UFUNCTION(Server, Reliable)
	void ServerDrop();
	virtual void ServerDrop_Implementation();

	//Multicast function to enable physics simulation
	UFUNCTION(NetMulticast, Reliable)
	void Physicalize(bool enable);

	//Helpers

	//-1 not held. 0 left 1 right
	UFUNCTION()
	int GetHeldHand();

	UPROPERTY(Replicated)
	bool bInRightHand;
	

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	//Replication
	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION()
	void OnRep_CurrentHolder();

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
