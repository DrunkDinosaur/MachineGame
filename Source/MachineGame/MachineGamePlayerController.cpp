// Copyright Epic Games, Inc. All Rights Reserved.

#include "MachineGamePlayerController.h"
#include "GameFramework/Pawn.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "NiagaraSystem.h"
#include "NiagaraFunctionLibrary.h"
#include "MachineGameCharacter.h"
#include "Engine/World.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"

AMachineGamePlayerController::AMachineGamePlayerController()
{
	bShowMouseCursor = true;
	DefaultMouseCursor = EMouseCursor::Default;
	CachedDestination = FVector::ZeroVector;
	FollowTime = 0.f;
}

void AMachineGamePlayerController::BeginPlay()
{
	// Call the base class  
	Super::BeginPlay();

	//Add Input Mapping Context
	if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
	{
		Subsystem->AddMappingContext(DefaultMappingContext, 0);
	}
}

void AMachineGamePlayerController::SetupInputComponent()
{
	// set up gameplay key bindings
	Super::SetupInputComponent();

	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(InputComponent))
	{
		// Setup mouse input events
		EnhancedInputComponent->BindAction(SetDestinationClickAction, ETriggerEvent::Started, this, &AMachineGamePlayerController::OnInputStarted);
		EnhancedInputComponent->BindAction(SetDestinationClickAction, ETriggerEvent::Triggered, this, &AMachineGamePlayerController::OnSetDestinationTriggered);
		EnhancedInputComponent->BindAction(SetDestinationClickAction, ETriggerEvent::Completed, this, &AMachineGamePlayerController::OnSetDestinationReleased);
		EnhancedInputComponent->BindAction(SetDestinationClickAction, ETriggerEvent::Canceled, this, &AMachineGamePlayerController::OnSetDestinationReleased);

		EnhancedInputComponent->BindAction(SelectClickAction, ETriggerEvent::Started, this, &AMachineGamePlayerController::OnInputStarted);
		EnhancedInputComponent->BindAction(SelectClickAction, ETriggerEvent::Completed, this, &AMachineGamePlayerController::OnSelectClick);

		EnhancedInputComponent->BindAction(HoldShiftAction, ETriggerEvent::Triggered, this, &AMachineGamePlayerController::OnHoldShift);
		EnhancedInputComponent->BindAction(HoldShiftAction, ETriggerEvent::Completed, this, &AMachineGamePlayerController::OnReleaseShift);
	}
}



void AMachineGamePlayerController::OnInputStarted()
{
	StopMovement();
}

// Triggered every frame when the input is held down
void AMachineGamePlayerController::OnSetDestinationTriggered()
{
	// We flag that the input is being pressed
	FollowTime += GetWorld()->GetDeltaSeconds();
	
	// We look for the location in the world where the player has pressed the input
	FHitResult Hit;
	bool bHitSuccessful = false;
	bHitSuccessful = GetHitResultUnderCursor(ECollisionChannel::ECC_Visibility, true, Hit);
	
	// If we hit a surface, cache the location
	if (bHitSuccessful)
	{
		CachedDestination = Hit.Location;
	}
	
	// Move towards mouse pointer or touch
	APawn* ControlledPawn = GetPawn();
	if (ControlledPawn != nullptr)
	{
		FVector WorldDirection = (CachedDestination - ControlledPawn->GetActorLocation()).GetSafeNormal();
		ControlledPawn->AddMovementInput(WorldDirection, 1.0, false);
	}
}

void AMachineGamePlayerController::OnSetDestinationReleased()
{
	// If it was a short press
	if (FollowTime <= ShortPressThreshold)
	{
		// We move there and spawn some particles
		UAIBlueprintHelperLibrary::SimpleMoveToLocation(this, CachedDestination);
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(this, FXCursor, CachedDestination, FRotator::ZeroRotator, FVector(1.f, 1.f, 1.f), true, true, ENCPoolMethod::None, true);
	}

	FollowTime = 0.f;
}

void AMachineGamePlayerController::OnSelectClick()
{
	UE_LOG(LogTemp, Warning, TEXT("AMachineGamePlayerController::OnSelectClick()"));

	FHitResult Hit;
	bool bHitSuccessful = GetHitResultUnderCursor(ECollisionChannel::ECC_WorldStatic, true, Hit);

	if(bHitSuccessful)
	{
		AddToSelectedCharacters(Hit.GetActor());
	}
}

void AMachineGamePlayerController::OnHoldShift()
{
	bShiftPressed = true;
}

void AMachineGamePlayerController::OnReleaseShift()
{
	bShiftPressed = false;
}

void AMachineGamePlayerController::AddToSelectedCharacters(AActor* SelectedCharacterPtr) 
{
	if(!bShiftPressed)
	{
		SelectedCharacters.Empty();	
	}
	
	if (AMachineGameCharacter* IngameCharacterPtr = Cast<AMachineGameCharacter>(SelectedCharacterPtr))
	{
		UE_LOG(LogTemp, Display, TEXT( "Hit actor: %s" ), *IngameCharacterPtr->GetName() );
		SelectedCharacters.Add(IngameCharacterPtr);
	}
	else if(!bShiftPressed){
		SelectedCharacters.Empty();
	}

	UE_LOG(LogTemp, Display, TEXT( "Hit actor: %i" ), SelectedCharacters.Num() );
}
