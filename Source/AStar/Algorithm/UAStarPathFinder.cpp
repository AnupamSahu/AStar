// Copyright Epic Games, Inc. All Rights Reserved.

#include "UAStarPathFinder.h"
#include "DistanceFunctionsLibrary.h"

UAStarPathFinder::UAStarPathFinder()
{
	Heuristics.Add(DistanceFunctionsLibrary::GetEuclideanDistance);
	Heuristics.Add(DistanceFunctionsLibrary::GetManhattanDistance);
	Heuristics.Add(DistanceFunctionsLibrary::GetChebyshevDistance);
	Heuristics.Add(DistanceFunctionsLibrary::GetOctileDistance);
}

void UAStarPathFinder::FindPath(FAStarGraphNode* Start, const FAStarGraphNode* Destination, TArray<const FAStarGraphNode*>& OutPath)
{
	if(!ensureMsgf(Heuristics.IsValidIndex(HeuristicIndex), TEXT("Invalid HeuristicIndex!")))
	{
		return;
	}
	
	Open.Reset();
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
			const float GCost = Heuristics[HeuristicIndex](Neighbor->Location, Start->Location);
			const float HCost = Heuristics[HeuristicIndex](Neighbor->Location, Destination->Location);

			const float NewFCost = GCost + HCost;
			
			// If New FCost is smaller than the Neighbor's current FCost, we just found a more optimal path
			const bool bIsInOpen = Open.Contains(Neighbor);
			if (NewFCost < Neighbor->FCost || !bIsInOpen)
			{
				// Update path to Neighbor and its FCost 
				Neighbor->FCost = NewFCost;
				Neighbor->GCost = GCost;
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
	ResetNodes();
}

void UAStarPathFinder::ChooseHeuristic(int32 Index)
{
	HeuristicIndex = Index;
}

void UAStarPathFinder::CreatePath(const FAStarGraphNode* Start, const FAStarGraphNode* End, TArray<const FAStarGraphNode*>& Out_Path) const
{
	Out_Path.Reset();
	
	const FAStarGraphNode* Current = End;

	if(Current == nullptr)
	{
		return;
	}

	Out_Path.Add(Current);
	
	while(Current && Current != Start)
	{
		Out_Path.Add(Current->Parent);
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
