// Fill out your copyright notice in the Description page of Project Settings.

#include "WildCardGameState.h"

#include "WildCardAIController.h"
#include "WildCardPlayerController.h"
#include "WildCardHUD.h"
#include "Kismet\GameplayStatics.h"

AWildCardGameState::AWildCardGameState()
{}

void AWildCardGameState::BeginPlay()
{
    Super::BeginPlay();

    UE_LOG(LogTemp, Warning, TEXT("WildCardGameState Begin Play"));

    APlayerController *PlayerController = GetWorld()->GetFirstPlayerController();
    WildCardPlayerController = Cast<AWildCardPlayerController>(PlayerController);
    if (WildCardPlayerController)
    {
        // UE_LOG(LogTemp, Warning, TEXT("Binding WildCardPlayerController OnSwitchTurn Event"));
        // WildCardPlayerController->OnSwitchTurn.BindUObject(this, &AWildCardGameState::SwitchTurnEventFunction);

        if (AWildCardCharacter* Character = Cast<AWildCardCharacter>(WildCardPlayerController->GetPawn()))
        {
            MainCharacter = Character;
            Characters.Add(Character);
            // This is temporary, the turn order should be determined by speed.
            Character->InTurn = true;
            CurrentPlayerIndex = 0;
            UE_LOG(LogTemp, Warning, TEXT("Added main character: %s"), *Character->GetName());
        }
    }
}

AWildCardCharacter* AWildCardGameState::FindNextCharacter()
{
    AWildCardCharacter *NextCharacter = nullptr;
    UE_LOG(LogTemp, Warning, TEXT("Notified on Switch Turn. CurrentPlayer is %d"), CurrentPlayerIndex);
    // Record the controller angle of the current player.
    AWildCardCharacter *CurrentCharacter = Cast<AWildCardCharacter>(Characters[CurrentPlayerIndex]);
    if (CurrentCharacter)
    {
        CurrentCharacter->ControllerAngle = WildCardPlayerController->GetControlRotation();
    }

    int NextPlayerIndex = (CurrentPlayerIndex + 1) % Characters.Num();
    UE_LOG(LogTemp, Warning, TEXT("NextPlayerIndex is %d"), NextPlayerIndex);
    if (AActor *NextPlayer = Characters[NextPlayerIndex])
    {
        NextCharacter = Cast<AWildCardCharacter>(NextPlayer);
        if (!NextCharacter)
        {
            UE_LOG(LogTemp, Error, TEXT("NextCharacter is nullptr"));
        }
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("NextPlayer is nullptr"));
    }

    CurrentPlayerIndex = NextPlayerIndex;
    return NextCharacter;
}

void AWildCardGameState::NextTurn()
{
    if (AWildCardCharacter *CurrentCharacter = Cast<AWildCardCharacter>(Characters[CurrentPlayerIndex]))
    {
        UE_LOG(LogTemp, Warning, TEXT("Disabling InTurn for CurrentCharacter %s"), *CurrentCharacter->GetName());
        CurrentCharacter->InTurn = false;
    }
    if (AWildCardCharacter *NextCharacter = FindNextCharacter())
    {
        // Reset NextCharacter's Stamina. This needs to be before HUD ChangeCharacter
        // for it to refresh UI with up-to-date data
        NextCharacter->Stamina = NextCharacter->MaxStamina;
        NextCharacter->InTurn = true;

        if (NextCharacter->IsEnemy)
        {
            UE_LOG(LogTemp, Error, TEXT("NextCharacter is Enemy"));
            
            if (AWildCardAIController* AIController = Cast<AWildCardAIController>(NextCharacter->GetController()))
            {
                UE_LOG(LogTemp, Error, TEXT("Got AIController"));
                AIController->BeginTurn();
            }
            return;
        }

        WildCardPlayerController->Possess(NextCharacter);
        if (AWildCardHUD* HUD = Cast<AWildCardHUD>(WildCardPlayerController->MyHUD))
        {
            HUD->ChangeCharacter(NextCharacter);
        }
        else
        {
            UE_LOG(LogTemp, Error, TEXT("Controller HUD is nullptr"));
        }

        // If NextCharacter's Controller Angle is never set, it will default to the initial controller angle
        // set by UE logic somewhere.
        WildCardPlayerController->SetControlRotation(NextCharacter->ControllerAngle);
    }
}
