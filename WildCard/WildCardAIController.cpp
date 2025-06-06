// Fill out your copyright notice in the Description page of Project Settings.


#include "WildCardAIController.h"

AWildCardAIController::AWildCardAIController()
{
	PrimaryActorTick.bCanEverTick = false;
}

void AWildCardAIController::BeginPlay()
{
	Super::BeginPlay();
	
	// Start the jump timer - jump every 1 second
	GetWorld()->GetTimerManager().SetTimer(JumpTimerHandle, this, &AWildCardAIController::MakeEnemyJump, 1.0f, true);
}

void AWildCardAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
	
	ControlledCharacter = Cast<AWildCardCharacter>(InPawn);
	if (ControlledCharacter)
	{
		UE_LOG(LogTemp, Warning, TEXT("AI Controller possessed: %s"), *ControlledCharacter->GetName());
	}
}

void AWildCardAIController::MakeEnemyJump()
{
	if (ControlledCharacter)
	{
		ControlledCharacter->Jump();
	}
}