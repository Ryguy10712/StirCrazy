// Fill out your copyright notice in the Description page of Project Settings.


#include "VrPawn.h"
#include "Stove.h"

#include "SCGameStateBase.h"
#include "WorldInteractable.h"
#include "Kismet/GameplayStatics.h"


// Sets default values
AVrPawn::AVrPawn(const FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer.SetDefaultSubobjectClass<UVRMovementComp>(ACharacter::CharacterMovementComponentName))
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	///Setup Root and Camera
	
	/**
	 * I like to have a VRRoot where I attach my headset and controllers to. That way i can adjust the VRRoot
	 * whilst knowing that I will never misalign the controllers from the HMD (Head mounted display)
	 * 
	 * The VRRoot is nothing but an empty object that has a location and that other objects can attach to. 
	 * The VRRoot is attached to the root component
	*/

	VRRoot = CreateDefaultSubobject<USceneComponent>(TEXT("VR Root"));
	VRRoot->SetupAttachment(RootComponent);
	VRRoot->SetRelativeLocation({0, 0, -GetCapsuleComponent()->GetUnscaledCapsuleHalfHeight()});
	VRCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("VR Camera"));
	VRCamera->SetupAttachment(VRRoot);
	VRCamera->bLockToHmd = true;

	///Setup Controllers

	LeftController = CreateDefaultSubobject<UMotionControllerComponent>("MC_Left");
	LeftController->MotionSource = FName("Left");
	LeftController->SetupAttachment(VRRoot); //Here i attach the controller to my VRRoot
	LeftController->SetCollisionProfileName(FName("MotionController"));
	LeftHandState.GripState = UnGripped;
	SM_Left = CreateDefaultSubobject<UStaticMeshComponent>("LeftHandMesh");
	SM_Left->SetupAttachment(LeftController);

	RightController = CreateDefaultSubobject<UMotionControllerComponent>("MC_Right");
	RightController->MotionSource = FName("Right");
	RightController->SetupAttachment(VRRoot);
	RightController->SetCollisionProfileName(FName("MotionController"));
	RightHandState.GripState = UnGripped;

	SM_Right = CreateDefaultSubobject<UStaticMeshComponent>("RightHandMesh");
	SM_Right->SetupAttachment(RightController);


	//default value for flatscreen development. I can override this in blueprint
	pancakeDev = false;
	


	if (UVRMovementComp* Candidate = Cast<UVRMovementComp>(GetMovementComponent()))
	{
		VRMovement = Candidate;
		VRMovement->SetUpdatedComponent(RootComponent);
	}

}

// Called when the game starts or when spawned
void AVrPawn::BeginPlay()
{
	Super::BeginPlay();

	#ifdef PLATFORM_WINDOWS

#ifndef PLATFORM_ANDROID

	auto result = discord::Core::Create(1142958645546602637, DiscordCreateFlags_Default, &Core);
	discord::Activity activity{};
	activity.SetState("Testing");
	activity.SetDetails("Currently Banging Head");
	Core->ActivityManager().UpdateActivity(activity, [](discord::Result result) {});

	#endif
#endif
	
		

	//Init In-world mapping context. This is needed for the enhanced input component to work. Enhanced input is a ue5 feature IIRC
	if (APlayerController* PlayerController = Cast<APlayerController>(GetController()))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(InWorldContext, 0);
		}
	}

	//For flatscreen testing i like to offset my controllers in front of me so i can see them.

	#ifdef UE_EDITOR

	if (pancakeDev)
	{
		VRCamera->bLockToHmd = false;
		VRRoot->SetRelativeLocation({ 0, 0, 90.17f });
		VRCamera->bUsePawnControlRotation = true;
		LeftController->SetRelativeLocation(FVector(200.f, -30.f, -25.f));
		LeftController->AttachToComponent(VRCamera, FAttachmentTransformRules::KeepWorldTransform);
		RightController->SetRelativeLocation(FVector(200.f, 30.f, -25.f));
		RightController->AttachToComponent(VRCamera, FAttachmentTransformRules::KeepWorldTransform);

	}

	#endif

}

