// Copyright Epic Games, Inc. All Rights Reserved.

#include "UAStarPathFinder.h"
#include "DrawDebugHelpers.h"
#include "AStar/Grid/GridLevelScript.h"
#include "Kismet/KismetSystemLibrary.h"
#include "DistanceFunctionsLibrary.h"

UAStarPathFinder::UAStarPathFinder()
{
	Heuristics.Add(DistanceFunctionsLibrary::GetEuclideanDistance);
	Heuristics.Add(DistanceFunctionsLibrary::GetManhattanDistance);
	Heuristics.Add(DistanceFunctionsLibrary::GetChebyshevDistance);
	Heuristics.Add(DistanceFunctionsLibrary::GetOctileDistance);
}

void UAStarPathFinder::BeginPlay()
{
	Super::BeginPlay();

	// Fetch the AGridLevelScript from Current Level
	GridLevelActor = Cast<AGridLevelScript>(GetWorld()->GetLevelScriptActor());
	if (IsValid(GridLevelActor))
	{
		// When the Grid is generated, update local Map
		GridLevelActor->OnGridGenerated.AddUniqueDynamic(this, &UAStarPathFinder::OnGridUpdate);

		// When the Grid is cleared, clear local Map
		GridLevelActor->OnGridCleared.AddUniqueDynamic(this, &UAStarPathFinder::OnGridClear);
	}
}

void UAStarPathFinder::UpdateMap()
{
	if (IsValid(GridLevelActor))
	{
		GridLevelActor->GetGrid(Map.MapBase);
	}
}

void UAStarPathFinder::ClearMap()
{
	UKismetSystemLibrary::FlushDebugStrings(GetWorld());
	UKismetSystemLibrary::FlushPersistentDebugLines(GetWorld());
	
	Map.MapBase.Empty();
}

bool UAStarPathFinder::Initialize(const FVector& StartLocation, const FVector& DestinationLocation, TArray<FVector>& OutPath)
{
	ClearMap();
	
	UpdateMap();

	// If Map is empty, don't proceed
	if (Map.MapBase.Num() == 0) { return false; }
	
	// Start fresh 
	OutPath.Empty();
	Open.Empty();
	Closed.Empty();
	
	if (!GridLevelActor->ConvertWorldToGridLocation(StartLocation, StartMapLocation)) { return false; }
	if (!GridLevelActor->ConvertWorldToGridLocation(DestinationLocation, DestinationMapLocation)) { return false; }

	// Destination is marked as the Target
	Map[DestinationMapLocation].bIsTarget = true;

	// Initialize the Open Array with the Start MapNode
	Open.HeapPush(Map[StartMapLocation], FMostOptimalNode());
	
	return true;
}

void UAStarPathFinder::FindPath(const FVector& StartLocation, const FVector& DestinationLocation, TArray<FVector>& OutPath)
{
	if (!Initialize(StartLocation, DestinationLocation, OutPath)) { return; }
	
	// While there are still MapNodes to be visited
	while (Open.Num() > 0)
	{
		// Current MapNode is now known to be visited
		FMapNode Current;
		Open.HeapPop(Current, FMostOptimalNode());
		Closed.HeapPush(Current, FMostOptimalNode());

		if(bDebugCosts)
		{
			const FString& CurrentFCost = FString::Printf(TEXT("(%.*f)"), 1, Current.FCost);
			DrawDebugString(GetWorld(), Current.WorldLocation, *CurrentFCost, nullptr, FColor::Red, 60.0f);
		}
		
		// If we are already at destination, just break
		if (Current.bIsTarget) { break; }

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
			float GCost = 0.0f;
			float HCost = 0.0f;
			
			if(Heuristics.IsValidIndex(HeuristicIndex))
			{
				GCost = Heuristics[HeuristicIndex](Neighbor.WorldLocation, StartLocation);
				HCost = Heuristics[HeuristicIndex](Neighbor.WorldLocation, DestinationLocation);
			}
			else
			{
				GCost = FVector::Distance(Neighbor.WorldLocation, StartLocation);
				HCost = FVector::Distance(Neighbor.WorldLocation, DestinationLocation);
			}

			const float NewFCost = GCost + HCost;
			
			// If New FCost is smaller than the Neighbor's current FCost, we just found a more optimal path
			if (NewFCost < Neighbor.FCost || !Open.Contains(Neighbor))
			{
				if(bDebugCosts)
				{
					const FString& NeighborFCost = FString::Printf(TEXT("(%.*f)"), 1, NewFCost);
					DrawDebugString(GetWorld(), Neighbor.WorldLocation, *NeighborFCost, nullptr, FColor::Blue, 60.0f);
				}
				
				// Update path to Neighbor and its FCost 
				Neighbor.FCost = NewFCost;
				Neighbor.GCost = GCost;
				Neighbor.ParentLocation = Current.MapLocation;

				// Mark Neighbor as seen, we will visit it later
				if (!Open.Contains(Neighbor))
				{
					Open.HeapPush(Neighbor, FMostOptimalNode());
				}
			}
		}
	}

	TracePath(OutPath);
}

void UAStarPathFinder::ChooseHeuristic(int32 Index)
{
	HeuristicIndex = Index;
}

void UAStarPathFinder::SetAllowDiagonal(bool bValue)
{
	bAllowDiagonal = bValue;
}

void UAStarPathFinder::TracePath(TArray<FVector>& OutPath)
{
	// Set the Current MapNode as the Destination MapNode
	FMapNode& Current = Map[DestinationMapLocation];

	// If we haven't arrived at the Starting MapNode and the Current MapNode has a valid Parent (a Path really exists)
	while (Current != Map[StartMapLocation] && Map.IsValidLocation(Current.ParentLocation))
	{
		// Add this MapNode to the Path
		OutPath.Add(Current.WorldLocation);
		// Set Current to the Parent of this MapNode
		Current = Map[Current.ParentLocation];
	}
	
	if (bDebugPath)
	{
		for (int32 i = 0; i < OutPath.Num() - 1; ++i)
		{
			DrawDebugLine(GetWorld(), OutPath[i], OutPath[i + 1], FColor::Red, true, -1, 0, 20.0f);
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

void UAStarPathFinder::OnGridClear()
{
	ClearMap();
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
