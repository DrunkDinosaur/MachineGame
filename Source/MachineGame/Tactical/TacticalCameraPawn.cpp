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
	
	CameraPanSense = 25.f;
	CameraRotationSens = 3.f;
	MouseHoldDelay = 0.1f;
	bEnableMousePan = true;
}

void ATacticalCameraPawn::BeginPlay()
{
	Super::BeginPlay();
}

void ATacticalCameraPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
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

		EnhancedInputComponent->BindAction(MouseCameraRotationAction, ETriggerEvent::Triggered, this, &ATacticalCameraPawn::OnMouseCameraRotate);
		EnhancedInputComponent->BindAction(EnableMouseCameraRotationAction, ETriggerEvent::Triggered, this, &ATacticalCameraPawn::EnableCameraRotate);
		EnhancedInputComponent->BindAction(EnableMouseCameraRotationAction, ETriggerEvent::Completed, this, &ATacticalCameraPawn::DisableCameraRotate);
	}
}

void ATacticalCameraPawn::RotateCamera(const float Yaw, const float Pitch)
{
	const FRotator CurrentRotation = GetActorRotation();
	const FRotator DeltaRotation = FRotator(Pitch, Yaw, 0);
	SetActorRotation(CurrentRotation - DeltaRotation*CameraRotationSens);
}

void ATacticalCameraPawn::PanCameraMouse(float AxisValue, bool bOrthogonal, bool zeroZ)
{
	if(bEnableMousePan){
		PanCamera(AxisValue, bOrthogonal, zeroZ);
	}
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

void ATacticalCameraPawn::OnMouseCameraRotate(const FInputActionValue& Value)
{
	if(bRotateOnMouseInput)
	{
		FVector2d AxisVector = Value.Get<FVector2d>();
		UE_LOG(LogTemp, Warning, TEXT("MouseCameraRotate: %x, %y"), AxisVector.X, AxisVector.Y);
		RotateCamera(AxisVector.X, AxisVector.Y);
	}
}

void ATacticalCameraPawn::EnableCameraRotate()
{
	MouseRotHoldTime += GetWorld()->GetDeltaSeconds();

	if(MouseRotHoldTime > MouseHoldDelay)
	{
		bRotateOnMouseInput = true;
	}
}

void ATacticalCameraPawn::DisableCameraRotate()
{
	bRotateOnMouseInput = false;
	MouseRotHoldTime = 0.f;
}
