// Fill out your copyright notice in the Description page of Project Settings.


#include "GrabableActor.h"
#include "VrPawn.h"

// Sets default values
AGrabableActor::AGrabableActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	bReplicates = true;

	NetLocked = false;
	InHandOffset = {0.f, 0.f, 0.f};

	bIsSkeletal = false;

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>("Mesh");
	SetRootComponent(Mesh);
	Mesh->SetSimulatePhysics(true);
	isSimulatingPhysics = true;

	//Setup Grab Volume
	GrabVolume = CreateDefaultSubobject<UBoxComponent>(TEXT("Grab Volume"));
	GrabVolume->SetupAttachment(Mesh);

	bInRightHand = false;
}

// Called when the game starts or when spawned
void AGrabableActor::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void AGrabableActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

#pragma region NewPickup/Drop

void AGrabableActor::PredictPickup(AVrPawn* Player, bool isRightHand)
{
	 //Predict the outcome of the pickup
	 SetOwner(Player);
	 NetLocked = true;
	 isSimulatingPhysics = false;
	 OnRep_isSimPhysics();
	 UMotionControllerComponent* MC = isRightHand ? Player->RightController : Player->LeftController;
	 AttachToComponent(MC, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
	 SetActorRelativeLocation(InHandOffset);
}

void AGrabableActor::ServerHandlePickup_Implementation(AVrPawn* Player, bool isRightHand)
{
	if (CurrentHolder == nullptr && CurrentHolder != Player) //make sure a pickup request didnt squeeze by before this one
	{
		GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Blue, "ServerPickValid");
		LastHolder = Player;
		SetOwner(Player);
		isSimulatingPhysics = false;
		OnRep_isSimPhysics();

		if (isRightHand)
		{
			AttachToComponent(Player->RightController, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
			SetActorRelativeLocation(InHandOffset);
			Player->RightHandState.HeldItem = this;
			bInRightHand = true;
			Player->ForceNetUpdate();
			CurrentHolder = Player;
		}
		else
		{
			AttachToComponent(Player->LeftController, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
			SetActorRelativeLocation(InHandOffset);
			Player->LeftHandState.HeldItem = this;
			bInRightHand = false;
			Player->ForceNetUpdate();
			CurrentHolder = Player;
		}

		ClientRespondPickup(true);
	}
	else
	{
		ClientRespondPickup(false);
	}
}

void AGrabableActor::ClientRespondPickup_Implementation(bool wasPredictionCorrect)
{
	if (wasPredictionCorrect)
	{
		NetLocked = false;
	}
	else
	{
		SetOwner(nullptr);
		isSimulatingPhysics = true;
		OnRep_isSimPhysics();
		DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
	}
}

void AGrabableActor::Drop()
{
	DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
	isSimulatingPhysics = true;
	OnRep_isSimPhysics();
	CurrentHolder = nullptr;
	ServerDrop();
	SetOwner(nullptr);
}

void AGrabableActor::ServerDrop_Implementation()
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
}

#pragma endregion

void AGrabableActor::Physicalize_Implementation(bool enable)
{
	Mesh->SetSimulatePhysics(enable);
}

void AGrabableActor::OnRep_isSimPhysics()
{
	Mesh->SetSimulatePhysics(isSimulatingPhysics);
}

void AGrabableActor::OnRep_CurrentHolder()
{
	if(!CurrentHolder)
	{
		DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
		return;
	}
	
	if (bInRightHand)
	{
		isSimulatingPhysics = false;
		OnRep_isSimPhysics();
		AttachToComponent(CurrentHolder->RightController, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
	}
	else
	{
		isSimulatingPhysics = false;
		OnRep_isSimPhysics();
		AttachToComponent(CurrentHolder->LeftController, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
	}
	
}

int AGrabableActor::GetHeldHand()
{
	if (!CurrentHolder)
	{
		return -1;
	}
	else
	{
		if (CurrentHolder->LeftHandState.HeldItem == this)
		{
			return 0;
		}
		else if (CurrentHolder->RightHandState.HeldItem == this)
		{
			return 1;
		}
		else
		{
			return -1;
		}
	}
}

void AGrabableActor::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AGrabableActor, isSimulatingPhysics);
	DOREPLIFETIME(AGrabableActor, bInRightHand)
	DOREPLIFETIME(AGrabableActor, CurrentHolder);
	DOREPLIFETIME(AGrabableActor, LastHolder);
	
}


