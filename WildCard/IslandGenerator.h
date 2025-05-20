// IslandGenerator.h
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include <vector>
#include "Island.h"
#include "IslandGenerator.generated.h"

UCLASS()
class WILDCARD_API AIslandGenerator : public AActor
{
	GENERATED_BODY()
    
public:    
	AIslandGenerator();
	std::vector<std::vector<FVector>> IslandCoordinates;

protected:
	virtual void BeginPlay() override;
    
	// The static mesh to use for islands
	UPROPERTY(EditAnywhere, Category = "Island Generation")
	TSubclassOf<AActor> BP_IslandClass;

	UPROPERTY(EditAnywhere, Category = "Island Generation")
	int radius = 1;

	UPROPERTY(EditAnywhere, Category = "Island Generation")
	float spacing = 10000;
    
private:
	// Generates a single island
	AIsland* GenerateIsland(const FVector& Location);

	TSubclassOf<AIsland> IslandClass;
	int GridSize;
};