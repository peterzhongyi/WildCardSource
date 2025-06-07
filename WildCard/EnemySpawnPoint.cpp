// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemySpawnPoint.h"

#include "WildCardGameState.h"

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

	// Delay spawning by 2 seconds
	GetWorld()->GetTimerManager().SetTimer(
		SpawnTimerHandle,
		this,
		&AEnemySpawnPoint::SpawnEnemy,
		2.0f,
		false
	);
}

void AEnemySpawnPoint::SpawnEnemy()
{
	if (EnemyClass)
	{
		FVector SpawnLocation = GetActorLocation();
		FRotator SpawnRotation = GetActorRotation();

		if (AWildCardCharacter* SpawnedEnemy = GetWorld()->SpawnActor<AWildCardCharacter>(EnemyClass, SpawnLocation, SpawnRotation))
		{
			UE_LOG(LogTemp, Warning, TEXT("Enemy spawned at %s"), *SpawnLocation.ToString());
			if (AWildCardGameState* GameState = Cast<AWildCardGameState>(GetWorld()->GetGameState()))
			{
				GameState->Characters.Add(SpawnedEnemy);
				UE_LOG(LogTemp, Warning, TEXT("Added new character to PlayerCharacters array. Total count: %d"), GameState->Characters.Num());
			}
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("EnemySpawnPoint: No EnemyClass set"));
	}
}

