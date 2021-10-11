// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "MapNode.generated.h"

// An encapsulation of a Grid Location in terms of indices.
struct FMapLocation
{
	FMapLocation() = default;
	FMapLocation(int32 XIndex, int32 YIndex) : X(XIndex), Y(YIndex) {}

	FMapLocation operator+(const FMapLocation& Other) const
	{
		return FMapLocation(Other.X + X, Other.Y + Y);
	}
	
	int32 X = -1;
	int32 Y = -1;
};

/**
 * Contains information of an Element in the Grid,
 * namely, World Location, Index Positions in the Grid,
 * Walkability, and its FCost.
 */
USTRUCT(BlueprintType)
struct ASTAR_API FMapNode
{
	
	GENERATED_BODY()

	// Compares two FGridElements for equality by comparing their GridLocations
	bool operator==(const FMapNode& Other) const
	{
		return MapLocation.X == Other.MapLocation.X && MapLocation.Y == Other.MapLocation.Y; 
	}

	bool operator!=(const FMapNode& Other) const
	{
		return !operator==(Other);
	}
	
	// Cost of the Shortest Path to this Grid Element = Movement cost + Heuristic Cost 
	float FCost = BIG_NUMBER;

	// Cost to Move from the Starting GridElement to this GridElement 
	float GCost = BIG_NUMBER;

	// World Location of this Grid Element
	FVector WorldLocation = FVector::ZeroVector;

	// Location of this Grid Element on the Grid
	FMapLocation MapLocation;

	// Location of this Grid Element's Parent, on the Grid
	FMapLocation ParentLocation;

	// If True this Element is Walkable, false otherwise
	bool bIsWalkable = false;

	// If True this Element is the Destination
	bool bIsTarget = false;
	
};

struct FMostOptimalNode
{
	bool operator()(const FMapNode& A, const FMapNode& B) const
	{
		return A.FCost == B.FCost ? A.GCost < B.GCost : A.FCost < B.FCost;
	}
};
