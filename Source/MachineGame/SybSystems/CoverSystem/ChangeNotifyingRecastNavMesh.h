// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "NavMesh/RecastNavMesh.h"
#include "ChangeNotifyingRecastNavMesh.generated.h"

// DELEGATES //

// Fired every X seconds.
// ChangedTiles contains tiles that have been updated since the last timer.
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FNavmeshTilesUpdatedBufferedDelegate, const TSet<uint64>&, ChangedTiles);

// Fired as tiles are updated.
// ChangedTiles contains the same tiles as what get passed around inside Recast.
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FNavmeshTilesUpdatedImmediateDelegate, const TSet<uint64>&, ChangedTiles);

// Fires once navigation generation is finished, i.e. there are no dirty tiles left.
// ChangedTiles contains all the tiles that have been updated since the last time nav was finished.
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FNavmeshTilesUpdatedUntilFinishedDelegate, const TSet<uint64>&, ChangedTiles);

/**
 * 
 */
UCLASS()
class MACHINEGAME_API AChangeNotifyingRecastNavMesh : public ARecastNavMesh
{
	GENERATED_BODY()
protected:
	TSet<uint64> UpdatedTilesIntervalBuffer;

	TSet<uint64> UpdatedTilesUntilFinishedBuffer;

	// Lock used for interval-buffered tile updates.
	FCriticalSection TileUpdateLockObject;

	const float TileBufferInterval = 0.2f;

	FTimerHandle TileUpdateTimerHandle;
	
public:	
	UPROPERTY()
	FNavmeshTilesUpdatedImmediateDelegate NavmeshTilesUpdatedImmediateDelegate;

	UPROPERTY()
	FNavmeshTilesUpdatedBufferedDelegate NavmeshTilesUpdatedBufferedDelegate;

	UPROPERTY()
	FNavmeshTilesUpdatedUntilFinishedDelegate NavmeshTilesUpdatedUntilFinishedDelegate;

	AChangeNotifyingRecastNavMesh();

	AChangeNotifyingRecastNavMesh(const FObjectInitializer& ObjectInitializer);

	virtual void BeginPlay() override;

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	// Called after a set of tiles had been updated. Due to how Recast's implementation works, it may repeatedly contain the same tiles between successive invocations.
	// This is worked around by buffering tile updates (see delegates).
	virtual void OnNavMeshTilesUpdated(const TArray<uint32>& ChangedTiles) override;
	virtual void OnNavMeshTilesUpdated(const TArray<FNavTileRef>& ChangedTiles) override;

	// Broadcasts buffered tile updates every TileBufferInterval seconds via NavmeshTilesUpdatedDelegate. Thread-safe.
	UFUNCTION()
	void ProcessQueuedTiles();

	// Delegate handler.
	UFUNCTION()
	void OnNavmeshGenerationFinishedHandler(ANavigationData* NavData);
};
