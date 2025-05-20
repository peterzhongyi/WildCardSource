// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Island.generated.h"

UCLASS()
class WILDCARD_API AIsland : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AIsland();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	UPROPERTY(EditDefaultsOnly)
    UStaticMeshComponent* IslandMesh;
};
