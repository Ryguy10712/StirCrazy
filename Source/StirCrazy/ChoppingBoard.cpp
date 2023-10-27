// Fill out your copyright notice in the Description page of Project Settings.


#include "ChoppingBoard.h"

#include "ChopableFood.h"

// Sets default values
AChoppingBoard::AChoppingBoard()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Root = CreateDefaultSubobject<USceneComponent>("Root");
	SetRootComponent(Root);
	
	BoardMesh = CreateDefaultSubobject<UStaticMeshComponent>("BoardMesh");
	BoardMesh->SetupAttachment(Root);
	
	SlicePile0 = CreateDefaultSubobject<USceneComponent>("SlicePile0");
	SlicePile1 = CreateDefaultSubobject<USceneComponent>("SlicePile1");

	SlicePile0->SetupAttachment(Root);
	SlicePile1->SetupAttachment(Root);

	FoodCollision = CreateDefaultSubobject<UBoxComponent>("FoodCollision");
	FoodCollision->SetupAttachment(Root);

}

// Called when the game starts or when spawned
void AChoppingBoard::BeginPlay()
{
	Super::BeginPlay();

	//Overlaps
	FoodCollision->OnComponentBeginOverlap.AddDynamic(this, &AChoppingBoard::OnBoardOverlap);
	
}

// Called every frame
void AChoppingBoard::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

USceneComponent* AChoppingBoard::GetAvailableSlicePile()
{
	FHitResult HitResult;
	FVector Loc0 = SlicePile0->GetComponentLocation();
	FVector Loc1 = SlicePile1->GetComponentLocation();

	if (!GetWorld()->LineTraceSingleByChannel(HitResult, Loc0, Loc0, ECC_Visibility))
	{
		//Did not hit anything, pile is open
		GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Purple, "Pile0 free");
		return SlicePile0;
	}
	else if(!GetWorld()->LineTraceSingleByChannel(HitResult, Loc1, Loc1, ECC_Visibility))
	{
		GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Purple, "Pile0 free");
		return SlicePile1;
	}

	//Both hit something, revert to original pile
	return SlicePile0;
}

void AChoppingBoard::OnBoardOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (AChopableFood* Food = Cast<AChopableFood>(OtherActor))
	{
		Food->CurrentChoppingBoard = this;
	}
}

void AChoppingBoard::OnBoardEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (AChopableFood* Food = Cast<AChopableFood>(OtherActor))
	{
		Food->CurrentChoppingBoard = this;
	}
}

