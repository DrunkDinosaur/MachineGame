// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "TacticalGameMode.generated.h"

class UCoverSystem;
/**
 * 
 */
UCLASS()
class MACHINEGAME_API ATacticalGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	ATacticalGameMode();

private:
	UCoverSystem* CoverSystem;

public:
	virtual void BeginPlay() override;
	virtual void EndPlay(EEndPlayReason::Type EndPlayReason) override;
};
