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
	if (ControlledCharacter == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("EnemyAttack - ControlledCharacter is nullptr")); 
		return;
	}

	
	
	ControlledCharacter->Attack();
	ControlledCharacter->Attack();
}

void AWildCardAIController::ActionDone()
{
	if (ControlledCharacter->Stamina <= 0.0f)
	{
		ControlledCharacter->OnAttackFinished.RemoveDynamic(this, &AWildCardAIController::ActionDone);
		WildCardGameState->NextTurn();
		return;
	}

	Action();
}
