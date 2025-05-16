// Copyright Epic Games, Inc. All Rights Reserved.

#include "WildCardGameMode.h"
#include "WildCardCharacter.h"
#include "WildCardGameState.h"
#include "WildCardHUD.h"
#include "UObject/ConstructorHelpers.h"

AWildCardGameMode::AWildCardGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPerson/Blueprints/BP_ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != nullptr)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}

	PlayerControllerClass = AWildCardPlayerController::StaticClass();
	GameStateClass = AWildCardGameState::StaticClass();
	HUDClass = AWildCardHUD::StaticClass();
}
