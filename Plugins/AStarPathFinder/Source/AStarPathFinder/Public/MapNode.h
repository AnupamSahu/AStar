// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

// TODO : Come back to this if you think we should add something here.
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

	// FCost getter.
	float FCost() const
	{
		// We don't really need extra storage for the FCost.
		return GCost + HCost;
	}

	// A clean node is a happy node.
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

struct FMostOptimalNode
{
	bool operator()(const FAStarGraphNode& A, const FAStarGraphNode& B) const
	{
		return A.FCost() == B.FCost() ? A.HCost < B.HCost : A.FCost() < B.FCost();
	}
};
