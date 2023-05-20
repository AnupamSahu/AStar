// Copyright Epic Games, Inc. All Rights Reserved.

#include "NavGrid.h"
#include "BlockActor.h"
#include "AStar/Algorithm/UAStarPathFinder.h"

void AGridLevelScript::TracePath(const ABlockActor* Start, const ABlockActor* Destination)
{
	FGridNode* StartNode = nullptr;
	for(TArray<FGridNode>& Row : BlockGrid)
	{
		StartNode = Row.FindByPredicate([Start](const FGridNode& Other){ return Other.Block == Start;});
		if(StartNode)
		{
			break;
		}
	}

	const FGridNode* DestinationNode = nullptr;
	for(const TArray<FGridNode>& Row : BlockGrid)
	{
		DestinationNode = Row.FindByPredicate([Start](const FGridNode& Other){ return Other.Block == Start;});
		if(DestinationNode)
		{
			break;
		}
	}


	if(StartNode && DestinationNode)
	{
		TArray<const FAStarGraphNode*> Path;
		
		UAStarPathFinder PathFinder;
		PathFinder.FindPath(&StartNode->AStarNode, &DestinationNode->AStarNode, Path);
	}
}

ABlockActor* AGridLevelScript::SpawnBlockActor(const FVector& Location) const
{
	FActorSpawnParameters SpawnParameters;
	SpawnParameters.ObjectFlags |= RF_Transient;

#if UE_EDITOR
	SpawnParameters.bHideFromSceneOutliner = true;
#endif

	ABlockActor* NewBlockActor = Cast<ABlockActor>(GetWorld()->SpawnActor(BlockActor, &Location, &FRotator::ZeroRotator, SpawnParameters));
	NewBlockActor->SetActorLocation(Location);
	NewBlockActor->UpdateBlock();

	return NewBlockActor;
}

void AGridLevelScript::GenerateGrid()
{
	ResetGrid();
	
	FVector Location = Origin;
	for (uint32 Row = 0; Row < Size; ++Row)
	{
		for (uint32 Column = 0; Column < Size; ++Column)
		{
			if (BlockActor != nullptr)
			{
				ABlockActor* NewBlockActor = SpawnBlockActor(Location);
				BlockGrid[Row].Add({NewBlockActor, FAStarGraphNode(Location)});

				FGridNode& NewNode = BlockGrid.Last().Last();
				LinkNodes(NewNode, Row, Column - 1);
				LinkNodes(NewNode, Row - 1, Column - 1);
				LinkNodes(NewNode, Row - 1, Column);
			}
			Location += FVector::RightVector * Separation;
		}
		Location += FVector::ForwardVector * Separation;
		Location.Y = Origin.Y;
	}
}

void AGridLevelScript::LinkNodes(FGridNode& TargetNode, const uint32 FromRow, const uint32 FromColumn)
{
	if(IsValidGridLocation(FromRow, FromColumn))
	{
		TargetNode.AStarNode.AdjacentNodes.Add(&BlockGrid[FromRow][FromColumn].AStarNode);
	}
}

void AGridLevelScript::ResetGrid()
{
	for (const TArray<FGridNode>& Row : BlockGrid)
	{
		for (const FGridNode& Node : Row)
		{
			if (IsValid(Node.Block))
			{
				Node.Block->Destroy();
			}
		}
	}

	BlockGrid.Init(TArray<FGridNode>(), Size);
}

bool AGridLevelScript::IsValidGridLocation(const uint32 Row, const uint32 Column) const
{
	return BlockGrid.IsValidIndex(Row) && BlockGrid.Num() > 0 && BlockGrid[0].IsValidIndex(Column);
}