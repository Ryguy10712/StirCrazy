// Fill out your copyright notice in the Description page of Project Settings.


#include "DeliveryStation.h"
#include "Kismet/GameplayStatics.h"
#include "StirCrazyGameModeBase.h"

// Sets default values
ADeliveryStation::ADeliveryStation()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Root = CreateDefaultSubobject<USceneComponent>("Root");
	SetRootComponent(Root);

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>("Mesh");
	Mesh->SetupAttachment(Root);

	DeliveryCollision = CreateDefaultSubobject<UBoxComponent>("Delivery Collision");
	DeliveryCollision->SetupAttachment(Root);
}

// Called when the game starts or when spawned
void ADeliveryStation::BeginPlay()
{
	Super::BeginPlay();

	DeliveryCollision->OnComponentBeginOverlap.AddDynamic(this, &ADeliveryStation::OnDeliveryOverlap);
	DeliveryCollision->OnComponentEndOverlap.AddDynamic(this, &ADeliveryStation::OnDeliveryEndOverlap);
	
}

// Called every frame
void ADeliveryStation::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ADeliveryStation::Deliver(ADish* Dish)
{
	if (AStirCrazyGameModeBase* Mode = Cast<AStirCrazyGameModeBase>(UGameplayStatics::GetGameMode(GetWorld())))
	{
		Mode->DeliverFood(Dish, Dish->LastHolder);
	}
}

void ADeliveryStation::OnDeliveryOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (ADish* Dish = Cast<ADish>(OtherActor))
	{
		Dish->OverlappedStation = this;
	}
}

void ADeliveryStation::OnDeliveryEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (ADish* Dish = Cast<ADish>(OtherActor))
	{
		Dish->OverlappedStation = nullptr;
	}
}

