// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/BoxComponent.h"
#include "GameFramework/Actor.h"
#include "NavVolumeModifier.generated.h"

UCLASS()
class ASTAR_API ANavVolumeModifier : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ANavVolumeModifier()
		:Penalty(0.0f)
	{
		PrimaryActorTick.bCanEverTick = false;

		BoxComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxComponent"));
		BoxComponent->SetCollisionProfileName(TEXT("OverlapAll"));
		SetRootComponent(BoxComponent);
	}

	float GetPenalty() const
	{
		return Penalty;
	}

private:

	UPROPERTY(EditAnywhere, Category = "Modifier Settings", meta = (ClampMin = 0.0, UIMin = 0.0, ClampMax = 1.0, UIMax = 1.0))
	float Penalty;

	UPROPERTY(EditAnywhere, Category = "Modifier Settings")
	UBoxComponent* BoxComponent;
	
};
