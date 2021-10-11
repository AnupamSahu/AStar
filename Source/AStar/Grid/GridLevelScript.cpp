// Copyright Epic Games, Inc. All Rights Reserved.

#include "GridLevelScript.h"
#include "BlockActor.h"
#include "DrawDebugHelpers.h"

void AGridLevelScript::GenerateGrid()
{
	ClearGrid();

	FVector Location = GridOrigin;
	for (uint32 i = 0; i < SizeX; ++i)
	{
		for (uint32 j = 0; j < SizeY; ++j)
		{
			if (BlockActorClass != nullptr)
			{
				FActorSpawnParameters SpawnParameters;
				SpawnParameters.ObjectFlags |= RF_Transient;
				SpawnParameters.bHideFromSceneOutliner = true;

				ABlockActor* NewBlockActor = Cast<ABlockActor>(
					GetWorld()->SpawnActor(BlockActorClass, &Location, &FRotator::ZeroRotator, SpawnParameters));
				NewBlockActor->SetActorLocation(Location);
				NewBlockActor->UpdateBlock();

				GridRows[i].Add(NewBlockActor);
			}
			Location += FVector::RightVector * MinimumSeparation;
		}
		Location += -FVector::ForwardVector * MinimumSeparation;
		Location.Y = GridOrigin.Y;
	}

	SetMinMaxLocations();

	OnGridGenerated.Broadcast();
}

void AGridLevelScript::ClearGrid()
{
	for (int32 i = 0; i < GridRows.Num(); ++i)
	{
		TArray<ABlockActor*> GridCol = GridRows[i];
		for (int32 j = 0; j < GridCol.Num(); ++j)
		{
			if (IsValid(GridCol[j]))
			{
				GridCol[j]->Destroy();
			}
		}
	}
	
	GridRows.Init(TArray<ABlockActor*>(), SizeX);

	OnGridCleared.Broadcast();
}

void AGridLevelScript::BeginPlay()
{
	Super::BeginPlay();
}

void AGridLevelScript::GetGrid(TArray<TArray<FMapNode>>& OutGrid) const
{
	const int32 RowSize = GridRows.Num();
	const int32 ColSize = GridRows[0].Num();

	OutGrid.Empty();
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
			GridElement.WorldLocation = GridRows[i][j]->GetActorLocation();
			GridElement.MapLocation = FMapLocation(i, j);
			GridElement.bIsWalkable = GridRows[i][j]->GetIsWalkable();

			OutGrid[i][j] = GridElement;
		}
	}
}

bool AGridLevelScript::CheckGridLocation(const FMapLocation& GridLocation) const
{
	return GridRows.IsValidIndex(GridLocation.X) && GridRows[GridLocation.X].IsValidIndex(GridLocation.Y);
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
	
	OutGridLocation.X = -(RelativeWorldLocation.X / MinimumSeparation);
	OutGridLocation.Y = (RelativeWorldLocation.Y / MinimumSeparation);

	return true;
}

void AGridLevelScript::SetMinMaxLocations()
{
	MaxWorldLocation = GridOrigin + FVector::RightVector * SizeX * MinimumSeparation;
	MinWorldLocation = GridOrigin - FVector::RightVector * SizeX * MinimumSeparation - FVector::ForwardVector * SizeY * MinimumSeparation;
}

bool AGridLevelScript::CheckWorldLocation(const FVector& WorldLocation) const
{
	return WorldLocation.X >= MinWorldLocation.X && WorldLocation.Y >= MinWorldLocation.Y
		&& WorldLocation.X <= MaxWorldLocation.X && WorldLocation.Y <= MaxWorldLocation.Y;
}
