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
#include "BoxSelectionHUD.h"

#include "TacticalCameraPawn.h"
#include "Blueprint/UserWidget.h"

ATacticalController::ATacticalController()
{
	bShowMouseCursor = true;
	DefaultMouseCursor = EMouseCursor::Default;
	CachedDestination = FVector::ZeroVector;
	HoldDelay = 0.f;
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
	TacticalCamera = Cast<ATacticalCameraPawn>(GetPawn());
	if(!TacticalCamera)
	{
		UE_LOG(LogTemp, Error, TEXT("TACTICAL CAMERA CAST FAILED"));
	}

	HudWidget = CreateWidget(this, TacticalHudWidgetClass);
	if(HudWidget)
	{
		HudWidget->AddToViewport();
	}
}

void ATacticalController::Tick(float DeltaTime)
{
	TrackMouseOnViewPort();	
}

void ATacticalController::TrackMouseOnViewPort()
{
	//FVector2D MousePosition = UWidgetLayoutLibrary::GetMousePositionOnViewport(GetWorld());

	double MouseX = 0.0;
	double MouseY = 0.0;
	GetMousePosition(MouseX, MouseY);
	const FVector2D ViewPortSize = UWidgetLayoutLibrary::GetViewportSize(GetWorld());
	const double MousePositionNormalizedX = MouseX/ViewPortSize.X;
	const double MousePositionNormalizedY = MouseY/ViewPortSize.Y;

	//UE_LOG(LogTemp, Warning, TEXT("Mouse pos %d, %d"), MouseX, MouseY);
	
	if(MousePositionNormalizedX < 0.10)
	{
		//UE_LOG(LogTemp, Warning, TEXT("MoveLeft"));
		TacticalCamera->PanCameraMouse(-1.0, true, true );
	}
	if(MousePositionNormalizedX > 0.90)
	{
		//UE_LOG(LogTemp, Warning, TEXT("MoveRight"));
		TacticalCamera->PanCameraMouse(1.0, true, true );
	}
	if(MousePositionNormalizedY > 0.90)
	{
		//UE_LOG(LogTemp, Warning, TEXT("MoveUp"));
		TacticalCamera->PanCameraMouse(-1.0, false, true );
	}
	if(MousePositionNormalizedY < 0.10)
	{
		//UE_LOG(LogTemp, Warning, TEXT("MoveDown"));
		TacticalCamera->PanCameraMouse(1.0, false, true );
	}
	
}

void ATacticalController::SelectCharacter(AMachineGameCharacter* IngameCharacterPtr)
{
	SelectedCharacters.Add(IngameCharacterPtr);
	IngameCharacterPtr->Select();
}

void ATacticalController::ClearSelection()
{
	for(AMachineGameCharacter* SelectedCharacter: SelectedCharacters)
	{
		SelectedCharacter->Deselect();
	}
	SelectedCharacters.Empty();
}


void ATacticalController::SetupInputComponent()
{
	// set up gameplay key bindings
	Super::SetupInputComponent();

	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(InputComponent))
	{
		EnhancedInputComponent->BindAction(SetDestinationClickAction, ETriggerEvent::Started, this, &ATacticalController::OnInputStarted);
		EnhancedInputComponent->BindAction(SetDestinationClickAction, ETriggerEvent::Triggered, this, &ATacticalController::OnSetDestinationTriggered);
		EnhancedInputComponent->BindAction(SetDestinationClickAction, ETriggerEvent::Completed, this, &ATacticalController::OnSetDestinationReleased);
		EnhancedInputComponent->BindAction(SetDestinationClickAction, ETriggerEvent::Canceled, this, &ATacticalController::OnSetDestinationReleased);

		EnhancedInputComponent->BindAction(SelectClickAction, ETriggerEvent::Started, this, &ATacticalController::OnInputStarted);
		
		EnhancedInputComponent->BindAction(SelectClickAction, ETriggerEvent::Triggered, this, &ATacticalController::OnSelectDrag);
		EnhancedInputComponent->BindAction(SelectClickAction, ETriggerEvent::Completed, this, &ATacticalController::OnSelectClicked);

		EnhancedInputComponent->BindAction(HoldShiftAction, ETriggerEvent::Triggered, this, &ATacticalController::OnHoldShift);
		EnhancedInputComponent->BindAction(HoldShiftAction, ETriggerEvent::Completed, this, &ATacticalController::OnReleaseShift);


	}
}

