// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "WildCardUserWidget.generated.h"

/**
 * 
 */
UCLASS()
class WILDCARD_API UWildCardUserWidget : public UUserWidget
{
	GENERATED_BODY()

public:
    // Declare the functions you want to call from C++
    // These must match the function names in your Blueprint
	UFUNCTION(BlueprintImplementableEvent)
	void UpdateHealth(float NewHealth);

	UFUNCTION(BlueprintImplementableEvent)
	void UpdateMaxHealth(float MaxHealth);

	UFUNCTION(BlueprintImplementableEvent)
	void UpdateOverHeadHealthBar();
};
