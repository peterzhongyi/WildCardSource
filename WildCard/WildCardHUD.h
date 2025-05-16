// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "WildCardCharacter.h"
#include "WildCardUserWidget.h" 
#include "WildCardHUD.generated.h"

/**
 * 
 */
UCLASS()
class WILDCARD_API AWildCardHUD : public AHUD
{
	GENERATED_BODY()

public:
    AWildCardHUD();

    // Called when the game starts
    virtual void BeginPlay() override;

    // Tick function to update HUD every frame
    virtual void Tick(float DeltaSeconds) override;

	// Reference to the current controlled character
    AWildCardCharacter* CurrentCharacter;

	void ChangeCharacter(AWildCardCharacter* NextCharacter);

protected:
	UFUNCTION()
	void OnStaminaChangedHandler(float NewStamina);

	UFUNCTION()
	void OnHealthChangedHandler(float NewHealth);

    // Widget class reference
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UWildCardUserWidget> OverlayWidgetClass;

	// Widget instance
	UPROPERTY()
	UWildCardUserWidget* OverlayWidget;
};
