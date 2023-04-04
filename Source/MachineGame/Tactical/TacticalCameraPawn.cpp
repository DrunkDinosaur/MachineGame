// Fill out your copyright notice in the Description page of Project Settings.

#include "TacticalCameraPawn.h"

#include "EnhancedInputComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "InputActionValue.h"

ATacticalCameraPawn::ATacticalCameraPawn()
{
	PrimaryActorTick.bCanEverTick = true;

	SpringArm = CreateDefaultSubobject<USpringArmComponent>("SpringArm");
	SpringArm->SetupAttachment(RootComponent);

	Camera = CreateDefaultSubobject<UCameraComponent>("Camera");
	Camera->SetupAttachment(SpringArm);
}

void ATacticalCameraPawn::BeginPlay()
{
	Super::BeginPlay();
	CameraPanSense = 25.f;
}

void ATacticalCameraPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ATacticalCameraPawn::OnTestAction()
{
	UE_LOG(LogTemp, Warning, TEXT("TEST PAWN CAMERA ACTION"));
}

void ATacticalCameraPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(InputComponent))
	{
		EnhancedInputComponent->BindAction(CameraRightAction, ETriggerEvent::Triggered, this, &ATacticalCameraPawn::OnCameraRight);
		EnhancedInputComponent->BindAction(CameraUpAction, ETriggerEvent::Triggered, this, &ATacticalCameraPawn::OnCameraUp);
		EnhancedInputComponent->BindAction(CameraZoomAction, ETriggerEvent::Triggered, this, &ATacticalCameraPawn::OnCameraZoom);
		EnhancedInputComponent->BindAction(CameraRotationAction, ETriggerEvent::Triggered, this, &ATacticalCameraPawn::OnCameraRotate);
	}
}

void ATacticalCameraPawn::RotateCamera(float Yaw, float Pitch)
{
	FRotator RotationOffset = GetCameraComponent()->GetComponentRotation();
	RotationOffset.Yaw = Yaw;
	RotationOffset.Pitch = Pitch;
	RotationOffset.Roll = 0.0;
	AddActorWorldRotation(RotationOffset);
}

void ATacticalCameraPawn::PanCamera(float AxisValue, bool bOrthogonal, bool zeroZ)
{
	FVector OffsetVector = GetCameraComponent()->GetComponentRotation().Vector().GetSafeNormal();

	if (zeroZ)
	{
		OffsetVector.Z = 0;
	}

	if (bOrthogonal)
	{
		OffsetVector = FVector(-1 * OffsetVector.Y, OffsetVector.X, OffsetVector.Z);
	}

	AddActorWorldOffset(OffsetVector * AxisValue * CameraPanSense);
}


void ATacticalCameraPawn::OnCameraRight(const FInputActionValue & Value) 
{
	UE_LOG(LogTemp, Warning, TEXT("OnCameraRight camera"));
	PanCamera(Value.Get<float>(), true, true );
}

void ATacticalCameraPawn::OnCameraUp(const FInputActionValue& Value) 
{
	PanCamera(Value.Get<float>(), false, true );
}

void ATacticalCameraPawn::OnCameraZoom(const FInputActionValue& Value)
{
	PanCamera(Value.Get<float>(), false, false);
}

void ATacticalCameraPawn::OnCameraRotate(const FInputActionValue& Value)
{
	float AxisValue = Value.Get<float>();
	RotateCamera(AxisValue, 0.0);
}
