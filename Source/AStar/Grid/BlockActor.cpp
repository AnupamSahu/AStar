// Copyright Epic Games, Inc. All Rights Reserved.


#include "BlockActor.h"

#include "Kismet/KismetSystemLibrary.h"

// Sets default values
ABlockActor::ABlockActor()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	USceneComponent* SceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("SceneComponent"));
	SetRootComponent(SceneComponent);

	BoxComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxComponent"));
	BoxComponent->SetupAttachment(SceneComponent);
	BoxComponent->SetGenerateOverlapEvents(true);
	BoxComponent->SetCollisionProfileName(TEXT("OverlapAllDynamic"), true);

	StaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMeshComponent"));
	StaticMeshComponent->SetupAttachment(BoxComponent);
	StaticMeshComponent->SetCollisionProfileName(TEXT("NoCollision"));
}

void ABlockActor::UpdateBlock()
{
	if (BoxComponent != nullptr)
	{
		TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
		ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_WorldStatic));

		TArray<AActor*> IgnoreActors;
		IgnoreActors.Add(this);

		TArray<AActor*> HitActors;

		bIsNotWalkable = UKismetSystemLibrary::BoxOverlapActors(GetWorld(), GetActorLocation(),
		                                                          BoxComponent->GetScaledBoxExtent(), ObjectTypes,
		                                                          AActor::StaticClass(), IgnoreActors, HitActors);
		
		if(StaticMeshComponent != nullptr)
		{
			StaticMeshComponent->SetMaterial(0, bIsNotWalkable ? NonWalkableMaterial : WalkableMaterial);
		}
	}
}

bool ABlockActor::GetIsWalkable() const
{
	return !bIsNotWalkable;
}
