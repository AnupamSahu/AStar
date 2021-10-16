// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

/**
 * 
 */
class ASTAR_API HeuristicLibrary
{
public:

	static float GetManhattanDistance(const FVector& V1,const FVector& V2);

	static float GetEuclideanDistance(const FVector& V1,const FVector& V2);

	static float GetChebyshevDistance(const FVector& V1,const FVector& V2);

	static float GetOctileDistance(const FVector& V1,const FVector& V2);
};
