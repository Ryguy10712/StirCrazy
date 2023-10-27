// Fill out your copyright notice in the Description page of Project Settings.


#include "Stove.h"

// Sets default values
AStove::AStove()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;

	//Default Scene Root
	DefaultSceneRoot = CreateDefaultSubobject<USceneComponent>("DefaultSceneRoot");
	SetRootComponent(DefaultSceneRoot);

	//Mesh
	StoveMesh = CreateDefaultSubobject<USkeletalMeshComponent>("StoveMesh");
	StoveMesh->SetupAttachment(RootComponent);

	//DishCollider
	DishCollision = CreateDefaultSubobject<UBoxComponent>("DishCollision");
	DishCollision->SetupAttachment(StoveMesh);

	HandleGripCollision = CreateDefaultSubobject<UBoxComponent>("DevBox");
	HandleGripCollision->SetupAttachment(StoveMesh);

	bDoorOpen = false;

	//Init overlaps

}

// Called when the game starts or when spawned
void AStove::BeginPlay()
{
	Super::BeginPlay();

	DishCollision->OnComponentBeginOverlap.AddDynamic(this, &AStove::OnDishOverlap);

	DishCollision->OnComponentEndOverlap.AddDynamic(this, &AStove::OnDishEndOverlap);
	
}

// Called every frame
void AStove::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AStove::OnDishOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (AGrabableActor* Item = Cast<AGrabableActor>(OtherActor))
	{
		if (AFood* Food = Cast<AFood>(Item))
		{
			Food->OverlappedCooker = this;
		}
	}
}

void AStove::OnDishEndOverlap(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{

	if (AFood* Food = Cast<AFood>(OtherActor))
	{
		Food->OverlappedCooker = nullptr;
	}
}

void AStove::MoveInsideOven(AGrabableActor* Item)
{
	if (HasAuthority())
	{
		GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Blue, "ServerMove");
	}
	Item->DisableComponentsSimulatePhysics();
	Item->AttachToComponent(StoveMesh, FAttachmentTransformRules::SnapToTargetNotIncludingScale, FName("S_Tray"));
	Item->SetOwner(this);
	S_MoveInsideOven(Item);
}

void AStove::S_MoveInsideOven_Implementation(AGrabableActor* Item)
{
	Item->DisableComponentsSimulatePhysics();
	Item->AttachToComponent(StoveMesh, FAttachmentTransformRules::SnapToTargetNotIncludingScale, FName("S_Tray"));
	ItemInOven = Item;
	Item->SetOwner(this);
	if (AFood* Food = Cast<AFood>(Item))
	{
		if (!Food->bIsCookable) return;

		StartCookingItem(Food);
	}
}

void AStove::StartCookingItem_Implementation(AFood* Food)
{
	ItemInOven = Food;
	Food->CurrentCooker = this;

	GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Orange, "runnin");

	Food->WellDelegate.BindUFunction(this, FName("UpdateItemCookState"), ECookedState::Well);
	Food->BurntDelegate.BindUFunction(this, FName("UpdateItemCookState"), Burnt);

	GetWorldTimerManager().SetTimer(Food->WellHandle, Food->WellDelegate, Food->CookTimeSeconds, false);
	GetWorldTimerManager().SetTimer(Food->BurntHandle, Food->BurntDelegate, Food->BurnTimeSeconds, false);
}

void AStove::RemoveFromOven_Implementation(AFood* Food)
{
	ItemInOven = nullptr;
	GetWorldTimerManager().ClearTimer(Food->WellHandle);
	GetWorldTimerManager().ClearTimer(Food->BurntHandle);
}

void AStove::UpdateItemCookState(TEnumAsByte<ECookedState> State)
{
	if(!HasAuthority()) return;

	GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Red, "UpdateState");
	AFood* Food = CastChecked<AFood>(ItemInOven);
	Food->CookedState = State;
}


void AStove::OnRepItemInOven()
{
	
}

void AStove::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AStove, ItemInOven);
}