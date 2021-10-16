// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/BoxComponent.h"
#include "GameFramework/Actor.h"
#include "BlockActor.generated.h"

UCLASS()
class ASTAR_API ABlockActor : public AActor
{
	GENERATED_BODY()
public:
	
	// Sets default values for this actor's properties
	ABlockActor();

	// Sets the Block's material and Walkability based on, if its overlapping with other Actors
	virtual void UpdateBlock();

	// Returns true if this Block is Walkable, false otherwise
	bool GetIsWalkable() const;

	// Returns the WeightModifier value
	float GetWeightModifier() const;
	
protected:

	// Modifies the weight of the Node at this Block
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Block Settings")
	float WeightModifier = 0.0f;

	// This Material is used on a Walkable Block
	UPROPERTY(EditAnywhere)
	UMaterialInterface* WalkableMaterial = nullptr;

	// This Material is used on a Non-Walkable Block
	UPROPERTY(EditAnywhere)
	UMaterialInterface* NonWalkableMaterial = nullptr;
	
	// The Block's StaticMesh Component
	UPROPERTY(VisibleDefaultsOnly)
	UStaticMeshComponent* StaticMeshComponent = nullptr;

	// The Block's BoxCollision Component
	UPROPERTY(VisibleDefaultsOnly)
	UBoxComponent* BoxComponent = nullptr;

protected:

	// If true, this block overlap's with Blocking Actors
	bool bIsNotWalkable = false;
};
