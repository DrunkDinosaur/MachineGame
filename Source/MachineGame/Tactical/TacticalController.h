// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "NiagaraSystem.h"
#include "GameFramework/PlayerController.h"
#include "TacticalController.generated.h"

struct FInputActionValue;
class AMachineGameCharacter;
/**
 * 
 */
UCLASS()
class MACHINEGAME_API ATacticalController : public APlayerController
{
	GENERATED_BODY()
	
public:
	ATacticalController();

	bool bShiftPressed = false;

	/** Time Threshold to know if it was a short press */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	float ShortPressThreshold;
	
	/** Time Threshold to know if it was a short press */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	float CameraPanSense;

	/** FX Class that we will spawn when clicking */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	UNiagaraSystem* FXCursor;

	/** MappingContext */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Input, meta=(AllowPrivateAccess = "true"))
	class UInputMappingContext* DefaultMappingContext;
	
	/** Jump Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Input, meta=(AllowPrivateAccess = "true"))
	class UInputAction* SetDestinationClickAction;

	/** Select Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Input, meta=(AllowPrivateAccess = "true"))
	class UInputAction* SelectClickAction;

	/** Hold Shift */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Input, meta=(AllowPrivateAccess = "true"))
	class UInputAction* HoldShiftAction;
	
	/** Camera Controls */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Input, meta=(AllowPrivateAccess = "true"))
	class UInputAction* CameraRightAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Input, meta=(AllowPrivateAccess = "true"))
	class UInputAction* CameraUpAction;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Input, meta=(AllowPrivateAccess = "true"))
	class UInputAction* CameraZoomAction;


	virtual void Tick(float DeltaTime) override;
protected:
	/** True if the controlled character should navigate to the mouse cursor. */
	uint32 bMoveToMouseCursor : 1;

	virtual void SetupInputComponent() override;
	
	// To add mapping context
	virtual void BeginPlay() override;

	/** Input handlers */
	void OnInputStarted();
	void OnSetDestinationTriggered();
	void OnSetDestinationReleased();
	void OnSelectClick();
	void OnHoldShift();
	void OnReleaseShift();

	void OnCameraRight(const FInputActionValue & Value);
	void OnCameraUp(const FInputActionValue & Value);
	void OnCameraZoom(const FInputActionValue & Value);
	
	void AddToSelectedCharacters(AActor* SelectedActorPtr);
	

private:
	FVector CachedDestination;
	float FollowTime; // For how long it has been pressed
	TSet<AMachineGameCharacter*> SelectedCharacters;
	void PanCamera(float AxisValue, bool bOrthogonal, bool zeroZ) const;
	
};


