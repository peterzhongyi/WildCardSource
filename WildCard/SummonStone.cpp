// Fill out your copyright notice in the Description page of Project Settings.


#include "SummonStone.h"
#include "WildCardGameState.h"
#include "WildCardCharacter.h"

ASummonStone::ASummonStone()
{
	// Enable ticking for this actor
	PrimaryActorTick.bCanEverTick = true;
	
	// Initialize stationary time
	StationaryTime = 0.0f;
	
	// Configure projectile movement for bouncing
	if (ProjectileMovementComponent)
	{
		ProjectileMovementComponent->bShouldBounce = true;
		ProjectileMovementComponent->Bounciness = 0.5f; // Adjust bounce strength
		ProjectileMovementComponent->Friction = 0.5f;   // Add some friction to slow down over time
	}

	static ConstructorHelpers::FClassFinder<AWildCardCharacter> CharacterObj(TEXT("/Game/ThirdPerson/Blueprints/BP_SyntyCharacter"));
	if (CharacterObj.Class != nullptr)
	{
		CharacterClass = CharacterObj.Class;
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Can't find CharacterClass"));
	}
}

void ASummonStone::BeginPlay()
{
	Super::BeginPlay();
	
}

void ASummonStone::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	// Get current speed from ProjectileMovementComponent
	float CurrentSpeed = ProjectileMovementComponent->Velocity.Size();
	
	// Check if the stone is moving slowly enough to be considered stationary
	if (CurrentSpeed < MinVelocityThreshold)
	{
		StationaryTime += DeltaTime;
		
		// Destroy after being stationary for the specified time
		if (StationaryTime >= DestructionDelay)
		{
			UE_LOG(LogTemp, Warning, TEXT("SummonStone destroying itself after being stationary"));

			// Spawn the character at the stone's location
			FVector SpawnLocation = GetActorLocation();
			FRotator SpawnRotation = FRotator::ZeroRotator;
    
			 AWildCardCharacter* NewCharacter = GetWorld()->SpawnActor<AWildCardCharacter>(CharacterClass, SpawnLocation, SpawnRotation);

			// Add to GameState's PlayerCharacters array
			if (AWildCardGameState* GameState = Cast<AWildCardGameState>(GetWorld()->GetGameState()))
			{
				GameState->PlayerCharacters.Add(NewCharacter);
				UE_LOG(LogTemp, Warning, TEXT("Added new character to PlayerCharacters array. Total count: %d"), GameState->PlayerCharacters.Num());
			}
			
			Destroy();
		}
	}
	else
	{
		// Reset the timer if the stone starts moving again
		StationaryTime = 0.0f;
	}
}

void ASummonStone::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	// Don't destroy on hit like the base Projectile class does
	// Just let it bounce naturally through the ProjectileMovementComponent
	UE_LOG(LogTemp, Warning, TEXT("SummonStone hit something, bouncing"));
}