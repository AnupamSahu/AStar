// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "AStar/Types/MapNode.h"

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
 *
 * This Component provides the shortest Path between
 * Locations on a pre-configured grid by
 * implementing the A* Algorithm.
 */
class ASTAR_API UAStarPathFinder
{
public:
	
	UAStarPathFinder();
	
	/** Provides an array of MapNodes, given a StartLocation and a DestinationLocation in World Coordinates.
	 *  If either StartLocation or DestinationLocation are outside the Map's bounds, an empty path is provided.
	 */
	void FindPath(FAStarGraphNode* Start, const FAStarGraphNode* Destination, TArray<const FAStarGraphNode*>& OutPath);
	
	// Selects a Heuristic Function identified by an Index
	void ChooseHeuristicFunction(const EHeuristic Choice);

protected:

	void CreatePath(const FAStarGraphNode* Start, const FAStarGraphNode* End, TArray<const FAStarGraphNode*>& Out_Path) const;

	void ResetNodes();

protected:
	
	TMap<EHeuristic, TFunction<float(const FVector&, const FVector&)>> HeuristicsMap;

	TFunction<float(const FVector&, const FVector&)> HeuristicFunction;
	
	// An array of Elements that have not been visited yet
	TArray<FAStarGraphNode*> Open;

	// An array of Elements that have been visited
	TSet<FAStarGraphNode*> Closed;
};
