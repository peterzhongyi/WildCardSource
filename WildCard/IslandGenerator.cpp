// IslandGenerator.cpp
#include "IslandGenerator.h"

AIslandGenerator::AIslandGenerator()
{
    PrimaryActorTick.bCanEverTick = false;

    GridSize = 2 * radius + 1;
    IslandCoordinates = std::vector<std::vector<FVector>>(GridSize, std::vector<FVector>(GridSize));
    
    // Create a root component
    RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));

    static ConstructorHelpers::FClassFinder<AIsland> IslandObj(TEXT("/Game/ThirdPerson/Blueprints/BP_Island"));
    if (IslandObj.Class != nullptr)
    {
        IslandClass = IslandObj.Class;
        UE_LOG(LogTemp, Warning, TEXT("IslandClass set to %s"), *IslandClass->GetName());
    }
}

void AIslandGenerator::BeginPlay()
{
    Super::BeginPlay();
    
    TArray<FVector> IslandLocations;

    // Initialize coordinates
    // Since center [2,2] is (0,0), we need to offset the coordinates
    for(int i = 0; i < GridSize; i++) {
        for(int j = 0; j < GridSize; j++) {
            // Calculate the coordinate values
            // Subtract 2 to make [2,2] the center (0,0)
            float x = (j - radius) * spacing;
            float y = (radius - i) * spacing; // Invert y so that up is positive
            
            FVector IslandLocation = FVector(x, y, 0);
            GenerateIsland(IslandLocation);
            IslandCoordinates[i][j] = IslandLocation;
        }
    }
}

AIsland* AIslandGenerator::GenerateIsland(const FVector& Location)
{
    // Create a unique name for this island component
    FName IslandName = *FString::Printf(TEXT("Island"));
    
    AIsland* Island = GetWorld()->SpawnActor<AIsland>(IslandClass, Location, FRotator(0, 0, 0));
    Island->SetOwner(this);
    
    return Island;
}