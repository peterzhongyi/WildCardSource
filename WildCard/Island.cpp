// Fill out your copyright notice in the Description page of Project Settings.


#include "Island.h"

// Sets default values
AIsland::AIsland()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	IslandMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Island Mesh"));
	RootComponent = IslandMesh;
}

// Called when the game starts or when spawned
void AIsland::BeginPlay()
{
	Super::BeginPlay();
	
}

