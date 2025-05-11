// Fill out your copyright notice in the Description page of Project Settings.

#include "WildCardGameState.h"
#include "WildCardPlayerController.h"
#include "Kismet\GameplayStatics.h"

AWildCardGameState::AWildCardGameState()
{
    static ConstructorHelpers::FClassFinder<ACharacter> PlayerCharacterBPClass(TEXT("/Game/ThirdPerson/Blueprints/BP_ThirdPersonCharacter"));
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
            ACharacter *Character = Cast<ACharacter>(PlayerCharacters[i]);
            if (Character)
            {
                APlayerController *PC = Cast<APlayerController>(Character->GetController());
                if (PC)
                {
                    UE_LOG(LogTemp, Warning, TEXT("Player Character %d: %s is possessed by a player controller"), i, *Character->GetName());
                    CurrentPlayerIndex = i;
                }
            }
        }
    }
}

void AWildCardGameState::SwitchTurnEventFunction()
{
    UE_LOG(LogTemp, Warning, TEXT("Notified on Switch Turn. CurrentPlayer is %d"), CurrentPlayerIndex);   
    // Record the controller angle of the current player.
    AWildCardCharacter *CurrentCharacter = Cast<AWildCardCharacter>(PlayerCharacters[CurrentPlayerIndex]);
    if (CurrentCharacter)
    {
        CurrentCharacter->ControllerAngle = WildCardPlayerController->GetControlRotation();
    }

    int NextPlayerIndex = (CurrentPlayerIndex + 1) % PlayerCharacters.Num();
    UE_LOG(LogTemp, Warning, TEXT("NextPlayerIndex is %d"), NextPlayerIndex);
    AActor* NextPlayer = PlayerCharacters[NextPlayerIndex];
    AWildCardCharacter *NextCharacter = Cast<AWildCardCharacter>(NextPlayer);
    WildCardPlayerController->Possess(Cast<AWildCardCharacter>(NextPlayer));
    // If NextCharacter's Controller Angle is never set, it will default to the initial controller angle
    // set by UE logic somewhere.
    WildCardPlayerController->SetControlRotation(NextCharacter->ControllerAngle);

    CurrentPlayerIndex = NextPlayerIndex;
}