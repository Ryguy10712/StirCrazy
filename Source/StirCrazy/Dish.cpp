// Fill out your copyright notice in the Description page of Project Settings.


#include "Dish.h"
#include "DeliveryStation.h"
#include "Kismet/GameplayStatics.h"
#include "VrPawn.h"


ADish::ADish()
{
	bReplicates = true;

	FoodCollision = CreateDefaultSubobject<UBoxComponent>("FoodCollision");
	FoodCollision->SetupAttachment(RootComponent);
}

void ADish::BeginPlay()
{
	Super::BeginPlay();

	FoodCollision->OnComponentBeginOverlap.AddDynamic(this, &ADish::OnFoodBeginOverlap);
	FoodCollision->OnComponentEndOverlap.AddDynamic(this, &ADish::OnFoodEndOverlap);

}

void ADish::Drop()
{
	if (OverlappedStation)
	{
		DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
		isSimulatingPhysics = true;
		OnRep_isSimPhysics();
		CurrentHolder = nullptr;
		DeliverDishServer(OverlappedStation);
		SetOwner(nullptr);
	}
	else
	{
		Super::Drop();
	}
}

void ADish::DeliverDishServer_Implementation(ADeliveryStation* Station)
{
	DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
	isSimulatingPhysics = true;

	switch (GetHeldHand())
	{
		case 0:
			CurrentHolder->LeftHandState.HeldItem = nullptr;
			break;
		case 1:
			CurrentHolder->RightHandState.HeldItem = nullptr;
			break;
	}
	CurrentHolder = nullptr;
	SetOwner(nullptr);

	Station->Deliver(this);
}

void ADish::PlateFood_Implementation(AFood* Food)
{
	Contents.Add(Food);

	Food->CurrentHolder = nullptr;
	Food->isSimulatingPhysics = false;
	OnRep_isSimPhysics();
	Food->Mesh->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	Food->CurrentDish = this;

	for (UDA_Recipes* Recipe : IntendedRecipes)
	{
		const bool isValidRecipe = ValidateContentsWithRecipe(Recipe);
		if (isValidRecipe)
		{
			TArray<AFood*> ContentsCopy = Contents;
			for (AFood* Content : ContentsCopy)
			{
				Contents.Remove(Content);
				Content->Destroy();
			}


			FActorSpawnParameters Params;
			Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
			FVector SpawnLoc = GetActorLocation() + Recipe->PlatedFood->GetDefaultObject<AFood>()->PlatedOffset;

			FTransform Transform;
			Transform.SetLocation(GetActorLocation() + SpawnLoc);


			AFood* SpawnedFood = GetWorld()->SpawnActorDeferred<AFood>(Recipe->PlatedFood, FTransform(GetActorRotation(), SpawnLoc), this, Food->LastHolder, ESpawnActorCollisionHandlingMethod::AlwaysSpawn);

			SpawnedFood->isSimulatingPhysics = false;
			SpawnedFood->OnRep_isSimPhysics();
			SpawnedFood->ForceNetUpdate();

			UGameplayStatics::FinishSpawningActor(SpawnedFood, FTransform(GetActorRotation(), SpawnLoc));
			Contents.Add(SpawnedFood);
			SpawnedFood->AttachToActor(this, FAttachmentTransformRules::KeepWorldTransform);

			//Used to attach on client
			//RespondPlateFood(SpawnedFood);

		}
	}
}

bool ADish::ValidateContentsWithRecipe(UDA_Recipes* Recipe)
{
	if (Contents.Num() != Recipe->Ingredients.Num())
	{
		return false;
	}

	for (AFood* Content : Contents)
	{
		bool isContentValid = Recipe->Ingredients.ContainsByPredicate([Content](FIngredientInfo Info) {
			return Info.FoodClass == Content->GetClass()
				&& Info.CookMethod == Content->CookMethod
				&& Info.CookState == Content->CookedState;
			});

		if (!isContentValid)
		{
			return false;
		}
	}

	return true;
}
void ADish::OnFoodBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{

	if (AFood* Food = Cast<AFood>(OtherActor))
	{
		Food->OverlappedDish = this;
	}
}

void ADish::OnFoodEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (AFood* Food = Cast<AFood>(OtherActor))
	{
		Food->OverlappedDish = nullptr;
	}
}

void ADish::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ADish, Contents);
}
