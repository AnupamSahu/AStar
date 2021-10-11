// Copyright Epic Games, Inc. All Rights Reserved.

#include "UAStarPathFinder.h"
#include "DrawDebugHelpers.h"
#include "AStar/Grid/GridLevelScript.h"
#include "Kismet/KismetSystemLibrary.h"

void UAStarPathFinder::BeginPlay()
{
	Super::BeginPlay();

	// Fetch the AGridLevelScript from Current Level
	GridLevelActor = Cast<AGridLevelScript>(GetWorld()->GetLevelScriptActor());
	if (IsValid(GridLevelActor))
	{
		// When the Grid is generated, update local Map
		GridLevelActor->OnGridGenerated.AddUniqueDynamic(this, &UAStarPathFinder::UAStarPathFinder::OnGridUpdate);
	}
}

void UAStarPathFinder::Initialize(const FVector& StartLocation, const FVector& DestinationLocation, TArray<FVector>& OutPath)
{
	// If Map is empty, don't proceed
	if(Map.MapBase.Num() == 0) { return; }

	// Start fresh 
	OutPath.Empty();
	Open.Empty();
	Closed.Empty();


	GridLevelActor->ConvertWorldToGridLocation(StartLocation, StartMapLocation);
	GridLevelActor->ConvertWorldToGridLocation(DestinationLocation, DestinationMapLocation);

	// Destination is marked as the Target
	Map[DestinationMapLocation].bIsTarget = true;

	// Initialize the Open Array with the Start MapNode
	Open.HeapPush(Map[StartMapLocation], FMostOptimalNode());
}

void UAStarPathFinder::FindPath(const FVector& StartLocation, const FVector& DestinationLocation, TArray<FVector>& OutPath)
{
	Initialize(StartLocation, DestinationLocation, OutPath);

	// If Map is empty, don't proceed
	if(Map.MapBase.Num() == 0) { return; }
	
	// While there are still MapNodes to be visited
	while (Open.Num() > 0)
	{
		// Current MapNode is now known to be visited
		FMapNode Current;
		Open.HeapPop(Current, FMostOptimalNode());
		Closed.HeapPush(Current, FMostOptimalNode());

		// If we are already at destination, just break
		if (Current.bIsTarget)
		{
			break;
		}

		// Fetch all Neighboring MapNodes of the Current MapNode
		TArray<FMapLocation> Neighbors;
		GetNeighbors(Current.MapLocation, Neighbors);

		// For each Neighbor
		for (int32 i = 0; i < Neighbors.Num(); ++i)
		{
			// If Neighbor is already visited or not walkable (wall or obstacle), reject it and continue
			FMapNode& Neighbor = Map[Neighbors[i]];
			if (Closed.Contains(Neighbor) || !Neighbor.bIsWalkable) { continue; }

			// Calculate the Movement, Heuristic and The FCost 
			const float GCost = FVector::Distance(Neighbor.WorldLocation, Current.WorldLocation);
			const float HCost = FVector::Distance(Neighbor.WorldLocation, DestinationLocation);
			const float NewFCost = GCost + HCost;

			// If New FCost is smaller than the Neighbor's current FCost, we just found a more optimal path
			if (NewFCost < Neighbor.FCost)
			{
				// Update path to Neighbor and its FCost 
				Neighbor.FCost = NewFCost;
				Neighbor.GCost = GCost;
				Neighbor.ParentLocation = Current.MapLocation;
			}

			// Mark Neighbor as seen, we will visit it later
			if (!Open.Contains(Neighbor))
			{
				Open.HeapPush(Neighbor, FMostOptimalNode());
			}
		}
	}
	
	TracePath(OutPath);
}

void UAStarPathFinder::TracePath(TArray<FVector>& OutPath)
{
	// Set the Current MapNode as the Destination MapNode
	FMapNode& Current = Map[DestinationMapLocation];

	// While we haven't arrived at the Starting MapNode and the Current MapNode has a valid Parent (a Path really exists)
	while(Current != Map[StartMapLocation] && Map.IsValidLocation(Current.ParentLocation))
	{
		// Add this MapNode to the Path
		OutPath.Add(Current.WorldLocation);
		// Set Current to the Parent of this MapNode
		Current = Map[Current.ParentLocation];
	}

	if(bDebugPath)
	{
		UKismetSystemLibrary::FlushPersistentDebugLines(GetWorld());
		for(int32 i = 0; i < OutPath.Num() - 1; ++i)
		{
			DrawDebugLine(GetWorld(), OutPath[i], OutPath[i+1], FColor::Red, true, -1, 0, 20.0f);
		}
	}
}

void UAStarPathFinder::OnGridUpdate()
{
	// Validate GridLevelActor, and fetch the Map
	if (IsValid(GridLevelActor))
	{
		GridLevelActor->GetGrid(Map.MapBase);
	}
}

void UAStarPathFinder::GetNeighbors(const FMapLocation& Target, TArray<FMapLocation>& OutNeighborLocations)
{
	// Scan 8 Neighbors if bAllowDiagonal is true, 4 otherwise
	const uint32 NumNeighbors = bAllowDiagonal ? 8 : 4;
	for (uint32 i = 0; i < NumNeighbors; ++i)
	{
		const FMapLocation& NeighborLocation = Target + NeighborIndices[i];
		if (Map.IsValidLocation(NeighborLocation))
		{
			OutNeighborLocations.Add(NeighborLocation);
		}
	}
}
