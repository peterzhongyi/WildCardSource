// WildCardHUD.cpp
#include "WildCardHUD.h"
#include "Engine/Canvas.h"
#include "Engine/Texture2D.h"
#include "TextureResource.h"
#include "CanvasItem.h"
#include "UObject/ConstructorHelpers.h"
#include "Kismet/GameplayStatics.h"

void AWildCardHUD::BindWidgetCallbacks(AWildCardCharacter* character)
{
    character->OnStaminaChanged.AddDynamic(this, &AWildCardHUD::OnStaminaChangedHandler);
    character->OnHealthChanged.AddDynamic(this, &AWildCardHUD::OnHealthChangedHandler);
}

void AWildCardHUD::UnBindWidgetCallbacks(AWildCardCharacter* character)
{
    character->OnStaminaChanged.RemoveDynamic(this, &AWildCardHUD::OnStaminaChangedHandler);
    character->OnHealthChanged.RemoveDynamic(this, &AWildCardHUD::OnHealthChangedHandler);
}

void AWildCardHUD::RefreshOverlay()
{
    // Order is important! Otherwise, dividing by 0
    if (OverlayWidget == nullptr || CurrentCharacter == nullptr)
    {
        UE_LOG(LogTemp, Error, TEXT("RefreshOverlay - either OverlayWidge or CurrentCharacter is nullptr"));
        return;
    }
    OverlayWidget->UpdateMaxStamina(CurrentCharacter->MaxStamina); 
    OverlayWidget->UpdateStamina(CurrentCharacter->Stamina);
    OverlayWidget->UpdateMaxHealth(CurrentCharacter->MaxHealth);
    OverlayWidget->UpdateHealth(CurrentCharacter->Health);
}

AWildCardHUD::AWildCardHUD()
{
    // Constructor - initialize anything needed here
    static ConstructorHelpers::FClassFinder<UWildCardOverlayWidget> OverlayWidgetObj(TEXT("/Game/ThirdPerson/Blueprints/WBP_Overlay"));
    if (OverlayWidgetObj.Class != nullptr)
    {
        OverlayWidgetClass = OverlayWidgetObj.Class;
        UE_LOG(LogTemp, Warning, TEXT("OverlayWidgetClass set to %s"), *OverlayWidgetClass->GetName());
    }
}

void AWildCardHUD::BeginPlay()
{
    Super::BeginPlay();
    
    // Log that the HUD has been initialized
    UE_LOG(LogTemp, Warning, TEXT("WildCardHUD Begin Play"));
    if (APlayerController* PC = GetOwningPlayerController())
    {
        CurrentCharacter = Cast<AWildCardCharacter>(PC->GetPawn());
        if (CurrentCharacter)
        {
            UE_LOG(LogTemp, Warning, TEXT("HUD current character set to %s"), *CurrentCharacter->GetName());
            BindWidgetCallbacks(CurrentCharacter);
        }
        else
        {
            UE_LOG(LogTemp, Error, TEXT("HUD unable to set CurrentCharacter"));
        }
        
    }
    
    // Create the overlay widget
    if (OverlayWidgetClass)
    {
        OverlayWidget = CreateWidget<UWildCardOverlayWidget>(GetOwningPlayerController(), OverlayWidgetClass);
        if (OverlayWidget)
        {
            UE_LOG(LogTemp, Warning, TEXT("Successfully created overlay widget"));

            // Set up HUD for CurrentCharacter
            RefreshOverlay();
            OverlayWidget->AddToViewport();
        }
        else
        {
            UE_LOG(LogTemp, Error, TEXT("Failed to create overlay widget"));
        }
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("OverlayWidgetClass is not set. Did you create the WBP_Overlay Blueprint?"));
    }
}

void AWildCardHUD::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);
}

void AWildCardHUD::OnStaminaChangedHandler(float NewStamina)
{
    // UE_LOG(LogTemp, Warning, TEXT("HUD received Stamina Change Update %f"), NewStamina);
    if (OverlayWidget) {
        OverlayWidget->UpdateStamina(NewStamina);
    }
}

void AWildCardHUD::OnHealthChangedHandler(float NewHealth)
{
    UE_LOG(LogTemp, Warning, TEXT("HUD received Health Change Update %f"), NewHealth);
    if (OverlayWidget) {
        OverlayWidget->UpdateHealth(NewHealth);
    }
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
    
    UnBindWidgetCallbacks(CurrentCharacter);
    BindWidgetCallbacks(NextCharacter);
    
    CurrentCharacter = NextCharacter;
    RefreshOverlay();
}
