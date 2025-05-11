// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameState.h"
#include "WildCardPlayerController.h"
#include "WildCardGameState.generated.h"

/**
 * 
 */
UCLASS()
class WILDCARD_API AWildCardGameState : public AGameStateBase
{
	GENERATED_BODY()
	
public:
	AWildCardGameState();
	void SwitchTurnEventFunction();

	AWildCardPlayerController* WildCardPlayerController;
	TSubclassOf<ACharacter> PlayerCharacterClass;
	TArray<AActor*> PlayerCharacters;
	int CurrentPlayerIndex;

protected:
	virtual void BeginPlay() override;
};