void AVrPawn::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

#ifdef PLATFORM_WINDOWS
#ifndef PLATFORM_ANDROID
	
	//End discord activity
	Core->ActivityManager().ClearActivity([](discord::Result const Result)
	{
		UE_LOG(LogTemp, Warning, TEXT("Clearing activity"));
		if (Result != discord::Result::Ok)
		{
			UE_LOG(LogTemp, Warning, TEXT("Failed to clear activity"));
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("Cleared activity"));
		}
	});

	#endif
#endif
	
}

// Called every frame
void AVrPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	#ifdef PLATFORM_WINDOWS
#ifndef PLATFORM_ANDROID

	Core->RunCallbacks();
#endif
	
	#endif
}

// Called to bind functionality to input
void AVrPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent); // do not remove, calls the parent function


	//Here I setup input
	UEnhancedInputComponent* Input = Cast<UEnhancedInputComponent>(PlayerInputComponent);
	Input->BindAction(LGraspAction, ETriggerEvent::Started, this, &AVrPawn::Grasp, false);
	Input->BindAction(LUngraspAction, ETriggerEvent::Triggered, this, &AVrPawn::UnGrasp, false);
	Input->BindAction(RGraspAction, ETriggerEvent::Started, this, &AVrPawn::Grasp, true);
	Input->BindAction(RUngraspAction, ETriggerEvent::Triggered, this, &AVrPawn::UnGrasp, true);

	Input->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AVrPawn::Move);
	Input->BindAction(TurnAction, ETriggerEvent::Triggered, this, &AVrPawn::Turn);
	
	///*DEV ONLY*/// //only allow the mouse to influence input if im testing in flatscreen. 

	#ifdef UE_EDITOR

	if(pancakeDev)
	{
		Input->BindAction(LookAction, ETriggerEvent::Triggered, this, &AVrPawn::Look);
	}

	#endif
	
}

#pragma region Input

//This is the function I use when testing in flatscreen. I need to rotate the camera somehow. 
void AVrPawn::Look(const FInputActionInstance& Instance)
{
	//Input is an FVector2D
	FVector2D Val = Instance.GetValue().Get<FVector2D>();

	if (Controller != nullptr)
	{
		AddControllerYawInput(Val.X);
		AddControllerPitchInput(-Val.Y);
	}
	
}


void AVrPawn::Move(const FInputActionInstance& Instance)
{
	AGameStateBase* State = UGameplayStatics::GetGameState(GetWorld());
	if (!State)
	{
		return;
	}
	ASCGameStateBase* SCState = Cast<ASCGameStateBase>(State);

	if (Controller != nullptr && !SCState->allMovementDisabled)
	{
		float X = Instance.GetValue().Get<FVector2D>().X;
		float Y = Instance.GetValue().Get<FVector2D>().Y;
		
		#ifdef WITH_EDITOR

		if (!pancakeDev)
		{
			//Move in relation to the yaw orientation of the HMD
			AddMovementInput(VRCamera->GetForwardVector(), X);
			AddMovementInput(VRCamera->GetRightVector(), Y);
		}
		
		else
		{
			//actor rotation will always match camera rotation when not VR
			AddMovementInput(GetActorForwardVector(), X);
			AddMovementInput(GetActorRightVector(), Y);
		}

		#else

		AddMovementInput(GetActorForwardVector(), X);
		AddMovementInput(GetActorRightVector(), Y);

		#endif

		
		
	}
}

void AVrPawn::Turn(const FInputActionInstance& Instance)
{
	if (Controller != nullptr)
	{
		float Value = Instance.GetValue().Get<float>();
		AddControllerYawInput(Value);
	}
}

