// Fill out your copyright notice in the Description page of Project Settings.


#include "WildCardAIController.h"

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

	// Bind to attack finished delegate
	ControlledCharacter->OnAttackFinished.AddDynamic(this, &AWildCardAIController::ActionDone);
	
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
	ActionCounter++;
	if (ControlledCharacter == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("EnemyAttack - ControlledCharacter is nullptr")); 
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
	float DistanceToPlayer = FVector::Dist(PlayerLocation, EnemyLocation);
	float AttackRange = 200.0f; // Adjust this value as needed

	if (DistanceToPlayer > AttackRange)
	{
		// Jump to player
		ControlledCharacter->JumpSpeed;

		TArray<FVector> potential_points = ControlledCharacter->GetUniformNavMeshPoints(
			PlayerLocation,
			ControlledCharacter->JumpSpeed,
			ControlledCharacter->CharacterGravity,
			100.0);

		FVector BestLocation = EnemyLocation;
		float BestDistance = DistanceToPlayer;
		for (const FVector& Point : potential_points)
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

			float Distance = FVector::Dist(Point, PlayerLocation);
			if (Distance < BestDistance)
			{
				BestLocation = Point;
				BestDistance = Distance;
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

		FRotator JumpAngle = ControlledCharacter->GetLowerArcDirection(EnemyLocation, PlayerLocation,
			ControlledCharacter->JumpSpeed, ControlledCharacter->CharacterGravity);

		FVector JumpDirection = JumpAngle.Vector();
		ControlledCharacter->LaunchCharacter(JumpDirection * ControlledCharacter->JumpSpeed, false, false);

		
		// Move towards player
		UE_LOG(LogTemp, Warning, TEXT("Moving towards player, distance: %f"), DistanceToPlayer);
		// There might be a bug with UE on the acceptance radius. 0.8f will let enemy stops outside of attackrange,
		// resulting in an endless Action loop
		MoveToLocation(PlayerCharacter->GetActorLocation(), AttackRange * 0.5f);
	}
	else
	{
		ControlledCharacter->Attack();
		ControlledCharacter->Attack();
	}
}

void AWildCardAIController::ActionDone()
{
	if (ControlledCharacter->Stamina <= 0.0f || ActionCounter > 20)
	{
		ActionCounter = 0;
		ControlledCharacter->OnAttackFinished.RemoveDynamic(this, &AWildCardAIController::ActionDone);
		WildCardGameState->NextTurn();
		return;
	}

	Action();
}

void AWildCardAIController::OnMoveCompleted(FAIRequestID RequestID, const FPathFollowingResult& Result)
{
	Super::OnMoveCompleted(RequestID, Result);
	
	// After movement is complete, mark action as done
	UE_LOG(LogTemp, Warning, TEXT("OnMoveCompleted called"));
	ActionDone();
}
