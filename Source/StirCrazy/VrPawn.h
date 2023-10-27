// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
//VR Components
#include "Camera/CameraComponent.h"
//Character
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "VRMovementComp.h"

#include "Net/UnrealNetwork.h"
#include "MotionControllerComponent.h"
//Input
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "GrabableActor.h"
#include "GameFramework/PlayerController.h"
#include "WorldInteractable.h"

#ifdef PLATFORM_WINDOWS

#include "Discord/discord.h"

#endif

#include "VrPawn.generated.h"

DECLARE_MULTICAST_DELEGATE_OneParam(FOnGraspDelegate, bool);

DECLARE_MULTICAST_DELEGATE_OneParam(FOnUngraspDelegate, bool);

UENUM(BlueprintType)
enum EGripState
{
	UnGripped,
	Gripped
};

USTRUCT(BlueprintType)
struct FHandState
{
	GENERATED_BODY()

	UPROPERTY() TEnumAsByte<EGripState> GripState = EGripState::UnGripped;

	UPROPERTY() AGrabableActor* HeldItem;
};

UCLASS()
class STIRCRAZY_API AVrPawn : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	AVrPawn(const FObjectInitializer& ObjectInitializer);

	//Delegates
	FOnGraspDelegate OnGraspDelegate;
	
	FOnUngraspDelegate OnUngraspDelegate;

	UPROPERTY()
	UVRMovementComp* VRMovement;

	//Default Scene Root
	UPROPERTY(EditAnywhere) USceneComponent* DefaultRoot;

	//Head
	UPROPERTY(EditAnywhere) USceneComponent* VRRoot;
	UPROPERTY(EditAnywhere) UCameraComponent* VRCamera; 

	//Capsule
	//UPROPERTY(EditAnywhere) UCapsuleComponent* Capsule;

	//Controllers
	UPROPERTY(VisibleAnywhere) UMotionControllerComponent* LeftController;
	UPROPERTY(VisibleAnywhere) UMotionControllerComponent* RightController;

	//Replication For Controllers
	UPROPERTY(EditAnywhere, Replicated) FHandState LeftHandState;
	UPROPERTY(EditAnywhere, Replicated) FHandState RightHandState;
	
	//Controller Hand Meshes, to be set in blueprint
	UPROPERTY(EditAnywhere) UStaticMeshComponent* SM_Left;
	UPROPERTY(EditAnywhere) UStaticMeshComponent* SM_Right;

	//Input, to be set in blueprint
	UPROPERTY(EditAnywhere) UInputMappingContext* InWorldContext;
	UPROPERTY(EditAnywhere) UInputAction* LGraspAction;
	UPROPERTY(EditAnywhere) UInputAction* LUngraspAction;
	UPROPERTY(EditAnywhere) UInputAction* RGraspAction;
	UPROPERTY(EditAnywhere) UInputAction* RUngraspAction;
	UPROPERTY(EditAnywhere) UInputAction* MoveAction;
	UPROPERTY(EditAnywhere) UInputAction* TurnAction;

	///*DEV ONLY*/// I like to test in flatscreen, so i need to use my mouse to look around, hence "LookAction"
	UPROPERTY(EditAnywhere) UInputAction* LookAction;
	UPROPERTY(EditAnywhere) UInputAction* DevAction;


	//////////////////////Pickup & Dropping

	//Predicts Pickup action and then gets server to handle it. GrabableActor class will reconcile if pickup is invalid.
	UFUNCTION(BlueprintCallable)
	void RequestItemPickup(AGrabableActor* Item, bool isRightHand);

	//Gains ownership and gets the GrabableActor to handle the pickup. Reconciliation will happen after this
	UFUNCTION(Server, Reliable)
	void ServerRequestItemPickup(AGrabableActor* Item, bool isRightHand);
	virtual void ServerRequestItemPickup_Implementation(AGrabableActor* Item, bool isRightHand);

	UFUNCTION(Server, Reliable)
	void RequestInteractableOwnership(AWorldInteractable* Interactable, bool isRightHand);
	
	/*Drop is Handled in the item class. Since we already have ownership of the item theres no point in starting the drop process from this class to try and gain ownership*/

	/*DEV ONLY*/ //this is the var i like to use when i test in flatscreen. it fixes stuf for me. Can be set in blueprint
	UPROPERTY(EditAnywhere) bool pancakeDev;


protected:
	//Called when the player hits the grip button. Can be used to plat a grip anim
	void Grasp(const FInputActionInstance &Instance, bool isRightHand);
	void UnGrasp(const FInputActionInstance &Instance, bool isRightHand);
	void Move(const FInputActionInstance &Instance);
	void Turn(const FInputActionInstance &Instance);
	///*DEVONLY*/// just the look function for flatscreen testing 
	void Look(const FInputActionInstance &Instance);
	
	//This fucntion is needed to specify your own properties for replication
	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;


	//Discord
	#ifdef PLATFORM_WINDOWS
	discord::Core* Core{};
	#endif

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

};
