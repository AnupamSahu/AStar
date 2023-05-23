// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "BlockActor.h"
#include "AStar/Algorithm/UAStarPathFinder.h"
#include "AStar/Types/MapNode.h"
#include "AStarNavVolume.generated.h"

// Forward Declarations
class ABlockActor;

struct FGridNode
{
	ABlockActor* Block;

	FAStarGraphNode AStarNode;
};

/**
 * Grid Level Script offers the user to
 * Generate a grid of Actors that represent Walkable or Blocked zones.
 * The grid acts like a navigation region for AStar Pathfinding.
 */
UCLASS()
class ASTAR_API AAStarNavVolume : public AActor
{

	GENERATED_BODY()

public:
	
	UFUNCTION(BlueprintCallable)
	virtual void GenerateGrid();

	UFUNCTION(BlueprintCallable)
	void FindPath(const FVector& Start, const FVector& Destination);
	
private:

	virtual void ResetGrid();

	bool IsValidGridLocation(uint32 Row, uint32 Column) const;

	bool ConvertWorldToGridLocation(const FVector& WorldLocation, int32& Row, int32& Column) const;
	
	void SetMinMaxLocations();
	
	bool CheckWorldLocation(const FVector& WorldLocation) const;

	void CreateLinks();
	
	void LinkNodes(FGridNode& TargetNode, uint32 FromRow, uint32 FromColumn);

	ABlockActor* SpawnBlockActor(const FVector& Location) const;

private:

	// The Origin of the Grid, all Grid Blocks are placed relative to this Origin
	UPROPERTY(EditAnywhere, Category = "Grid Settings", Meta = (MakeEditWidget = true))
	FVector Origin;
	
	// Number of Blocks Along X Axis
	UPROPERTY(EditAnywhere, Category = "Grid Settings")
	uint32 Size;

	// Defines the minimum separation between two blocks
	UPROPERTY(EditAnywhere, Category = "Grid Settings")
	float Separation = 100.0f;
	
	// An Actor that represents a Block
	UPROPERTY(EditAnywhere, Category = "Grid Settings")
	TSubclassOf<ABlockActor> BlockActor;
	
	// A two-dimensional array containing all Block Actors
	TArray<TArray<FGridNode>> BlockGrid;

	
	FVector MinWorldLocation, MaxWorldLocation;

	
	UAStarPathFinder PathFinder;
	
};