//Calls pickup function if theres a grabableactor within range
void AVrPawn::Grasp(const FInputActionInstance &Instance, bool isRightHand)
{
	UMotionControllerComponent* Hand = isRightHand ? RightController: LeftController;
	UStaticMeshComponent* SM = isRightHand ? SM_Right : SM_Left;
	TArray<AActor*> OverlappedGrabActors;
	SM->GetOverlappingActors(OverlappedGrabActors, TSubclassOf<AGrabableActor>());
	
	if (OverlappedGrabActors.Num())
	{
		//determine closest actor
		float ClosestDistance = -1.f;
		AGrabableActor* closestActor = Cast<AGrabableActor>(OverlappedGrabActors[0]);
		for (AActor* Actor : OverlappedGrabActors)
		{
			if (AGrabableActor* GrabActor = Cast<AGrabableActor>(Actor); GrabActor != nullptr)
			{
				if (const float CandidateDistance = FVector::Distance(Hand->GetComponentLocation(), GrabActor->GetActorLocation()); ClosestDistance == -1.f || CandidateDistance < ClosestDistance)
				{
					ClosestDistance = CandidateDistance;
					closestActor = GrabActor;
				}
			}
		}

		//At this point cloest actor is determined
		//Pickup(closestActor, isRightHand); //old
		if (closestActor != nullptr)
		{
			RequestItemPickup(closestActor, isRightHand);
			return;
		}
	}
	//Not an item, maybe an interactable
	
	TArray<AActor*> OverlappedInteractables;
	SM->GetOverlappingActors(OverlappedInteractables, TSubclassOf<AWorldInteractable>());

	if(OverlappedInteractables.Num())
	{
		//Determine the closest one
		float ClosestDistance = -1.f;
		AWorldInteractable* ClosestActor = Cast<AWorldInteractable>(OverlappedGrabActors[0]);
		for(AActor* Actor : OverlappedInteractables)
		{
			if(AWorldInteractable* Interactable = Cast<AWorldInteractable>(Actor); Interactable != nullptr)
			{
				if(const float CandidateDistance = FVector::Distance(Hand->GetComponentLocation(), Interactable->GetActorLocation()); ClosestDistance == -1.f || CandidateDistance < ClosestDistance)
				{
					ClosestDistance = CandidateDistance;
					ClosestActor = Interactable;
				}
			}
		}

		if(ClosestActor != nullptr)
		{
			//Determine if ownership is needed for this action
			if(ClosestActor->bInteractNeedsOwnership)
			{
				RequestInteractableOwnership(ClosestActor, isRightHand);
			}
			else
			{
				ClosestActor->OnInteract(this, isRightHand);
			}
		}
			
	}
	

}

void AVrPawn::UnGrasp(const FInputActionInstance& Instance, bool isRightHand)
{
	if (isRightHand && RightHandState.HeldItem != nullptr)
	{
		RightHandState.HeldItem->Drop();
	}
	else if (!isRightHand && LeftHandState.HeldItem != nullptr)
	{
		LeftHandState.HeldItem->Drop();
	}
}

#pragma endregion

#pragma region Pickup/Drop

void AVrPawn::RequestItemPickup(AGrabableActor* Item, bool isRightHand)
{
	if (!Item->CurrentHolder)
	{
		Item->PredictPickup(this, isRightHand);
		ServerRequestItemPickup(Item, isRightHand);
	}
}

void AVrPawn::ServerRequestItemPickup_Implementation(AGrabableActor* Item, bool isRightHand)
{
	Item->SetOwner(this);
	Item->ServerHandlePickup(this, isRightHand);
}

#pragma endregion

#pragma region Interactables

void AVrPawn::RequestInteractableOwnership_Implementation(AWorldInteractable* Interactable, bool isRightHand)
{
	Interactable->SetOwner(this);
	Interactable->OnInteract(this, isRightHand);
}

#pragma endregion

#pragma region Replication

void AVrPawn::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AVrPawn, LeftHandState);
	DOREPLIFETIME(AVrPawn, RightHandState);
}


#pragma endregion

