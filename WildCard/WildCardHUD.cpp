// WildCardHUD.cpp
#include "WildCardHUD.h"
#include "Engine/Canvas.h"
#include "Engine/Texture2D.h"
#include "TextureResource.h"
#include "CanvasItem.h"
#include "UObject/ConstructorHelpers.h"
#include "Kismet/GameplayStatics.h"

AWildCardHUD::AWildCardHUD()
{
    // Constructor - initialize anything needed here
}

void AWildCardHUD::BeginPlay()
{
    Super::BeginPlay();
    
    // Log that the HUD has been initialized
    UE_LOG(LogTemp, Warning, TEXT("WildCardHUD Begin Play"));
    APlayerController* PC = GetOwningPlayerController();
    if (PC)
    {
        CurrentCharacter = Cast<AWildCardCharacter>(PC->GetPawn());
        if (CurrentCharacter)
        {
            UE_LOG(LogTemp, Warning, TEXT("HUD current character set to %s"), *CurrentCharacter->GetName());
            CurrentCharacter->OnStaminaChanged.AddDynamic(this, &AWildCardHUD::OnStaminaChangedHandler);
        }
        else
        {
            UE_LOG(LogTemp, Error, TEXT("HUD unable to set CurrentCharacter"));
        }
        
    }
}

void AWildCardHUD::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);
    
    // Update the reference to the current character
    // if (CurrentCharacter)
    // {
    //     UE_LOG(LogTemp, Warning, TEXT("HUD current character set to %s"), *CurrentCharacter->GetName());
    // }

}

void AWildCardHUD::OnStaminaChangedHandler(float NewStamina)
{
    UE_LOG(LogTemp, Warning, TEXT("HUD received Stamina Change Update %f"), NewStamina);
}

void AWildCardHUD::ChangeCharacter(AWildCardCharacter* NextCharacter)
{
    if (CurrentCharacter == nullptr)
    {
        UE_LOG(LogTemp, Error, TEXT("HUD ChangeCharacter - CurrentCharacter is null"));
        return;
    }
    if (NextCharacter == nullptr)
    {
        UE_LOG(LogTemp, Error, TEXT("HUD ChangeCharacter - NextCharacter is null"));
        return;
    }
    CurrentCharacter->OnStaminaChanged.RemoveDynamic(this, &AWildCardHUD::OnStaminaChangedHandler);
    NextCharacter->OnStaminaChanged.AddDynamic(this, &AWildCardHUD::OnStaminaChangedHandler);
    CurrentCharacter = NextCharacter;
}