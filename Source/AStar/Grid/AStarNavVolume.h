// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "BlockActor.h"
#include "AStar/Types/MapNode.h"
#include "Engine/LevelScriptActor.h"
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
class ASTAR_API AAStarNavVolume : public ALevelScriptActor
{

	GENERATED_BODY()

public:
	
	UFUNCTION(BlueprintCallable)
	virtual void GenerateGrid();
	
	UFUNCTION(BlueprintCallable)
	virtual void ResetGrid();

private:

	bool IsValidGridLocation(uint32 Row, uint32 Column) const;

	void ConvertWorldToGridLocation(const FVector& WorldLocation, int32& Row, int32& Column) const;
	
	void SetMinMaxLocations();
	
	bool CheckWorldLocation(const FVector& WorldLocation) const;

	void LinkNodes(FGridNode& TargetNode, uint32 FromRow, uint32 FromColumn);

	ABlockActor* SpawnBlockActor(const FVector& Location) const;

protected:

	// The Origin of the Grid, all Grid Blocks are placed relative to this Origin
	UPROPERTY(EditAnywhere, Category = "Grid Settings")
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
	
protected:

	// A two-dimensional array containing all Block Actors
	TArray<TArray<FGridNode>> BlockGrid;
	
private:

	FVector MinWorldLocation;

	FVector MaxWorldLocation;
	
};
