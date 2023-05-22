// Copyright Epic Games, Inc. All Rights Reserved.

#include "NavGrid.h"
#include "BlockActor.h"
#include "AStar/Algorithm/UAStarPathFinder.h"

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
	verifyf(BlockActor != nullptr, TEXT("BlockActor is not set."));

	ResetGrid();
	
	FVector Location = Origin;
	for (uint32 Row = 0; Row < Size; ++Row)
	{
		for (uint32 Column = 0; Column < Size; ++Column)
		{
			ABlockActor* NewBlockActor = SpawnBlockActor(Location);
			BlockGrid[Row].Add({NewBlockActor, FAStarGraphNode(Location)});

			FGridNode& NewNode = BlockGrid.Last().Last();
			LinkNodes(NewNode, Row, Column - 1);
			LinkNodes(NewNode, Row - 1, Column - 1);
			LinkNodes(NewNode, Row - 1, Column);
			
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

void AGridLevelScript::ConvertWorldToGridLocation(const FVector& WorldLocation, int32& Row, int32& Column) const
{
	Row = -1;
	Column = -1;

	if (!CheckWorldLocation(WorldLocation))
	{
		return;
	}

	const FVector& RelativeWorldLocation = WorldLocation - Origin;
	
	Row = (RelativeWorldLocation.X / Separation);
	Column = (RelativeWorldLocation.Y / Separation);
}

void AGridLevelScript::SetMinMaxLocations()
{
	MaxWorldLocation = Origin + FVector::RightVector * (Size - 1) * Separation + FVector::ForwardVector * (Size - 1) * Separation;
	MinWorldLocation = Origin;
}

bool AGridLevelScript::CheckWorldLocation(const FVector& WorldLocation) const
{
	return WorldLocation.X >= MinWorldLocation.X && WorldLocation.Y >= MinWorldLocation.Y
		&& WorldLocation.X <= MaxWorldLocation.X && WorldLocation.Y <= MaxWorldLocation.Y;
}