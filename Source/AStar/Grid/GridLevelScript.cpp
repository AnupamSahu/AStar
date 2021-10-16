// Copyright Epic Games, Inc. All Rights Reserved.

#include "GridLevelScript.h"
#include "BlockActor.h"

void AGridLevelScript::GenerateGrid()
{
	ClearGrid();
	InitGrid();

	FVector Location = GridOrigin;
	for (uint32 i = 0; i < SizeX; ++i)
	{
		for (uint32 j = 0; j < SizeY; ++j)
		{
			if (BlockActorClass != nullptr)
			{
				FActorSpawnParameters SpawnParameters;
				SpawnParameters.ObjectFlags |= RF_Transient;

#if UE_EDITOR
				SpawnParameters.bHideFromSceneOutliner = true;
#endif
				
				ABlockActor* NewBlockActor = Cast<ABlockActor>(GetWorld()->SpawnActor(BlockActorClass, &Location, &FRotator::ZeroRotator, SpawnParameters));
				NewBlockActor->SetActorLocation(Location);
				NewBlockActor->UpdateBlock();

				Grid[i].Add(NewBlockActor);
			}
			Location += FVector::RightVector * MinimumSeparation;
		}
		Location += FVector::ForwardVector * MinimumSeparation;
		Location.Y = GridOrigin.Y;
	}

	SetMinMaxLocations();

	OnGridGenerated.Broadcast();
}

void AGridLevelScript::ClearGrid()
{
	for (int32 i = 0; i < Grid.Num(); ++i)
	{
		TArray<ABlockActor*>& GridCol = Grid[i];
		for (int32 j = 0; j < GridCol.Num(); ++j)
		{
			if (IsValid(GridCol[j]))
			{
				GridCol[j]->Destroy();
			}
		}
	}

	Grid.Init(TArray<ABlockActor*>(), 0);
	
	OnGridCleared.Broadcast();
}

void AGridLevelScript::InitGrid()
{
	Grid.Init(TArray<ABlockActor*>(), SizeX);
}

void AGridLevelScript::BeginPlay()
{
	Super::BeginPlay();
}

void AGridLevelScript::GetGrid(TArray<TArray<FMapNode>>& OutGrid) const
{
	OutGrid.Empty();
	
	const int32 RowSize = Grid.Num();
	if(RowSize == 0)
	{
		return;
	}

	const int32 ColSize = Grid[0].Num();

	OutGrid.Init(TArray<FMapNode>(), RowSize);
	for(int32 i=0; i<OutGrid.Num(); ++i)
	{
		OutGrid[i].Init(FMapNode(), ColSize);
	}
	
	for (int32 i = 0; i < RowSize; ++i)
	{
		for (int32 j = 0; j < ColSize; ++j)
		{
			FMapNode GridElement;
			ABlockActor* BlockActor = Grid[i][j];
			
			GridElement.MapLocation = FMapLocation(i, j);
			GridElement.WorldLocation = BlockActor->GetActorLocation();
			GridElement.bIsWalkable = BlockActor->GetIsWalkable();
			GridElement.WeightModifier = BlockActor->GetWeightModifier();

			OutGrid[i][j] = GridElement;
		}
	}
}

bool AGridLevelScript::CheckGridLocation(const FMapLocation& GridLocation) const
{
	return Grid.IsValidIndex(GridLocation.X) && Grid[GridLocation.X].IsValidIndex(GridLocation.Y);
}

bool AGridLevelScript::ConvertWorldToGridLocation(const FVector& WorldLocation, FMapLocation& OutGridLocation) const
{
	OutGridLocation.X = -1;
	OutGridLocation.Y = -1;

	if (!CheckWorldLocation(WorldLocation))
	{
		return false;
	}

	const FVector& RelativeWorldLocation = WorldLocation - GridOrigin;
	
	OutGridLocation.X = (RelativeWorldLocation.X / MinimumSeparation);
	OutGridLocation.Y = (RelativeWorldLocation.Y / MinimumSeparation);

	return true;
}

void AGridLevelScript::SetMinMaxLocations()
{
	MaxWorldLocation = GridOrigin + FVector::RightVector * (SizeX-1) * MinimumSeparation + FVector::ForwardVector * (SizeY-1) * MinimumSeparation;
	MinWorldLocation = GridOrigin;
}

bool AGridLevelScript::CheckWorldLocation(const FVector& WorldLocation) const
{
	return WorldLocation.X >= MinWorldLocation.X && WorldLocation.Y >= MinWorldLocation.Y
		&& WorldLocation.X <= MaxWorldLocation.X && WorldLocation.Y <= MaxWorldLocation.Y;
}
