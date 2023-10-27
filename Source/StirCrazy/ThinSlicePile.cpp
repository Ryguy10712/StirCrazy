// Fill out your copyright notice in the Description page of Project Settings.


#include "ThinSlicePile.h"

AThinSlicePile::AThinSlicePile()
{
	PrimaryActorTick.bCanEverTick = false;

	DefaultSceneRoot = CreateDefaultSubobject<USceneComponent>("DefaultSceneRoot");
	SetRootComponent(DefaultSceneRoot);

	Mesh->DestroyComponent();

	CurrentSlices = 0;
	MaxSlices = 5;
}

void AThinSlicePile::BeginPlay()
{
	Super::BeginPlay();
	
	SliceMeshes.Empty();

	//Create first slice
	UStaticMeshComponent* StaticMeshComponent = NewObject<UStaticMeshComponent>(this);
	StaticMeshComponent->SetupAttachment(RootComponent);
	StaticMeshComponent->SetStaticMesh(SliceMesh);
	StaticMeshComponent->SetRelativeTransform(SliceTransforms[0]);
	StaticMeshComponent->RegisterComponent();
	AddInstanceComponent(StaticMeshComponent);
	SliceMeshes.Add(StaticMeshComponent);
	CurrentSlices = 1;
}

void AThinSlicePile::IncrementSlices(int Amount)
{
	if (Amount == 0)
	{
		return;
	}

	else if (CurrentSlices + Amount > MaxSlices || CurrentSlices + Amount < 0)
	{
		return;
	}

	const int StartingSliceAmount = CurrentSlices;
	CurrentSlices += Amount;
	
	//Add or remove meshes to the stack to match the current slices

	if (Amount > 0)
	{
		for (int i = 0; i < Amount; i++)
		{
			UStaticMeshComponent* StaticMeshComponent = NewObject<UStaticMeshComponent>(this);
			StaticMeshComponent->SetupAttachment(RootComponent);
			StaticMeshComponent->SetStaticMesh(SliceMesh);

			if (SliceTransforms.IsValidIndex(StartingSliceAmount + i))
			{
				StaticMeshComponent->SetRelativeTransform(SliceTransforms[StartingSliceAmount + i]);
			}

			StaticMeshComponent->RegisterComponent();
			AddInstanceComponent(StaticMeshComponent);
			SliceMeshes.Add(StaticMeshComponent);
		}
	}
	else if (Amount < 0)
	{
		for (int i = 0; i < -Amount; i++)
		{
			if (SliceMeshes.IsValidIndex(StartingSliceAmount - 1 + i))
			{
				SliceMeshes[StartingSliceAmount - 1 + i]->DestroyComponent();
				SliceMeshes.RemoveAt(StartingSliceAmount - 1 + i);
			}
		}
	}
}

void AThinSlicePile::SetSlices(int Amount)
{
	//Determine wether we need to add or remove slices

	if (Amount > CurrentSlices)
	{
		IncrementSlices(Amount - CurrentSlices);
	}
	else if (Amount < CurrentSlices)
	{
		IncrementSlices(Amount - CurrentSlices);
	}
}

void AThinSlicePile::Break()
{
	TArray<UStaticMeshComponent*> SliceMeshesCopy = SliceMeshes;
}