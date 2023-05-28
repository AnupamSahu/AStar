// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "BlockActor.h"
#include "MapNode.h"
#include "UAStarPathFinder.h"
#include "AStarNavVolume.generated.h"

// Forward Declarations
class ABlockActor;

// A block on the grid contains information about its block and the AStar node that it represents.
struct FGridNode
{
	ABlockActor* Block;

	FAStarGraphNode AStarNode;
};

/**
 * A simple implementation of a navigation volume that
 * spawns collision detection boxes in a user specified
 * region, in a grid like arrangement, to map walkable places.
 *
 * This class also serves as a good example of how the UAStarPathFinder class is used.
 */
UCLASS()
class ASTAR_API AAStarNavVolume : public AActor
{

	GENERATED_BODY()

public:

	// Spawn collision detection blocks and map blocked and walkable areas.
	UFUNCTION(BlueprintCallable)
	virtual void GenerateGrid();

	// Use AStar Pathfinding to find the shortest path between Start and Destination locations if they are valid walkable locations.
	UFUNCTION(BlueprintCallable)
	void FindPath(const FVector& Start, const FVector& Destination);

	// Select a heuristic function that defines the distance between two points.
	UFUNCTION(BlueprintCallable)
	void ChooseHeuristicFunction(const EHeuristic Choice);
	
private:

	// Destroy all nodes.
	virtual void ResetGrid();

	// Checks if give row, column pair is a valid location in the grid.
	bool IsValidGridLocation(uint32 Row, uint32 Column) const;

	// Map a World Location to a location in the grid.
	bool ConvertWorldToGridLocation(const FVector& WorldLocation, int32& Row, int32& Column) const;

	// Define bounds to check the validity of grid locations.
	void SetMinMaxLocations();

	// Check if a World Location is associated to valid node in the grid.
	bool CheckWorldLocation(const FVector& WorldLocation) const;

	// Link AStar nodes to their neighbors.
	void CreateLinks();

	// Link One node to a specific neighbor identified by its row and column values.
	void LinkNodes(FGridNode& TargetNode, uint32 FromRow, uint32 FromColumn);

	// TODO : Do we really need actors?
	// Spawn a collision detection box at a specific location.
	ABlockActor* SpawnBlockActor(const FVector& Location) const;

private:

	// The Origin of the Grid, all Grid Blocks are placed relative to this Origin
	UPROPERTY(EditAnywhere, Category = "Nav Volume", Meta = (MakeEditWidget = true))
	FVector Origin;
	
	// Number of Blocks Along X Axis
	UPROPERTY(EditAnywhere, Category = "Nav Volume")
	uint32 Size;

	// Defines the minimum separation between two blocks
	UPROPERTY(EditAnywhere, Category = "Nav Volume")
	float Separation = 100.0f;
	
	// An Actor that represents a Block
	UPROPERTY(EditAnywhere, Category = "Nav Volume")
	TSubclassOf<ABlockActor> BlockActor;
	
	// A two-dimensional array containing all Block Actors (Collision detection boxes)
	TArray<TArray<FGridNode>> BlockGrid;

	// Bounds of the map.
	FVector MinWorldLocation, MaxWorldLocation;

	// AStar Pathfinder object.
	UAStarPathFinder PathFinder;
	
};
