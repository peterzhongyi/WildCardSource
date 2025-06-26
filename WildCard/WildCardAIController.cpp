// Fill out your copyright notice in the Description page of Project Settings.


#include "WildCardAIController.h"

#include "NavigationSystem.h"
#include "WildCardGameState.h"

AWildCardAIController::AWildCardAIController()
{
	PrimaryActorTick.bCanEverTick = false;
}

void AWildCardAIController::BeginTurn()
{
	if (ControlledCharacter == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("Begin turn failed because nullptr for for ControlledCharacter"));
		return;
	}
	UE_LOG(LogTemp, Warning, TEXT("Begin turn for %s"), *ControlledCharacter->GetName());

	// Actions can trigger future actions
	ControlledCharacter->OnActionDone.AddDynamic(this, &AWildCardAIController::Action);
	
	GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &AWildCardAIController::Action, 2.0f, false);
}

void AWildCardAIController::BeginPlay()
{
	Super::BeginPlay();
}

void AWildCardAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
	
	ControlledCharacter = Cast<AWildCardCharacter>(InPawn);
	if (ControlledCharacter)
	{
		UE_LOG(LogTemp, Warning, TEXT("AI Controller possessed: %s"), *ControlledCharacter->GetName());
	}

	WildCardGameState = Cast<AWildCardGameState>(GetWorld()->GetGameState());
	if (!WildCardGameState)
	{
		UE_LOG(LogTemp, Error, TEXT("AIController Can't find WildCardGameState")); 
	}
}

void AWildCardAIController::Action()
{
	// Increase counter to prevent infinite loops.
	ActionCounter++;
	
	if (ControlledCharacter == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("EnemyAttack - ControlledCharacter is nullptr")); 
		return;
	}

	if (ControlledCharacter->Stamina <= 0.0f || ActionCounter > 20)
	{
		ActionCounter = 0;
		ControlledCharacter->OnActionDone.RemoveDynamic(this, &AWildCardAIController::Action);
		WildCardGameState->NextTurn();
		return;
	}

	AWildCardCharacter* PlayerCharacter = WildCardGameState->MainCharacter;
	if (PlayerCharacter == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("EnemyAttack - PlayerCharacter is nullptr")); 
		return;
	}

	FVector EnemyLocation = ControlledCharacter->GetActorLocation();
	FVector PlayerLocation = PlayerCharacter->GetActorLocation();
	
	// Pick a skill to cast - just attack, at the moment
	float DistanceToPlayer = FVector::Dist(PlayerLocation, EnemyLocation);
	float AttackRange = 300.0f; // Adjust this value as needed
	if (DistanceToPlayer <= AttackRange)
	{
		ControlledCharacter->Attack();
		ControlledCharacter->Attack();
		return;
	}
	
	// Find location to cast skill.
	TArray<FVector> NavMeshPoints = ControlledCharacter->GetUniformNavMeshPoints(100.0);

	FVector BestLocation = EnemyLocation;
	float BestDistance = DistanceToPlayer;
	for (const FVector& Point : NavMeshPoints)
	{
		DrawDebugSphere(
			GetWorld(),
			Point,
			10.0f,           // Radius
			4,              // Segments
			FColor::Red,     // Color
			false,           // Persistent lines
			50.0f             // Lifetime in seconds
		);
		
		// TODO: add method to validate Point beyond just checking DistanceToPlayer
		if (FVector::Dist(Point, PlayerLocation) <= AttackRange)
		{
			DrawDebugSphere(
				GetWorld(),
				Point,
				10.0f,           // Radius
				4,              // Segments
				FColor::Yellow,     // Color
				false,           // Persistent lines
				50.0f             // Lifetime in seconds
			);
			if (FVector::Dist(Point, EnemyLocation) < BestDistance)
			{
				BestLocation = Point;
				BestDistance = FVector::Dist(Point, EnemyLocation);
			}
		}
	}

	DrawDebugSphere(
		GetWorld(),
		BestLocation,
		10.0f,           // Radius
		4,              // Segments
		FColor::Green,     // Color
		false,           // Persistent lines
		50.0f             // Lifetime in second
	);
	

	// FRotator JumpAngle = ControlledCharacter->GetLowerArcDirection(EnemyLocation, PlayerLocation,
	// 	ControlledCharacter->JumpSpeed, ControlledCharacter->CharacterGravity);

	// FVector JumpDirection = JumpAngle.Vector();
	// ControlledCharacter->LaunchCharacter(JumpDirection * ControlledCharacter->JumpSpeed, false, false);
	
	// Move towards player
	UE_LOG(LogTemp, Warning, TEXT("Moving towards player, distance: %f"), DistanceToPlayer);
	double PathLength = 0.0;
	UNavigationSystemV1::GetPathLength(GetWorld(), 
	GetPawn()->GetActorLocation(), 
	BestLocation, PathLength);

	if (PathLength > 0.0)
	{
		UE_LOG(LogTemp, Warning, TEXT("Path length to destination: %f"), PathLength);
	}
	MoveToLocation(BestLocation, -1.0f, false);
}

void AWildCardAIController::OnMoveCompleted(FAIRequestID RequestID, const FPathFollowingResult& Result)
{
	Super::OnMoveCompleted(RequestID, Result);
	
	// After movement is complete, mark action as done
	UE_LOG(LogTemp, Warning, TEXT("OnMoveCompleted called"));
	Action();
}
