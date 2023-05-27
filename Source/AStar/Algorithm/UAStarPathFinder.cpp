// Copyright Epic Games, Inc. All Rights Reserved.

#include "UAStarPathFinder.h"
#include "DistanceFunctionsLibrary.h"

UAStarPathFinder::UAStarPathFinder()
{
	HeuristicsMap.Add(EHeuristic::Euclidean, DistanceFunctionsLibrary::GetEuclideanDistance);
	HeuristicsMap.Add(EHeuristic::Manhattan, DistanceFunctionsLibrary::GetManhattanDistance);
	HeuristicsMap.Add(EHeuristic::Chebyshev, DistanceFunctionsLibrary::GetChebyshevDistance);
	HeuristicsMap.Add(EHeuristic::Octile, DistanceFunctionsLibrary::GetOctileDistance);
	
	HeuristicFunction = HeuristicsMap[EHeuristic::Euclidean];
}

void UAStarPathFinder::FindPath(FAStarGraphNode* Start, const FAStarGraphNode* Destination, TArray<const FAStarGraphNode*>& OutPath)
{
	if(!Start->bIsWalkable || !Destination->bIsWalkable)
	{
		return;
	}

	ResetNodes();
	Open.Reset();
	Closed.Reset();

	Start->GCost = 0.0f;
	Start->HCost = HeuristicFunction(Start->Location, Destination->Location);
	Start->Parent = nullptr;
	
	Open.Push(Start);
	
	// While there are still MapNodes to be visited
	while (Open.Num() > 0)
	{
		// Current MapNode is now known to be visited
		FAStarGraphNode* Current;
		Open.HeapPop(Current, FMostOptimalNode());
		Closed.Add(Current);
		
		// If we are already at destination, just break
		if (Current == Destination)
		{
			break;
		}

		// For each Neighbor
		for (FGraphNode* GraphNode : Current->AdjacentNodes)
		{
			FAStarGraphNode* Neighbor = static_cast<FAStarGraphNode*>(GraphNode);
		
			// If Neighbor is already visited or not walkable (wall or obstacle), reject it and continue
			if (Closed.Contains(Neighbor) || !Neighbor->bIsWalkable)
			{
				continue;
			}
			
			// Calculate the Movement, Heuristic and The FCost
			const float GCost = Current->GCost + HeuristicFunction(Neighbor->Location, Current->Location);
			
			// If New FCost is smaller than the Neighbor's current FCost, we just found a more optimal path
			const bool bIsInOpen = Open.Contains(Neighbor);
			if (GCost < Neighbor->GCost || !bIsInOpen)
			{
				// Update path to Neighbor and its FCost 
				Neighbor->GCost = GCost;
				Neighbor->HCost = HeuristicFunction(Neighbor->Location, Destination->Location);
				Neighbor->Parent = Current;
				
				// Mark Neighbor as seen, we will visit it later
				if (!bIsInOpen)
				{
					Open.HeapPush(Neighbor, FMostOptimalNode());
				}
			}
		}
	}

	CreatePath(Start, Destination, OutPath);
}

void UAStarPathFinder::ChooseHeuristicFunction(const EHeuristic Choice)
{
	if(!HeuristicsMap.Contains(Choice))
	{
		return;
	}
	HeuristicFunction = HeuristicsMap[Choice];
}

void UAStarPathFinder::CreatePath(const FAStarGraphNode* Start, const FAStarGraphNode* End, TArray<const FAStarGraphNode*>& Out_Path) const
{
	const FAStarGraphNode* Current = End;
	
	Out_Path.Reset();
	
	while(Current)
	{
		Out_Path.Add(Current);
		Current = Current->Parent;
	}

	Algo::Reverse(Out_Path);
}

void UAStarPathFinder::ResetNodes()
{
	for (FAStarGraphNode* Node : Closed)
	{
		Node->Reset();
	}
}
