#include "ItemSpawner.h"

#include "VrPawn.h"

AItemSpawner::AItemSpawner()
{
	SpawnerMesh = CreateDefaultSubobject<UStaticMeshComponent>("Spawn Mesh");
	SpawnLocation = CreateDefaultSubobject<USceneComponent>("Spawn Location");
	SpawnLocation->SetupAttachment(DefaultSceneRoot);
	bAutoSpawn = false;
	bSpawnByGrasp = true;
	SpawnInterval = 5.0f;
	MaxSpawns = 0;
	CurrentSpawns = 0;

	bInteractNeedsOwnership = true;
}

void AItemSpawner::OnInteract_Implementation(AVrPawn* Player, bool bIsRightHand)
{
	if(HasAuthority())
	{
		SpawnItem(Player, bIsRightHand);
	}
}

void AItemSpawner::SpawnItem_Implementation(AVrPawn* Player, bool bIsRightHand)
{
	if(CurrentSpawns >= MaxSpawns && MaxSpawns != 0)
	{
		GetWorld()->GetTimerManager().ClearTimer(SpawnHandle);
		SpawnHandle.Invalidate();
		return;
	}
	
	if(Player && SpawnClass->IsChildOf(AGrabableActor::StaticClass()))
	{
		AGrabableActor* Item = GetWorld()->SpawnActorDeferred<AGrabableActor>(SpawnClass, FTransform::Identity, this, Player, ESpawnActorCollisionHandlingMethod::AlwaysSpawn);
		Item->ServerHandlePickup(Player, bIsRightHand);
		Item->FinishSpawning(FTransform::Identity);
		SetOwner(nullptr);
	}
	else
	{
		GetWorld()->SpawnActor<AGrabableActor>(SpawnClass, SpawnLocation->GetComponentTransform());
	}
	
	CurrentSpawns ++;
	
}

void AItemSpawner::BeginPlay()
{
	if(HasAuthority() && bAutoSpawn)
	{
		FTimerDelegate SpawnDelegate;
		SpawnDelegate.BindLambda([this]()
		{
			SpawnItem(nullptr, false);
		});
		GetWorld()->GetTimerManager().SetTimer(SpawnHandle, SpawnDelegate, SpawnInterval, true);
	}
}








