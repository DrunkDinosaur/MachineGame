// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "NiagaraSystem.h"
#include "GameFramework/PlayerController.h"

#include "TacticalController.generated.h"


class ATacticalCameraPawn;
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


	virtual void Tick(float DeltaTime) override;

	void SetSelectedCharacters(TArray<AMachineGameCharacter*> SelectedChars);

protected:
	uint32 bMoveToMouseCursor : 1;

	virtual void SetupInputComponent() override;
	virtual void BeginPlay() override;

	void OnInputStarted();
	void OnSetDestinationTriggered();
	void OnSetDestinationReleased();
	void OnSelectClicked();
	void OnSelectDrag();
	void OnHoldShift();
	void OnReleaseShift();
	
	void AddToSelectedCharacters(AActor* SelectedActorPtr);
	
private:
	FVector CachedDestination;
	float HoldDelay; // For how long it has been pressed
	TSet<AMachineGameCharacter*> SelectedCharacters;

	UPROPERTY(EditAnywhere)
    TSubclassOf<UUserWidget> TacticalHudWidgetClass;
    
    UPROPERTY(EditAnywhere)
    UUserWidget* HudWidget; 
	
	ATacticalCameraPawn* TacticalCamera;
	
	void TrackMouseOnViewPort();
	void SelectCharacter(AMachineGameCharacter* IngameCharacterPtr);
	void ClearSelection();
};
