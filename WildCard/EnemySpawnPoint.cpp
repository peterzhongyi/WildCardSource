// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemySpawnPoint.h"

// Sets default values
AEnemySpawnPoint::AEnemySpawnPoint()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));

	// Create debug sphere
	DebugSphere = CreateDefaultSubobject<USphereComponent>(TEXT("DebugSphere"));
	DebugSphere->SetupAttachment(RootComponent);
	DebugSphere->SetSphereRadius(50.0f);
	DebugSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	DebugSphere->SetVisibility(true);
}

// Called when the game starts or when spawned
void AEnemySpawnPoint::BeginPlay()
{
	Super::BeginPlay();

	if (EnemyClass)
	{
		FVector SpawnLocation = GetActorLocation();
		FRotator SpawnRotation = GetActorRotation();
		
		AWildCardCharacter* SpawnedEnemy = GetWorld()->SpawnActor<AWildCardCharacter>(EnemyClass, SpawnLocation, SpawnRotation);
		if (SpawnedEnemy)
		{
			UE_LOG(LogTemp, Warning, TEXT("Enemy spawned at %s"), *SpawnLocation.ToString());
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("EnemySpawnPoint: No EnemyClass set"));
	}
}

