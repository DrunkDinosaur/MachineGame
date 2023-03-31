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
#include "TacticalCameraPawn.h"
#include "Blueprint/WidgetLayoutLibrary.h"
#include "Camera/CameraComponent.h"

ATacticalController::ATacticalController()
{
	bShowMouseCursor = true;
	DefaultMouseCursor = EMouseCursor::Default;
	CachedDestination = FVector::ZeroVector;
	FollowTime = 0.f;
	CameraPanSense = 25.f;
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

void ATacticalController::Tick(float DeltaTime)
{
	//FVector2D mousePosition = UWidgetLayoutLibrary::GetMousePositionOnViewport(GetWorld());
	//UE_LOG(LogTemp, Warning, TEXT("Mouse pos %d, %d"), mousePosition.X, mousePosition.Y);
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
		EnhancedInputComponent->BindAction(CameraUpAction, ETriggerEvent::Triggered, this, &ATacticalController::OnCameraUp);
		EnhancedInputComponent->BindAction(CameraZoomAction, ETriggerEvent::Triggered, this, &ATacticalController::OnCameraZoom);
		EnhancedInputComponent->BindAction(CameraRotationAction, ETriggerEvent::Triggered, this, &ATacticalController::OnCameraRotate);
		
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

void ATacticalController::OnCameraRight(const FInputActionValue & Value) 
{
	PanCamera(Value.Get<float>(), true, true );
}

void ATacticalController::OnCameraUp(const FInputActionValue& Value) 
{
	PanCamera(Value.Get<float>(), false, true );
}

void ATacticalController::OnCameraZoom(const FInputActionValue& Value)
{
	PanCamera(Value.Get<float>(), false, false);
}

void ATacticalController::OnCameraRotate(const FInputActionValue& Value)
{
	float AxisValue = Value.Get<float>();
	if(ATacticalCameraPawn* RTSCameraPawn = Cast<ATacticalCameraPawn>(GetPawn()))
	{
		FRotator RotationOffset = RTSCameraPawn->GetCameraComponent()->GetComponentRotation();
		RotationOffset.Yaw = AxisValue;
		RotationOffset.Pitch = 0.0;
		RotationOffset.Roll = 0.0;
		RTSCameraPawn->AddActorWorldRotation(RotationOffset);
		//RTSCameraPawn->AddActorWorldOffset(OffsetVector*AxisValue*CameraPanSense);	
	}
}

void ATacticalController::AddToSelectedCharacters(AActor* SelectedCharacterPtr) 
{
	if(!bShiftPressed)
	{
		SelectedCharacters.Empty();	
	}
	
	if (AMachineGameCharacter* IngameCharacterPtr = Cast<AMachineGameCharacter>(SelectedCharacterPtr))
	{
		UE_LOG(LogTemp, Display, TEXT( "Hit actor: %s" ), *IngameCharacterPtr->GetName());
		SelectedCharacters.Add(IngameCharacterPtr);
	}
	else if(!bShiftPressed){
		SelectedCharacters.Empty();
	}

	UE_LOG(LogTemp, Display, TEXT( "Selected actors count: %i" ), SelectedCharacters.Num() );
}

void ATacticalController::PanCamera(float AxisValue, bool bOrthogonal, bool zeroZ) const
{
	//UE_LOG(LogTemp, Display, TEXT( "PanCameraXY" ));
	if(ATacticalCameraPawn* RTSCameraPawn = Cast<ATacticalCameraPawn>(GetPawn()))
	{
		FVector OffsetVector = RTSCameraPawn->GetCameraComponent()->GetComponentRotation().Vector().GetSafeNormal();
		
		if(zeroZ)
		{
			OffsetVector.Z = 0;
		}
		
		if(bOrthogonal)
		{
			OffsetVector = FVector(OffsetVector.Y,OffsetVector.X,OffsetVector.Z) ;
		}
		
		RTSCameraPawn->AddActorWorldOffset(OffsetVector*AxisValue*CameraPanSense);	
	}
}
