// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "CoverSystem.generated.h"

class AChangeNotifyingRecastNavMesh;
/**
 * 
 */
UCLASS()
class MACHINEGAME_API UCoverSystem : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

private:
	static UCoverSystem* Singleton;
	UCoverSystem();

	// Our custom navmesh
	UPROPERTY()
	AChangeNotifyingRecastNavMesh* Navmesh;
	
public:
	/**
		 * @brief - It's called GetInstance. It gets instance. What else do you need to know LMAO
		 * @param World - World context of cover system
		 * @return - Singleton instance of cover system
		 */
	static UCoverSystem* GetInstance(UWorld* World);
	void OnBeginPlay();
	void DrawNavMeshDebugLines(uint64 TileIndex);

	virtual ~UCoverSystem() override;

	// Callback for navmesh tile updates.
	UFUNCTION()
	void OnNavMeshTilesUpdated(const TSet<uint64>& UpdatedTiles);
};
