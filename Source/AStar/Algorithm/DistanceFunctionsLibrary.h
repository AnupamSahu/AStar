// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

/**
 * The HeuristicLibrary is a collection of mathematical functions
 * that calculate the distance between two points in a different ways.
 */
class ASTAR_API DistanceFunctionsLibrary
{
public:
	// Manhattan distance is calculated as the sum of the absolute differences between the two vectors.
	static float GetManhattanDistance(const FVector& V1,const FVector& V2);

	// Euclidean distance between two points in Euclidean space
	// is the length of a line segment between the two points. 
	static float GetEuclideanDistance(const FVector& V1,const FVector& V2);

	// Chebyshev distance, is a metric defined on a vector space
	// where the distance between two vectors is the greatest of their differences along any coordinate dimension. 
	static float GetChebyshevDistance(const FVector& V1,const FVector& V2);

	// The Octile distance is used to estimate the distance between two cells heuristically.
	// The respective lengths of cardinal and diagonal moves are 1 and 1.414.
	// A matrix the same size as the map is used to store all the grid points.
	static float GetOctileDistance(const FVector& V1,const FVector& V2);
};
