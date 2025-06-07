// Fill out your copyright notice in the Description page of Project Settings.


#include "WildCardAIController.h"

AWildCardAIController::AWildCardAIController()
{
	PrimaryActorTick.bCanEverTick = false;
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
}

void AWildCardAIController::MakeEnemyJump()
{
	// Start the jump timer - jump every 1 second
	GetWorld()->GetTimerManager().SetTimer(JumpTimerHandle, this, &AWildCardAIController::MakeEnemyJump, 2.0f, true);

	if (ControlledCharacter)
	{
		ControlledCharacter->Jump();
	}
}