// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "MachineGame/MachineGameCharacter.h"
#include "BoxSelectionHUD.generated.h"

/**
 * 
 */
UCLASS()
class MACHINEGAME_API ABoxSelectionHUD : public AHUD
{
	GENERATED_BODY()

public:
	virtual void DrawHUD() override;
	
	//TArray<AMachineGameCharacter*> SelectCharactersInRectangle(const FVector2D BoxStart, const FVector2D BoxEnd);
	
	void StartDrawing();
	void StopDrawing();

private:
	FVector2d InitialMousePosition;
	FVector2d CurrentMousePosition;

	bool bIsDrawing = false;
};
