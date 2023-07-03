// Copyright (c) 2018 David Nadaski. All Rights Reserved.

#include "ChangeNotifyingRecastNavMesh.h"
#include "NavigationSystem.h"
#include "Engine/World.h"

DECLARE_LOG_CATEGORY_EXTERN(OurNavMesh, Log, All);
DEFINE_LOG_CATEGORY(OurNavMesh)

AChangeNotifyingRecastNavMesh::AChangeNotifyingRecastNavMesh() : Super()
{}

AChangeNotifyingRecastNavMesh::AChangeNotifyingRecastNavMesh(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{}

void AChangeNotifyingRecastNavMesh::BeginPlay()
{
	Super::BeginPlay();
	UE_LOG(LogTemp, Warning, TEXT("void AChangeNotifyingRecastNavMesh::BeginPlay()"));
	GetWorld()->GetTimerManager().SetTimer(TileUpdateTimerHandle, this, &AChangeNotifyingRecastNavMesh::ProcessQueuedTiles, TileBufferInterval, true);
	//TODO: remove? started getting double registrations after the mission selector integration
	UNavigationSystemV1::GetCurrent(GetWorld())->OnNavigationGenerationFinishedDelegate.RemoveDynamic(this, &AChangeNotifyingRecastNavMesh::OnNavmeshGenerationFinishedHandler);
	UNavigationSystemV1::GetCurrent(GetWorld())->OnNavigationGenerationFinishedDelegate.AddDynamic(this, &AChangeNotifyingRecastNavMesh::OnNavmeshGenerationFinishedHandler); // avoid a name clash with OnNavMeshGenerationFinished()
}

void AChangeNotifyingRecastNavMesh::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	GetWorld()->GetTimerManager().ClearTimer(TileUpdateTimerHandle);

	Super::EndPlay(EndPlayReason);
}

void AChangeNotifyingRecastNavMesh::OnNavMeshTilesUpdated(const TArray<FNavTileRef>& ChangedTiles)
{
	Super::OnNavMeshTilesUpdated(ChangedTiles);
	UE_LOG(LogTemp, Warning, TEXT("AChangeNotifyingRecastNavMesh::OnNavMeshTilesUpdated"));
	TSet<uint64> updatedTiles;
	UE_LOG(LogTemp, Warning, TEXT("UpdatedTilesNum: %i"), ChangedTiles.Num());
	for (FNavTileRef ChangedTile : ChangedTiles)
	{
		uint64 TileId = (uint64)ChangedTile;
		//UE_LOG(LogTemp, Warning, TEXT("changedTile %llu"), TileId);
		updatedTiles.Add(TileId);
		UpdatedTilesIntervalBuffer.Add(TileId);
		UpdatedTilesUntilFinishedBuffer.Add(TileId);
	}

	// fire the immediate delegate
	UE_LOG(LogTemp, Warning, TEXT("updatedTiles: %i"), updatedTiles.Num());
	UE_LOG(LogTemp, Warning, TEXT("UpdatedTilesIntervalBuffer: %i"), UpdatedTilesIntervalBuffer.Num());
	UE_LOG(LogTemp, Warning, TEXT("UpdatedTilesUntilFinishedBuffer: %i"), UpdatedTilesUntilFinishedBuffer.Num());
	NavmeshTilesUpdatedImmediateDelegate.Broadcast(updatedTiles);
}

void AChangeNotifyingRecastNavMesh::OnNavMeshTilesUpdated(const TArray<uint32>& ChangedTiles)
{
	Super::OnNavMeshTilesUpdated(ChangedTiles);
	UE_LOG(LogTemp, Warning, TEXT("updated: %d"), ChangedTiles.Num() )
}

void AChangeNotifyingRecastNavMesh::ProcessQueuedTiles()
{
	//UE_LOG(LogTemp, Warning, TEXT("AChangeNotifyingRecastNavMesh::ProcessQueuedTiles"));
	//FScopeLock TileUpdateLock(&TileUpdateLockObject);
	UE_LOG(LogTemp, Warning, TEXT("UpdatedTilesIntervalBuffer: %i"), UpdatedTilesIntervalBuffer.Num());
	if (UpdatedTilesIntervalBuffer.Num() > 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("ProcessQueuedTiles - tile count: %d"), UpdatedTilesIntervalBuffer.Num());
		NavmeshTilesUpdatedBufferedDelegate.Broadcast(UpdatedTilesIntervalBuffer);
		UpdatedTilesIntervalBuffer.Empty();
	}
}

void AChangeNotifyingRecastNavMesh::OnNavmeshGenerationFinishedHandler(ANavigationData* NavData)
{
	FScopeLock TileUpdateLock(&TileUpdateLockObject);
	NavmeshTilesUpdatedUntilFinishedDelegate.Broadcast(UpdatedTilesUntilFinishedBuffer);
	UpdatedTilesUntilFinishedBuffer.Empty();
}
