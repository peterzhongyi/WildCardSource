// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "WildCardCharacter.h"
#include "WildCardAIController.generated.h"

/**
 * 
 */
UCLASS()
class WILDCARD_API AWildCardAIController : public AAIController
{
	GENERATED_BODY()

public:
	AWildCardAIController();

	void BeginTurn();

	UPROPERTY()
	class AWildCardGameState* WildCardGameState;

protected:
	virtual void BeginPlay() override;
	virtual void OnPossess(APawn* InPawn) override;

	UPROPERTY()
	AWildCardCharacter* ControlledCharacter;

	// Timer handle for behavior
	FTimerHandle TimerHandle;
	
	UFUNCTION()
	void Action();

	UFUNCTION()
	void ActionDone();
};
