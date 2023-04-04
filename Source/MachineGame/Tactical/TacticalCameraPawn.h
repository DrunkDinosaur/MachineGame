// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InputActionValue.h"
#include "GameFramework/Pawn.h"
#include "TacticalCameraPawn.generated.h"

class UInputAction;
class UCameraComponent;

UCLASS()
class MACHINEGAME_API ATacticalCameraPawn : public APawn
{
	GENERATED_BODY()

public:
	ATacticalCameraPawn();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

	void OnTestAction();
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	FORCEINLINE UCameraComponent* GetCameraComponent() const { return Camera; }

	void RotateCamera(float Yaw, float Pitch); 
	
	void PanCamera(float AxisValue, bool bOrthogonal, bool zeroZ);

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	float CameraPanSense;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Input, meta=(AllowPrivateAccess = "true"))
	class UInputAction* CameraRightAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Input, meta=(AllowPrivateAccess = "true"))
	class UInputAction* CameraUpAction;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Input, meta=(AllowPrivateAccess = "true"))
	class UInputAction* CameraZoomAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Input, meta=(AllowPrivateAccess = "true"))
	class UInputAction* CameraRotationAction;

	void OnCameraRight(const FInputActionValue & Value);
	void OnCameraUp(const FInputActionValue & Value);
	void OnCameraZoom(const FInputActionValue & Value);
	void OnCameraRotate(const FInputActionValue & Value);
private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"));
	class USpringArmComponent* SpringArm;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"));
	UCameraComponent* Camera;
};
