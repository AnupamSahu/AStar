// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "AStar/Types/MapNode.h"
#include "Engine/LevelScriptActor.h"
#include "GridLevelScript.generated.h"

// Forward Declarations
class ABlockActor;

/**
 * Grid Level Script offers the user to
 * Generate a grid of Actors that represent Walkable or Blocked zones.
 * The grid acts like a navigation region for AStar Pathfinding.
 */
UCLASS()
class ASTAR_API AGridLevelScript : public ALevelScriptActor
{
	
	GENERATED_BODY()
	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnGridGenerated);
	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnGridCleared);
	
public:

	AGridLevelScript();
	
	// Provides a two-dimensional Array of FGridElement, a structure that contains information about an element in the Grid 
	virtual void GetGrid(TArray<TArray<FMapNode>>& OutGrid) const;

	// Given a GridLocation (pair of Grid Indices), returns the Validity of this location
	virtual bool CheckGridLocation(const FMapLocation& GridLocation) const;

	// Converts a World Location to a Location on the Grid, returns true if the World Location lies within the bounds of the Grid  
	virtual bool ConvertWorldToGridLocation(const FVector& WorldLocation, FMapLocation& OutGridLocation) const;

	// Used for debugging
	virtual void Tick(float DeltaSeconds) override;

public:
	
	// Called when the Grid is generate
	FOnGridGenerated OnGridGenerated;

	// Called when the Grid is cleared
	FOnGridCleared OnGridCleared;
	
protected:

	// Sets the Minimum and Maximum World Locations of the Grid's bounds  
	virtual void SetMinMaxLocations();

	// Returns true if the given World Location lies within the Min and Max World Locations of the Grid, false otherwise
	virtual bool CheckWorldLocation(const FVector& WorldLocation) const;
	
	UFUNCTION(BlueprintCallable)
	// Generates Blocks in a Grid-like arrangement 
	virtual void GenerateGrid();

	UFUNCTION(BlueprintCallable)
	// Destroys all Blocks
	virtual void ClearGrid();

	// Minor Initializations
	virtual void BeginPlay() override;

protected:

	// The Origin of the Grid, all Grid Blocks are placed relative to this Origin
	UPROPERTY(EditAnywhere, Category = "Grid Settings")
	FVector GridOrigin;
	
	// Number of Blocks Along X Axis
	UPROPERTY(EditAnywhere, Category = "Grid Settings")
	uint32 SizeX;

	// Number of Blocks Along Y Axis
	UPROPERTY(EditAnywhere, Category = "Grid Settings")
	uint32 SizeY;

	// Defines the minimum separation between two blocks
	UPROPERTY(EditAnywhere, Category = "Grid Settings")
	float MinimumSeparation = 100.0f;
	
	// An Actor that represents a Block
	UPROPERTY(EditAnywhere, Category = "Grid Settings")
	TSubclassOf<ABlockActor> BlockActorClass;

	// If true, Debug information is drawn on the Screen
	UPROPERTY(EditAnywhere, Category = "Grid Settings")
	bool bDebug = false;
	
protected:

	// A two-dimensional array containing all Block Actors
	TArray<TArray<ABlockActor*>> GridRows;

	// Stores the Minimum Location of the Grid's Bounds
	FVector MinWorldLocation;

	// Stores the Maximum Location of the Grid's Bounds
	FVector MaxWorldLocation;
	
};
