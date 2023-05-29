// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "MapNode.h"

UENUM(BlueprintType)
enum class EHeuristic : uint8
{
	Euclidean = 0,
	Manhattan = 1,
	Chebyshev = 2,
	Octile = 3
};

/**
 * The A* Algorithm is an advanced BFS algorithm
 * that searches for shorter paths first rather than the longer paths.
 * It is optimal as well as a complete algorithm.
 */
class ASTARPATHFINDER_API UAStarPathFinder
{
public:
	
	// Initialise Heuristic Functions.
	UAStarPathFinder();
	
	// Find a path between Start and Destination node and output the result in an array of FAStarGraphNodes. 
	void FindPath(FAStarGraphNode* Start, const FAStarGraphNode* Destination, TArray<const FAStarGraphNode*>& OutPath);
	
	// Select a Heuristic Function
	void ChooseHeuristicFunction(const EHeuristic Choice);

protected:

	// Join AStar Nodes by linking their parent nodes to form a chain that represents the path between Start and End node. 
	void CreatePath(const FAStarGraphNode* Start, const FAStarGraphNode* Destination, TArray<const FAStarGraphNode*>& Out_Path) const;

	// Clear all pathfinding data and reset the graph to its original state.
	void ResetGraph();

protected:

	// Mapping of Heuristic functions.
	TMap<EHeuristic, TFunction<float(const FVector&, const FVector&)>> HeuristicsMap;

	// The heuristic function being used to calculate distances between two points.
	TFunction<float(const FVector&, const FVector&)> HeuristicFunction;

	// An array of nodes that have not been visited yet.
	TArray<FAStarGraphNode*> Open;

	// A set of nodes that have already been visited.
	TSet<FAStarGraphNode*> Closed;
};
