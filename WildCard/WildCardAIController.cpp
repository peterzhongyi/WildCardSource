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

	// Start the jump timer - jump every 1 second
	GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &AWildCardAIController::EnemyAttack, 2.0f, false);
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

void AWildCardAIController::EnemyAttack()
{
	if (ControlledCharacter)
	{
		ControlledCharacter->Attack();
		ControlledCharacter->Attack();
	}
	WildCardGameState->NextTurn();
}