void ATacticalController::OnInputStarted()
{
	StopMovement();
}

// Triggered every frame when the input is held down
void ATacticalController::OnSetDestinationTriggered()
{
	 HoldDelay += GetWorld()->GetDeltaSeconds();
	
	 FHitResult Hit;
	 bool bHitSuccessful;
	 bHitSuccessful = GetHitResultUnderCursor(ECollisionChannel::ECC_Visibility, true, Hit);
	
	 if (bHitSuccessful)
	 {
	 	CachedDestination = Hit.Location;
	 }
	//
	// for(AMachineGameCharacter* ControlledPawn : SelectedCharacters)
	// {
	// 	if (ControlledPawn != nullptr)
	// 	{
	// 		FVector WorldDirection = (CachedDestination - ControlledPawn->GetActorLocation()).GetSafeNormal();
	// 		ControlledPawn->AddMovementInput(WorldDirection, 1.0, false);
	// 	}
	// }
}

void ATacticalController::OnSetDestinationReleased()
{
	if (HoldDelay <= ShortPressThreshold)
	{
		for(AMachineGameCharacter* SelectedCharacter : SelectedCharacters)
		{
			UE_LOG(LogTemp, Warning, TEXT("SIMPLE MOVE"))
			UAIBlueprintHelperLibrary::SimpleMoveToLocation(SelectedCharacter->GetController(), CachedDestination);
		}

		UNiagaraFunctionLibrary::SpawnSystemAtLocation(this, FXCursor, CachedDestination, FRotator::ZeroRotator, FVector(1.f, 1.f, 1.f), true, true, ENCPoolMethod::None, true);
	}

	HoldDelay = 0.f;
}

void ATacticalController::OnSelectClicked()
{
	if (HoldDelay <= ShortPressThreshold)
	{
		FHitResult Hit;
		bool bHitSuccessful = GetHitResultUnderCursor(ECollisionChannel::ECC_WorldStatic, true, Hit);

		if(bHitSuccessful)
		{
			AddToSelectedCharacters(Hit.GetActor());
		}
	}
	
	if(ABoxSelectionHUD* Hud = Cast<ABoxSelectionHUD>(GetHUD()))
	{
		Hud->StopDrawing();
	}

	HoldDelay = 0.0;
}

void ATacticalController::OnSelectDrag()
{
	HoldDelay += GetWorld()->GetDeltaSeconds();
	UE_LOG(LogTemp, Warning, TEXT("AMachineGamePlayerController::OnSelectDrag()"));
	if(ABoxSelectionHUD* Hud = Cast<ABoxSelectionHUD>(GetHUD()))
	{
		Hud->StartDrawing();
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("No BoxSelectionHud present"));
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

void ATacticalController::AddToSelectedCharacters(AActor* SelectedCharacterPtr) 
{
	if(!bShiftPressed)
	{
		ClearSelection();	
	}
	
	if (AMachineGameCharacter* IngameCharacterPtr = Cast<AMachineGameCharacter>(SelectedCharacterPtr))
	{
		UE_LOG(LogTemp, Display, TEXT( "Hit actor: %s" ), *IngameCharacterPtr->GetName());
		SelectCharacter(IngameCharacterPtr);
		//SelectedCharacters.Add(IngameCharacterPtr);
		//IngameCharacterPtr->Select();
	}

	UE_LOG(LogTemp, Display, TEXT( "Selected actors count: %i" ), SelectedCharacters.Num() );
}

void ATacticalController::SetSelectedCharacters(TArray<AMachineGameCharacter*> SelectedChars)
{
	for(AMachineGameCharacter* Char : SelectedChars)
	{
		SelectCharacter(Char);
		SelectedCharacters.Add(Char);
		Char->Select();
	}
}

