// Fill out your copyright notice in the Description page of Project Settings.


#include "TacticalController.h"

#include "MachineGame/MachineGameCharacter.h"

#include "GameFramework/Pawn.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "NiagaraSystem.h"
#include "NiagaraFunctionLibrary.h"
#include "Engine/World.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"

ATacticalController::ATacticalController()
{
	bShowMouseCursor = true;
	DefaultMouseCursor = EMouseCursor::Default;
	CachedDestination = FVector::ZeroVector;
	FollowTime = 0.f;
}

void ATacticalController::BeginPlay()
{
	// Call the base class  
	Super::BeginPlay();

	//Add Input Mapping Context
	if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
	{
		Subsystem->AddMappingContext(DefaultMappingContext, 0);
	}
}

void ATacticalController::SetupInputComponent()
{
	// set up gameplay key bindings
	Super::SetupInputComponent();

	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(InputComponent))
	{
		// Setup mouse input events
		EnhancedInputComponent->BindAction(SetDestinationClickAction, ETriggerEvent::Started, this, &ATacticalController::OnInputStarted);
		EnhancedInputComponent->BindAction(SetDestinationClickAction, ETriggerEvent::Triggered, this, &ATacticalController::OnSetDestinationTriggered);
		EnhancedInputComponent->BindAction(SetDestinationClickAction, ETriggerEvent::Completed, this, &ATacticalController::OnSetDestinationReleased);
		EnhancedInputComponent->BindAction(SetDestinationClickAction, ETriggerEvent::Canceled, this, &ATacticalController::OnSetDestinationReleased);

		EnhancedInputComponent->BindAction(SelectClickAction, ETriggerEvent::Started, this, &ATacticalController::OnInputStarted);
		EnhancedInputComponent->BindAction(SelectClickAction, ETriggerEvent::Completed, this, &ATacticalController::OnSelectClick);

		EnhancedInputComponent->BindAction(HoldShiftAction, ETriggerEvent::Triggered, this, &ATacticalController::OnHoldShift);
		EnhancedInputComponent->BindAction(HoldShiftAction, ETriggerEvent::Completed, this, &ATacticalController::OnReleaseShift);

		EnhancedInputComponent->BindAction(CameraRightAction, ETriggerEvent::Triggered, this, &ATacticalController::OnCameraRight);
		EnhancedInputComponent->BindAction(CameraUpAction, ETriggerEvent::Completed, this, &ATacticalController::OnCameraUp);
	}
}



void ATacticalController::OnInputStarted()
{
	StopMovement();
}

// Triggered every frame when the input is held down
void ATacticalController::OnSetDestinationTriggered()
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
	for(AMachineGameCharacter* ControlledPawn : SelectedCharacters)
	{
		if (ControlledPawn != nullptr)
		{
			FVector WorldDirection = (CachedDestination - ControlledPawn->GetActorLocation()).GetSafeNormal();
			ControlledPawn->AddMovementInput(WorldDirection, 1.0, false);
		}
	}
}

void ATacticalController::OnSetDestinationReleased()
{
	UE_LOG(LogTemp, Warning, TEXT("ATacticalController::OnSetDestinationReleased()"))
	// If it was a short press
	if (FollowTime <= ShortPressThreshold)
	{
		// We move there and spawn some particles
		for(AMachineGameCharacter* SelectedCharacter : SelectedCharacters)
		{
			UE_LOG(LogTemp, Warning, TEXT("SIMPLE MOVE"))
			UAIBlueprintHelperLibrary::SimpleMoveToLocation(SelectedCharacter->GetController(), CachedDestination);
		}
//		UAIBlueprintHelperLibrary::SimpleMoveToLocation(this, CachedDestination);
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(this, FXCursor, CachedDestination, FRotator::ZeroRotator, FVector(1.f, 1.f, 1.f), true, true, ENCPoolMethod::None, true);
	}

	FollowTime = 0.f;
}

void ATacticalController::OnSelectClick()
{
	UE_LOG(LogTemp, Warning, TEXT("AMachineGamePlayerController::OnSelectClick()"));

	FHitResult Hit;
	bool bHitSuccessful = GetHitResultUnderCursor(ECollisionChannel::ECC_WorldStatic, true, Hit);

	if(bHitSuccessful)
	{
		AddToSelectedCharacters(Hit.GetActor());
	}
}

void ATacticalController::OnHoldShift()
{
	bShiftPressed = true;
}

void ATacticalController::OnReleaseShift()
{
	bShiftPressed = false;
}

void ATacticalController::OnCameraRight()
{
}

void ATacticalController::OnCameraUp()
{
}

void ATacticalController::AddToSelectedCharacters(AActor* SelectedCharacterPtr) 
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

	UE_LOG(LogTemp, Display, TEXT( "Selected actors count: %i" ), SelectedCharacters.Num() );
}
