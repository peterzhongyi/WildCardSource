// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "WildCardCharacter.h"
#include "Components/SphereComponent.h"
#include "GameFramework/Actor.h"
#include "EnemySpawnPoint.generated.h"

UCLASS()
class WILDCARD_API AEnemySpawnPoint : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AEnemySpawnPoint();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Enemy Spawn")
    TSubclassOf<AWildCardCharacter> EnemyClass;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Debug")
	USphereComponent* DebugSphere;
};
