// Fill out your copyright notice in the Description page of Project Settings.


#include "ChopableFood.h"
#include "ChoppingBoard.h"
#include "Kismet/GameplayStatics.h"

AChopableFood::AChopableFood()
{
	//Setup
	MaxChops = 0;
	CurrentChops = 0;
	bCanBeSliced = true;
	bReplicates = true;

	//Setup Static Mesh

	//Setup procedural mesh
	ProcMesh = CreateDefaultSubobject<UProceduralMeshComponent>("Procedural Mesh");
	SetRootComponent(ProcMesh);
	ProcMesh->SetSimulatePhysics(true);
	ProcMesh->SetCollisionObjectType(ECC_PhysicsBody);
	ProcMesh->bUseComplexAsSimpleCollision = false;

	Mesh->SetupAttachment(RootComponent);
	Mesh->SetHiddenInGame(true);

	Slices.Add(ProcMesh);

	SlicePileLocation = {0, 0, 0};
}

void AChopableFood::PredictSlice(FVector Loc, FVector Normal, UProceduralMeshComponent* InComp)
{
	bInPredictedState = true;

	//Return if we cannot slice
	if (CurrentChops >= MaxChops || !bCanBeSliced || !CurrentChoppingBoard || bInPredictedState)
	{
		return;
	}

	//if predictions are unreliable, dont bother predicting and go straight towards server slice
	if(!bPredictionsReliable)
	{
		ServerSlice(Loc, Normal, InComp, CurrentChops);
		return;
	}

	bInPredictedState = true;
	CurrentChops++;

	UProceduralMeshComponent* OtherHalf;
	UKismetProceduralMeshLibrary::SliceProceduralMesh(InComp, Loc, Normal, true, OtherHalf, EProcMeshSliceCapOption::CreateNewSectionForCap, CapMaterial);

	Slices.Add(OtherHalf);
	OtherHalf->SetSimulatePhysics(true);
	//Set timer to reset chop function
	bCanBeSliced = false;

	ServerSlice(Loc, Normal, InComp, CurrentChops);
}

void AChopableFood::ServerSlice_Implementation(FVector Loc, FVector Normal, UProceduralMeshComponent* InComp, int SliceCount)
{
	UE_LOG(LogTemp, Warning, TEXT("ServerSlice"));
	UProceduralMeshComponent* OtherHalf;
	UKismetProceduralMeshLibrary::SliceProceduralMesh(InComp, Loc, Normal, true, OtherHalf, EProcMeshSliceCapOption::CreateNewSectionForCap, CapMaterial);
	OtherHalf->SetSimulatePhysics(true);
	Slices.Add(OtherHalf);

	RespondSliceCorrect();
}

void AChopableFood::RespondSliceCorrect_Implementation()
{
	bInPredictedState = false;
	bPredictionsReliable =  true;
}

void AChopableFood::RespondSliceIncorrect_Implementation(const TArray<UProceduralMeshComponent*>& CorrectedSlices)
{
	for (int i = 0; i < CorrectedSlices.Num(); i++)
	{
		if(CorrectedSlices.IsValidIndex(i))
		{
			CorrectedSlices[i]->DestroyComponent();
		}
	}

	for(int i = 0; i < CorrectedSlices.Num(); i++)
	{
		//Add these components to actor and array
		if(CorrectedSlices.IsValidIndex(i))
		{
			Slices.Add(CorrectedSlices[i]);
			CorrectedSlices[i]->RegisterComponent();
			AddInstanceComponent(CorrectedSlices[i]);
		}
	}
	
}



void AChopableFood::BeginPlay()
{
	Mesh->SetHiddenInGame(true);
	UKismetProceduralMeshLibrary::CopyProceduralMeshFromStaticMeshComponent(Mesh, 0, ProcMesh, false);
}

void AChopableFood::Slice(FVector Loc, FVector Normal, UProceduralMeshComponent* InComp)
{
	//Return if we cannot slice
	if (CurrentChops >= MaxChops || !bCanBeSliced || !CurrentChoppingBoard)
	{
		return;
	}

	CurrentChops ++;

	UProceduralMeshComponent* OtherHalf;
	UKismetProceduralMeshLibrary::SliceProceduralMesh(InComp, Loc, Normal, true, OtherHalf, EProcMeshSliceCapOption::CreateNewSectionForCap, CapMaterial);

	Slices.Add(OtherHalf);
	OtherHalf->SetSimulatePhysics(true);
	//Set timer to reset chop function
	bCanBeSliced = false;
	GetWorld()->GetTimerManager().SetTimer(ChopTimerGate, this, &AChopableFood::ChopResetFunction, 0.5, false);
	
	FBoxSphereBounds3d OtherHalfBounds = OtherHalf->GetLocalBounds();
	
	/*InComp is the old slice/component, OtherHalf is the resultant slice*/

	FVector const InCompMax = InComp->GetLocalBounds().GetBox().Max;
	FVector const InCompMin = InComp->GetLocalBounds().GetBox().Min;
	FVector const OtherHalfMax = OtherHalfBounds.GetBox().Max;
	FVector const OtherHalfMin = OtherHalfBounds.GetBox().Min;

	float const InCompExtent = (InCompMax - InCompMin).Size();
	float const OtherHalfExtent = (OtherHalfMax - OtherHalfMin).Size();

	//Determine what the smaller slice is
	UProceduralMeshComponent* SmallerSlice = nullptr;
	float SmallerSliceExtent = 0;

	if (InCompExtent < OtherHalfExtent)
	{
		SmallerSlice = InComp;
		SmallerSliceExtent = InCompExtent;
	}
	else if (OtherHalfExtent < InCompExtent)
	{
		SmallerSlice = OtherHalf;
		SmallerSliceExtent = OtherHalfExtent;
	}
	else
	{
		//They are equal, default to the resultant mesh
		SmallerSlice = OtherHalf;
		SmallerSliceExtent = OtherHalfExtent;
	}
	
	//If thinly sliced, replace the thin slice with a thin slice pile and move it to the slice pile location
	if (SmallerSliceExtent <= MaxThinSliceSize)
	{
		SlicePileLocation = CurrentChoppingBoard->GetAvailableSlicePile()->GetComponentLocation();
		
		//Remove the smaller slice from the array
		Slices.Remove(SmallerSlice);
		SmallerSlice->DestroyComponent();

		if(!CurrentSlicePile)
		{
			//Spawn thin slice pile
			CurrentSlicePile = GetWorld()->SpawnActorDeferred<AThinSlicePile>(ThinSlicePileClass, FTransform(SlicePileLocation), this, nullptr, ESpawnActorCollisionHandlingMethod::AlwaysSpawn);
			CurrentSlicePile->SetActorEnableCollision(false);
			UGameplayStatics::FinishSpawningActor(CurrentSlicePile, FTransform(SlicePileLocation));
		}
		else
		{
			//Add to existing thin slice pile
			CurrentSlicePile->IncrementSlices(1);
		}


	}
}

void AChopableFood::ChopResetFunction()
{
	bCanBeSliced = true;
}