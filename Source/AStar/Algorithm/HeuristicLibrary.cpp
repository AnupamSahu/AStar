// Copyright Epic Games, Inc. All Rights Reserved.
#include "HeuristicLibrary.h"

float HeuristicLibrary::GetManhattanDistance(const FVector& V1, const FVector& V2)
{
	return abs(V1.X - V2.X) + abs(V1.Y - V2.Y);
}

float HeuristicLibrary::GetEuclideanDistance(const FVector& V1,const FVector& V2)
{
	return FVector::Distance(V1, V2);
}

float HeuristicLibrary::GetChebyshevDistance(const FVector& V1,const FVector& V2)
{
	return FMath::Max(abs(V1.X - V2.X), abs(V1.Y - V2.Y));
}

float HeuristicLibrary::GetOctileDistance(const FVector& V1, const FVector& V2)
{
	float DelX = abs(V1.X - V2.X);
	float DelY = abs(V1.Y - V2.Y);
	return (DelX + DelY) * (UE_SQRT_2 - 2) * FMath::Min(DelX, DelY);
}
