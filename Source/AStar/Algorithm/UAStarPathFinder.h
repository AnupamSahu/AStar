// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "AStar/Types/MapNode.h"
#include "UAStarPathFinder.generated.h"

// Forward Declarations
class AGridLevelScript;

// Encapsulates a Two-Dimensional array of MapNodes
struct FNavMap
{

	// Index a Node on the Map with its MapLocation
	FMapNode& operator[](const FMapLocation& MapLocation)
	{
		return MapBase[MapLocation.X][MapLocation.Y];
	}

	// Checks the validity of a MapLocation
	bool IsValidLocation(const FMapLocation& MapLocation) const
	{
		return MapBase.IsValidIndex(MapLocation.X) && MapBase[MapLocation.X].IsValidIndex(MapLocation.Y);
	}

	void ResetFCosts()
	{
		for(int32 i=0; i<MapBase.Num(); ++i)
		{
			for(int32 j=0; j<MapBase[i].Num(); ++j)
			{
				FMapNode& MapNode = MapBase[i][j];
				MapNode.FCost = BIG_NUMBER;
				MapNode.GCost = BIG_NUMBER;
				MapNode.ParentLocation = {};
			}
		}
	}
	
	// The base Two-Dimensional array of MapNodes on which the FNavMap is built
	TArray<TArray<FMapNode>> MapBase;
	
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
UCLASS(ClassGroup = Navigation, meta = (BlueprintSpawnableComponent))
class ASTAR_API UAStarPathFinder : public UActorComponent
{
public:
	
	GENERATED_BODY()

	UAStarPathFinder();
	
	// Finds a GridLevelScript
	virtual void BeginPlay() override;

	/** Provides an array of MapNodes, given a StartLocation and a DestinationLocation in World Coordinates.
	 *  If either StartLocation or DestinationLocation are outside the Map's bounds, an empty path is provided.
	 */
	UFUNCTION(BlueprintCallable)
	virtual void FindPath(const FVector& StartLocation, const FVector& DestinationLocation, TArray<FVector>& OutPath);
	
	// Selects a Heuristic Function identified by an Index
	UFUNCTION(BlueprintCallable)
	void ChooseHeuristic(int32 Index);

	// Selects a Heuristic Function identified by an Index
	UFUNCTION(BlueprintCallable)
	void SetAllowDiagonal(bool bValue);
	
protected:

	// Fetches an Array of MapNodes from the GridLevelScript
	void UpdateMap();

	// Empties the Map and flushes Debug Strings / Lines
	void ClearMap();

	// Calls UpdateMap()
	UFUNCTION()
	void OnGridUpdate();

	// Calls ClearMap()
	UFUNCTION()
	void OnGridClear();

	// Finds MapNodes Neighboring a given MapNode
	virtual void GetNeighbors(const FMapLocation& Target, TArray<FMapLocation>& OutNeighborLocations);

	// Performs some initialization before running the Path Finder
	bool Initialize(const FVector& StartLocation, const FVector& DestinationLocation, TArray<FVector>& OutPath);

	// Starts from Destination, backtracks to Start, filling OutPath with the Nodes travelled 
	void TracePath(TArray<FVector>& OutPath);
	
protected:

	// If true, 8 Neighbors are scanned instead of 4
	bool bAllowDiagonal = false;

	// If true, the Path is drawn on Screen
	UPROPERTY(EditAnywhere, Category = "Debug Settings")
	bool bDebugPath = true;

	// If true, visited MapNode display their FCosts
	UPROPERTY(EditAnywhere, Category = "Debug Settings")
	bool bDebugCosts = false;
	
protected:
	
	// A reference to a GridLevelActor present in the Scene
	UPROPERTY(Transient)
	AGridLevelScript* GridLevelActor;

	// An array containing Heuristic Functions
	TArray<TFunction<float(const FVector&,const FVector&)>> Heuristics;

	// Index of the Heuristic Function currently chosen
	uint32 HeuristicIndex = 0;
	
	// A Two-Dimensional array containing MapNodes created by the GridLevelActor
	FNavMap Map;

	// An array of Elements that have not been visited yet
	TArray<FMapNode> Open;

	// An array of Elements that have been visited
	TArray<FMapNode> Closed;

	// Map Location of Starting MapNode
	FMapLocation StartMapLocation;

	// Map Location of Destination MapNode
	FMapLocation DestinationMapLocation;

	// An array containing the Indices of all 8 Neighbors relative to indices (0, 0)
	TArray<FMapLocation> NeighborIndices =
	{
		{ 0,  1}, // North
		{ 1,  0}, // East
		{ 0, -1}, // South
		{-1,  0}, // West
		{-1,  1}, // North-West
		{ 1,  1}, // North-East
		{ 1, -1}, // South-East
		{-1, -1}, // South-West
	};
};
