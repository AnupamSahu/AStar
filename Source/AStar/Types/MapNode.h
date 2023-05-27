// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

struct FGraphNode
{
	TArray<FGraphNode*> AdjacentNodes;

	virtual ~FGraphNode() = default;
};

struct FAStarGraphNode : FGraphNode
{
	FAStarGraphNode() = default;
	
	FAStarGraphNode(const FVector& WorldLocation)
		: Location(WorldLocation)
	{}

	bool operator==(const FAStarGraphNode& Other) const
	{
		return Location == Other.Location && AdjacentNodes == Other.AdjacentNodes;
	}

	float FCost() const
	{
		return GCost + HCost;
	}
	
	void Reset()
	{
		GCost = BIG_NUMBER;
		HCost = BIG_NUMBER;
		Parent = nullptr;
	}

	FVector Location = FVector::ZeroVector;

	float GCost = BIG_NUMBER;
	
	float HCost = BIG_NUMBER;

	bool bIsWalkable = true;

	FAStarGraphNode* Parent = nullptr;
};

// TODO : Rename to FMostOptimalNode
struct FMostOptimalNode
{
	bool operator()(const FAStarGraphNode& A, const FAStarGraphNode& B) const
	{
		return A.FCost() == B.FCost() ? A.HCost < B.HCost : A.FCost() < B.FCost();
	}
};
