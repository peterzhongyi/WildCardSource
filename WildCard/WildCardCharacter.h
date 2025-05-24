// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h"
#include "Projectile.h"
#include "WildCardUserWidget.h"
#include "Components/WidgetComponent.h"
#include "WildCardCharacter.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnStaminaChangedDelegate, float, NewStamina);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnHealthChangedDelegate, float, NewHealth);

UCLASS(config=Game)
class AWildCardCharacter : public ACharacter
{
	GENERATED_BODY()

	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FollowCamera;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Projectile", meta = (AllowPrivateAccess = "true"))
	class USceneComponent* ProjectileSpawnPoint;
	
	/** MappingContext */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputMappingContext* DefaultMappingContext;
	

public:
	AWildCardCharacter();

	/** Called for movement input */
	void Move(const FInputActionValue& Value);

	/** Called for looking input */
	void Look(const FInputActionValue& Value);

	// Record Controller Angle between turns.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Controller")
    FRotator ControllerAngle;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Character Stats")
    float Stamina;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character Stats")
	float MaxStamina;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Character Stats")
	float Health;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character Stats")
	float MaxHealth;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character Stats")
	float StaminaPerUnitDistance;

	UPROPERTY()
	FVector PreviousLocation;

	FOnStaminaChangedDelegate OnStaminaChanged;
	FOnHealthChangedDelegate OnHealthChanged;

	void UpdateStamina(float NewStamina);
	void UpdateHealth(float NewHealth);
	float GetHealth();

	void FireBall();
	
protected:	
	// To add mapping context
	virtual void BeginPlay();

	virtual void Tick(float DeltaTime) override;

	TSubclassOf<AProjectile> ProjectileClass;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "UI")
	TObjectPtr<UWidgetComponent> HealthBarWidgetComponent;

	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UWildCardUserWidget> OverHeadHealthBarClass;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "UI")
	TObjectPtr<UWildCardUserWidget> OverHeadHealthBar;

public:
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }
};

