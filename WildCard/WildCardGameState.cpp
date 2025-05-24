// Fill out your copyright notice in the Description page of Project Settings.

#include "WildCardGameState.h"
#include "WildCardPlayerController.h"
#include "WildCardHUD.h"
#include "Kismet\GameplayStatics.h"

AWildCardGameState::AWildCardGameState()
{
    static ConstructorHelpers::FClassFinder<ACharacter> PlayerCharacterBPClass(TEXT("/Game/ThirdPerson/Blueprints/BP_SyntyCharacter"));
    if (PlayerCharacterBPClass.Class != nullptr)
    {
        PlayerCharacterClass = PlayerCharacterBPClass.Class;
    }
}

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
    }

    UGameplayStatics::GetAllActorsOfClass(GetWorld(), PlayerCharacterClass, PlayerCharacters);
    for (int32 i = 0; i < PlayerCharacters.Num(); i++)
    {
        if (PlayerCharacters[i])
        {
            UE_LOG(LogTemp, Warning, TEXT("Player Character %d: %s"), i, *PlayerCharacters[i]->GetName());

            // Check if possessed by player controller
            if (ACharacter *Character = Cast<ACharacter>(PlayerCharacters[i]))
            {
                if (APlayerController *PC = Cast<APlayerController>(Character->GetController()))
                {
                    UE_LOG(LogTemp, Warning, TEXT("Player Character %d: %s is possessed by a player controller"), i, *Character->GetName());
                    CurrentPlayerIndex = i;
                }
            }
        }
    }
}

AWildCardCharacter *AWildCardGameState::SwitchTurnEventFunction()
{
    AWildCardCharacter *NextCharacter = nullptr;
    UE_LOG(LogTemp, Warning, TEXT("Notified on Switch Turn. CurrentPlayer is %d"), CurrentPlayerIndex);
    // Record the controller angle of the current player.
    AWildCardCharacter *CurrentCharacter = Cast<AWildCardCharacter>(PlayerCharacters[CurrentPlayerIndex]);
    if (CurrentCharacter)
    {
        CurrentCharacter->ControllerAngle = WildCardPlayerController->GetControlRotation();
    }
    // Reset CurrentCharacter's Stamina
    CurrentCharacter->Stamina = CurrentCharacter->MaxStamina;

    int NextPlayerIndex = (CurrentPlayerIndex + 1) % PlayerCharacters.Num();
    UE_LOG(LogTemp, Warning, TEXT("NextPlayerIndex is %d"), NextPlayerIndex);
    if (AActor *NextPlayer = PlayerCharacters[NextPlayerIndex])
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