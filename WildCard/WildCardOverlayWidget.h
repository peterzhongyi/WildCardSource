// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "WildCardOverlayWidget.generated.h"

/**
 * 
 */
UCLASS()
class WILDCARD_API UWildCardOverlayWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	// Declare the functions you want to call from C++
	// These must match the function names in your Blueprint
	UFUNCTION(BlueprintImplementableEvent)
	void UpdateStamina(float NewStamina);

	UFUNCTION(BlueprintImplementableEvent)
	void UpdateMaxStamina(float MaxStamina);

	UFUNCTION(BlueprintImplementableEvent)
	void UpdateHealth(float NewHealth);

	UFUNCTION(BlueprintImplementableEvent)
	void UpdateMaxHealth(float MaxHealth);
};
