// Copyright Epic Games, Inc. All Rights Reserved.

#include "AStarNavVolume.h"
#include "BlockActor.h"
#include "AStar/Algorithm/UAStarPathFinder.h"
#include "Kismet/KismetSystemLibrary.h"

void AAStarNavVolume::GenerateGrid()
{
	verifyf(BlockActor != nullptr, TEXT("BlockActor is not set."));

	ResetGrid();
	
	FVector Location = Origin;
	for (uint32 Row = 0; Row < Size; ++Row)
	{
		BlockGrid.Add(TArray<FGridNode>());
		for (uint32 Column = 0; Column < Size; ++Column)
		{
			ABlockActor* NewBlockActor = SpawnBlockActor(Location);
			BlockGrid[Row].Add({NewBlockActor, FAStarGraphNode(Location)});
			BlockGrid[Row].Last().AStarNode.bIsWalkable = NewBlockActor->GetIsWalkable();
			Location += FVector::RightVector * Separation;
		}

		Location += FVector::ForwardVector * Separation;
		Location.Y = Origin.Y;
	}

	CreateLinks();
	SetMinMaxLocations();
}

void AAStarNavVolume::CreateLinks()
{
	for(uint32 Row = 0; Row < Size; ++Row)
	{
		for(uint32 Column = 0; Column < Size; ++ Column)
		{
			FGridNode& Node = BlockGrid[Row][Column];
			LinkNodes(Node, Row, Column + 1);
			LinkNodes(Node, Row + 1, Column);
			LinkNodes(Node, Row, Column - 1);
			LinkNodes(Node, Row - 1, Column);
			LinkNodes(Node, Row - 1, Column + 1);
			LinkNodes(Node, Row + 1, Column + 1);
			LinkNodes(Node, Row + 1, Column - 1);
			LinkNodes(Node, Row - 1, Column - 1);
		}
	}
}

void AAStarNavVolume::TracePath(const FVector& Start, const FVector& Destination)
{
	FAStarGraphNode* StartNode = nullptr;
	const FAStarGraphNode* DestinationNode = nullptr;
	
	int32 Row, Column;
	if(ConvertWorldToGridLocation(Start, Row, Column))
	{
		StartNode = &BlockGrid[Row][Column].AStarNode;
	}
	
	if(ConvertWorldToGridLocation(Destination, Row, Column))
	{
		DestinationNode = &BlockGrid[Row][Column].AStarNode;
	}

	if(StartNode && DestinationNode)
	{
		TArray<const FAStarGraphNode*> Path;
		PathFinder.FindPath(StartNode, DestinationNode, Path);

		if(Path.Num() > 1)
		{
			const UWorld* World = GetWorld();

			UKismetSystemLibrary::FlushPersistentDebugLines(World);
			for(int32 Index = 0; Index < Path.Num() - 1; ++Index)
			{
				DrawDebugLine(World, Path[Index]->Location, Path[Index + 1]->Location, FColor::Red, true, -1, 0, 50.0f);
			}
		}
	}
}

void AAStarNavVolume::LinkNodes(FGridNode& TargetNode, const uint32 FromRow, const uint32 FromColumn)
{
	if(IsValidGridLocation(FromRow, FromColumn))
	{
		TargetNode.AStarNode.AdjacentNodes.Add(&BlockGrid[FromRow][FromColumn].AStarNode);
	}
}

ABlockActor* AAStarNavVolume::SpawnBlockActor(const FVector& Location) const
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

void AAStarNavVolume::ResetGrid()
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
}

bool AAStarNavVolume::IsValidGridLocation(const uint32 Row, const uint32 Column) const
{
	return BlockGrid.IsValidIndex(Row) && BlockGrid[Row].IsValidIndex(Column);
}

bool AAStarNavVolume::CheckWorldLocation(const FVector& WorldLocation) const
{
	return WorldLocation.X >= MinWorldLocation.X && WorldLocation.Y >= MinWorldLocation.Y
		&& WorldLocation.X <= MaxWorldLocation.X && WorldLocation.Y <= MaxWorldLocation.Y;
}

bool AAStarNavVolume::ConvertWorldToGridLocation(const FVector& WorldLocation, int32& Row, int32& Column) const
{
	Row = -1;
	Column = -1;

	if (!CheckWorldLocation(WorldLocation))
	{
		return false;
	}

	const FVector& RelativeWorldLocation = WorldLocation - Origin;
	
	Row = (RelativeWorldLocation.X / Separation);
	Column = (RelativeWorldLocation.Y / Separation);

	return true;
}

void AAStarNavVolume::SetMinMaxLocations()
{
	MaxWorldLocation = Origin + FVector::RightVector * (Size - 1) * Separation + FVector::ForwardVector * (Size - 1) * Separation;
	MinWorldLocation = Origin;
}