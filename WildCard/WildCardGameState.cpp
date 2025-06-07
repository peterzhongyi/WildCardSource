// Fill out your copyright notice in the Description page of Project Settings.

#include "WildCardGameState.h"
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
        UE_LOG(LogTemp, Warning, TEXT("Binding WildCardPlayerController OnSwitchTurn Event"));
        WildCardPlayerController->OnSwitchTurn.BindUObject(this, &AWildCardGameState::SwitchTurnEventFunction);

        AWildCardCharacter* MainCharacter = Cast<AWildCardCharacter>(WildCardPlayerController->GetPawn());
        if (MainCharacter)
        {
            Characters.Add(MainCharacter);
            CurrentPlayerIndex = 0;
            UE_LOG(LogTemp, Warning, TEXT("Added main character: %s"), *MainCharacter->GetName());
        }
    }

    
}

AWildCardCharacter *AWildCardGameState::SwitchTurnEventFunction()
{
    AWildCardCharacter *NextCharacter = nullptr;
    UE_LOG(LogTemp, Warning, TEXT("Notified on Switch Turn. CurrentPlayer is %d"), CurrentPlayerIndex);
    // Record the controller angle of the current player.
    AWildCardCharacter *CurrentCharacter = Cast<AWildCardCharacter>(Characters[CurrentPlayerIndex]);
    if (CurrentCharacter)
    {
        CurrentCharacter->ControllerAngle = WildCardPlayerController->GetControlRotation();
    }
    // Reset CurrentCharacter's Stamina
    CurrentCharacter->Stamina = CurrentCharacter->MaxStamina;

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