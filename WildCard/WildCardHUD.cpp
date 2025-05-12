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
        UE_LOG(LogTemp, Warning, TEXT("HUD current character set to %s"), *CurrentCharacter->GetName());
    }
}

void AWildCardHUD::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);
    
    // Update the reference to the current character
    if (CurrentCharacter)
    {
        UE_LOG(LogTemp, Warning, TEXT("HUD current character set to %s"), *CurrentCharacter->GetName());
    }

}