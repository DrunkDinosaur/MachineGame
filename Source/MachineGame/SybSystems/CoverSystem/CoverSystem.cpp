// Fill out your copyright notice in the Description page of Project Settings.

#include "CoverSystem.h"

#include "ChangeNotifyingRecastNavMesh.h"
#include "DrawDebugHelpers.h"
#include "NavigationSystem.h"
#include "NavMesh/RecastNavMesh.h"

DECLARE_LOG_CATEGORY_EXTERN(LogCoverSystem, Log, All);
DEFINE_LOG_CATEGORY(LogCoverSystem)

UCoverSystem* UCoverSystem::Singleton;

UCoverSystem::UCoverSystem(){
	Singleton = nullptr;
}

void UCoverSystem::DrawNavMeshDebugLines(const int32 TileIndex){
	UE_LOG(LogTemp, Warning, TEXT("DrawNavMeshDebugLines"));
	const ARecastNavMesh* navdata = Cast<ARecastNavMesh>(UNavigationSystemV1::GetCurrent(GetWorld())->MainNavData);

	FRecastDebugGeometry navGeo;
	navGeo.bGatherNavMeshEdges = true;

	//I guess batchquery is needed for some multithreading reason, idk, I copied this code from the web
	navdata->BeginBatchQuery();
	navdata->GetDebugGeometry(navGeo, TileIndex);
	navdata->FinishBatchQuery();

	TArray<FVector>& vertices = navGeo.NavMeshEdges;

	//draw debug edges
	if (vertices.Num() <= 1){
		UE_LOG(LogTemp, Warning, TEXT(" <=1  verticies"));
		return;
	}

	/**
	 * Here's how the Edges are stored inside NamVesh: [0,1,2 ... n]
	 * Instead of creating some "struct Edge{...}" these guys just return an array of vertices.
	 * These vertices are paired to make an edge:
	 * So [0,1] is the first edge, [2,3] is the second etc.
	 * hence we have i+2 iteration.
	 * Also it means that there will be always even number of verticies
	 * but I left the boundary condition (i + 1 < vertices.Num()) just in case
	 */
	for (int i = 0; i < vertices.Num(); i += 2){
		if (i + 1 < vertices.Num()){
			const FVector edgeStartVertex = vertices[i];
			const FVector edgeEndVertex = vertices[i + 1];
			const FVector edge = edgeEndVertex - edgeStartVertex;
			//const FVector edgeDir = edge.GetUnsafeNormal();
			DrawDebugPoint(GetWorld(), edgeStartVertex, 5.0, FColor::Green, true, -1);
			DrawDebugDirectionalArrow(GetWorld(), edgeStartVertex, edgeEndVertex, 200.0f, FColor::Purple, true, -1.0f, 0, 2.0f);
		}
	}
}

UCoverSystem::~UCoverSystem(){
	Singleton = nullptr;
}

void UCoverSystem::OnNavMeshTilesUpdated(const TSet<uint32>& UpdatedTiles){

	UE_LOG(LogCoverSystem, Warning, TEXT("UCoverSystem::OnNavMeshTilesUpdated"));
	
	if (!Singleton){
		return;
	}

	for (uint32 TileIdx : UpdatedTiles){
		DrawNavMeshDebugLines(TileIdx);

	}
}

UCoverSystem* UCoverSystem::GetInstance(UWorld* World){

	if (!IsValid(Singleton)){
		Singleton = NewObject<UCoverSystem>(World, "Cover System");
		if (!Singleton){
			UE_LOG(LogCoverSystem, Error, TEXT("Cannot create instance of cover system with NewObject<UCoverSystem>"));
		}

		Singleton->OnBeginPlay();
	}

	return Singleton;
}

void UCoverSystem::OnBeginPlay(){
	//bShutdown = false;
	UE_LOG(LogTemp, Warning, TEXT("UCoverSystem::OnBeginPlay") );
	UNavigationSystemV1* NavigationSystem = UNavigationSystemV1::GetCurrent(GetWorld());
	if (!IsValid(NavigationSystem)){
		UE_LOG(LogCoverSystem, Error, TEXT("NavigationSystem is invalid") );
		return;
	}

	UE_LOG(LogCoverSystem, Display, TEXT("NavigationSystem is valid") );
	
	// subscribe to tile update events on the navmesh
	const ANavigationData* MainNavData = NavigationSystem->MainNavData;
	if (MainNavData && MainNavData->IsA(AChangeNotifyingRecastNavMesh::StaticClass()))
	{
		UE_LOG(LogCoverSystem, Display, TEXT("Adding delegate") );
		Navmesh = const_cast<AChangeNotifyingRecastNavMesh*>(Cast<AChangeNotifyingRecastNavMesh>(MainNavData));
		Navmesh->NavmeshTilesUpdatedBufferedDelegate.AddDynamic(this, &UCoverSystem::OnNavMeshTilesUpdated);
	}
	
}
