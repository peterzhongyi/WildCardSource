// Fill out your copyright notice in the Description page of Project Settings.


#include "WildCardAIController.h"

#include "NavigationSystem.h"
#include "WildCardGameState.h"
#include "Components/CapsuleComponent.h"

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
	ControlledCharacter->OnActionDone.AddDynamic(this, &AWildCardAIController::NextAction);
	
	NextAction();
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
	UE_LOG(LogTemp, Warning, TEXT("Enemy AI Controller Start Action"));
	// Increase counter to prevent infinite loops.
	ActionCounter++;
	
	if (ControlledCharacter == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("Action - ControlledCharacter is nullptr")); 
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
		UE_LOG(LogTemp, Error, TEXT("Action - PlayerCharacter is nullptr")); 
		return;
	}
	
	// Get player and enemy location
	UNavigationSystemV1* NavSys = FNavigationSystem::GetCurrent<UNavigationSystemV1>(GetWorld());
	FNavLocation EnemyNavLocation;
	NavSys->ProjectPointToNavigation(ControlledCharacter->GetActorLocation(),
		EnemyNavLocation, FVector(100.0f, 100.0f, 1000000.0f));
	FVector EnemyLocation = FVector(EnemyNavLocation);
	
	FNavLocation PlayerNavLocation;
	NavSys->ProjectPointToNavigation(PlayerCharacter->GetActorLocation(),
		PlayerNavLocation, FVector(100.0f, 100.0f, 1000000.0f));
	FVector PlayerLocation = FVector(PlayerNavLocation);

	UE_LOG(LogTemp, Warning, TEXT("Action - enemy loc: %s"), *EnemyLocation.ToString());
	UE_LOG(LogTemp, Warning, TEXT("Action - player loc: %s"), *PlayerLocation.ToString());
	
	// Pick a skill to cast - just attack, at the moment
	float DistanceToPlayer = FVector::Dist(PlayerLocation, EnemyLocation);
	float AttackRange = 200.0f; // Adjust this value as needed
	if (DistanceToPlayer <= AttackRange)
	{
		// Calculate direction to player
		FVector DirectionToPlayer = (PlayerLocation - EnemyLocation).GetSafeNormal();
    
		// Create rotation from direction (only use Yaw, keep Pitch and Roll at 0)
		FRotator LookAtRotation = FRotationMatrix::MakeFromX(DirectionToPlayer).Rotator();
		LookAtRotation.Pitch = 0.0f;
		LookAtRotation.Roll = 0.0f;
    
		// Set the character's rotation
		ControlledCharacter->SetActorRotation(LookAtRotation);
		
		ControlledCharacter->Attack();
		ControlledCharacter->Attack();
		return;
	}

	UE_LOG(LogTemp, Warning, TEXT("Action - need to move to a cast location!"));
	
	// Find location to cast skill.
	TArray<FVector> NavMeshPoints = ControlledCharacter->GetUniformNavMeshPoints(100.0);
	bool FoundCastLocation = false;
	FVector BestCastLocation = FVector::ZeroVector;
	float BestCastDistance = FLT_MAX; // The closer to the enemy, the better
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
			FoundCastLocation = true;
			if (FVector::Dist(Point, EnemyLocation) < BestCastDistance)
			{
				BestCastLocation = Point;
				BestCastDistance = FVector::Dist(Point, EnemyLocation); 
			}
		}
	}

	if (!FoundCastLocation)
	{
		UE_LOG(LogTemp, Warning, TEXT("Action - no casting loc found!"));
		return;
	}

	DrawDebugSphere(
		GetWorld(),
		BestCastLocation,
		10.0f,           // Radius
		4,              // Segments
		FColor::Green,     // Color
		false,           // Persistent lines
		50.0f             // Lifetime in second
	);

	// Find Out Stamina Consumption without jumping
	UE_LOG(LogTemp, Warning, TEXT("EnemyLocation is %s"), *EnemyLocation.ToString());
	UE_LOG(LogTemp, Warning, TEXT("BestCastLocation is %s"), *BestCastLocation.ToString());
	double RawPath = 0.0;
	UNavigationSystemV1::GetPathLength(GetWorld(), EnemyLocation, BestCastLocation, RawPath);
	if (RawPath < 0.0)
	{
		UE_LOG(LogTemp, Warning,
			TEXT("There is no raw path to BestCastLocation %s"),
			*BestCastLocation.ToString());
		return;
	}
	UE_LOG(LogTemp, Warning, TEXT("Raw path length to BestCastLocation %f"), RawPath);
	float RawPathStaminaCost = static_cast<float>(RawPath) * ControlledCharacter->StaminaPerUnitDistance;
	UE_LOG(LogTemp, Warning, TEXT("Raw path stamina cost to BestCastLocation %f"), RawPathStaminaCost);
	

	// Find Out Stamina Consumption if jumping, and where to jump from.
	float StaminaCostWithJump;
	FVector BestMoveLocation = FVector::ZeroVector;
	FRotator OutRotation;
	if (ControlledCharacter->CalculateProjectileLaunchRotation(EnemyLocation, BestCastLocation,
			ControlledCharacter->JumpSpeed, ControlledCharacter->CharacterGravity, OutRotation))
	{
		StaminaCostWithJump = 20.f;
		BestMoveLocation = EnemyLocation;
		UE_LOG(LogTemp, Warning, TEXT("StaminaCostWithJump to BestCastLocation %f"), StaminaCostWithJump);
		
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Action - can't directly jump to casting loc, moving!"));
		bool FoundMoveLocation = false;
		float BestMoveDistance = FLT_MAX; // The closer to enemy, the better
		for (const FVector& Point : NavMeshPoints)
		{
			if (ControlledCharacter->CalculateProjectileLaunchRotation(Point, BestCastLocation,
				ControlledCharacter->JumpSpeed, ControlledCharacter->CharacterGravity, OutRotation))
			{
				// This is a valid location to jump from
				if (FVector::Dist(Point, EnemyLocation) < BestMoveDistance)
				{
					FoundMoveLocation = true;
					BestMoveLocation = Point;
					BestMoveDistance = FVector::Dist(Point, EnemyLocation);
				}
			}
		}
	
		if (!FoundMoveLocation)
		{
			UE_LOG(LogTemp, Warning, TEXT("Action - no moving loc found!"));
			return;
		}

		// Calculate total stamina cost (walk + jump)
		double PathToMoveLocation = 0.0;
		UNavigationSystemV1::GetPathLength(GetWorld(), EnemyLocation, BestMoveLocation, PathToMoveLocation);
		if (RawPath < 0.0)
		{
			UE_LOG(LogTemp, Warning,
				TEXT("There is no raw path to BestMoveLocation %s"),
				*BestMoveLocation.ToString());
			return;
		}
		UE_LOG(LogTemp, Warning, TEXT("PathToMoveLocation %f"), PathToMoveLocation);
		float PathToMoveLocationStaminaCost = static_cast<float>(PathToMoveLocation) * ControlledCharacter->StaminaPerUnitDistance;
		UE_LOG(LogTemp, Warning, TEXT("PathToMoveLocationStaminaCost to BestCastLocation %f"), PathToMoveLocationStaminaCost);

		StaminaCostWithJump = 20.f + PathToMoveLocationStaminaCost;
		UE_LOG(LogTemp, Warning, TEXT("StaminaCostWithJump to BestCastLocation %f"), StaminaCostWithJump);
	}

	// Simply move if it doesn't cost too much more stamina, compared to jumping.
	if (RawPathStaminaCost < StaminaCostWithJump)
	{
		UE_LOG(LogTemp, Warning, TEXT("Simply Moving to CastLocation"));
		MoveToLocation(BestCastLocation, -1.0f, false);
	}
	else
	{
		if (BestMoveLocation == EnemyLocation)
		{
			UE_LOG(LogTemp, Warning, TEXT("BestMoveLocation is EnemyLocation, Jump!"));
			FVector JumpDirection = OutRotation.Vector();
			ControlledCharacter->ActualJump(JumpDirection * ControlledCharacter->JumpSpeed);
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("Move to location to jump. The actual jumping is handled by next Action."));
			MoveToLocation(BestMoveLocation, -1.0f, true);
		}
	}
}

void AWildCardAIController::NextAction()
{
	GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &AWildCardAIController::Action, 1.0f, false);
}

void AWildCardAIController::OnMoveCompleted(FAIRequestID RequestID, const FPathFollowingResult& Result)
{
	Super::OnMoveCompleted(RequestID, Result);
	
	// After movement is complete, mark action as done
	UE_LOG(LogTemp, Warning, TEXT("OnMoveCompleted called"));
	NextAction();
}
