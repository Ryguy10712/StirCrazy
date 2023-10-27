// Fill out your copyright notice in the Description page of Project Settings.


#include "WorldInteractable.h"


// Sets default values
AWorldInteractable::AWorldInteractable()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	DefaultSceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("Default Scene Root"));
	SetRootComponent(DefaultSceneRoot);

	HandCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("Hand Collision"));
	HandCollision->SetupAttachment(DefaultSceneRoot);

	bInteractNeedsOwnership = false;
}

// Called when the game starts or when spawned
void AWorldInteractable::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AWorldInteractable::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AWorldInteractable::OnInteract_Implementation(AVrPawn* Player, bool bIsRightHand)
{
	
}

