// Fill out your copyright notice in the Description page of Project Settings.


#include "TacticalGameMode.h"

#include "TacticalController.h"
#include "MachineGame/SybSystems/CoverSystem/CoverSystem.h"

ATacticalGameMode::ATacticalGameMode()
{
	// use our custom PlayerController class
	PlayerControllerClass = ATacticalController::StaticClass();
	//
	// // set default pawn class to our Blueprinted character
	// static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/TopDown/Blueprints/BP_TopDownCharacter"));
	// if (PlayerPawnBPClass.Class != nullptr)
	// {
	// 	DefaultPawnClass = PlayerPawnBPClass.Class;
	// }
	//
	// // set default controller to our Blueprinted controller
	// static ConstructorHelpers::FClassFinder<APlayerController> PlayerControllerBPClass(TEXT("/Game/TopDown/Blueprints/BP_TopDownPlayerController"));
	// if(PlayerControllerBPClass.Class != NULL)
	// {
	// 	PlayerControllerClass = PlayerControllerBPClass.Class;
	// }
}

void ATacticalGameMode::BeginPlay(){
	Super::BeginPlay();

	UE_LOG(LogTemp, Warning, TEXT("GameModeGM BeginPlay"));
	
	CoverSystem = UCoverSystem::GetInstance(GetWorld());

	if(CoverSystem){
		UE_LOG(LogTemp, Display, TEXT("Cover system initialized"));
	}
	else{
		UE_LOG(LogTemp, Error, TEXT("Cover system is NOT initialized"));
	}

	/*if(CoverSystem && CoverSystem->GetWorld()){
		CoverSystem->GenerateNavMeshDebugLines();
	}*/
	
}

void ATacticalGameMode::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	//CoverSystem->bShutdown = true;
	CoverSystem = nullptr;
}
