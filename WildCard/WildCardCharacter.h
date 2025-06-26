// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h"
#include "Projectile.h"
#include "SummonStone.h"
#include "WildCardUserWidget.h"
#include "Components/WidgetComponent.h"
#include "Kismet/GameplayStatics.h"
#include "WildCardCharacter.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnStaminaChangedDelegate, float, NewStamina);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnHealthChangedDelegate, float, NewHealth);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnActionDoneDelegate);

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

	UFUNCTION(BlueprintCallable)
	void EnableSwordCollision();

	UFUNCTION(BlueprintCallable)
	void DisableSwordCollision();

	UFUNCTION(BlueprintCallable)
	void AttackEnded();

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

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character")
	bool IsEnemy = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character")
	bool InTurn = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character")
	float CharacterGravity = 980.0;

	UPROPERTY()
	FVector PreviousLocation;

	FOnStaminaChangedDelegate OnStaminaChanged;
	FOnHealthChangedDelegate OnHealthChanged;
	FOnActionDoneDelegate OnActionDone;

	void UpdateStamina(float NewStamina);
	void UpdateHealth(float NewHealth);
	float GetHealth();

	void FireBall();
	void Attack();
	void Hit();
	void Cancel();
	void Summon();

	TArray<FVector> GetValidProjectileLaunchPoints(FVector TargetPoint, float InitialSpeed, float Gravity);
	TArray<FVector> GetUniformNavMeshPoints(float GridSpacing = 200.0f);

	FRotator GetLowerArcDirection(FVector StartPoint, FVector TargetPoint, float InitialSpeed, float Gravity);

	// In public section, modify the existing Jump declaration or add:
	virtual void Jump() override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Jump")
	float JumpSpeed = 1000.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Jump")
	float GravityScale = 1.0f;
	
protected:	
	// To add mapping context
	virtual void BeginPlay();

	virtual void Tick(float DeltaTime) override;

	// In WildCardCharacter.h, add this to the protected section:
	virtual void Landed(const FHitResult& Hit) override;
	
	TSubclassOf<AProjectile> ProjectileClass;
	TSubclassOf<ASummonStone> SummonStoneClass;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "UI")
	TObjectPtr<UWidgetComponent> HealthBarWidgetComponent;

	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UWildCardUserWidget> OverHeadHealthBarClass;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "UI")
	TObjectPtr<UWildCardUserWidget> OverHeadHealthBar;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon")
	TObjectPtr<UStaticMeshComponent> GreatswordMesh;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animation")
	TObjectPtr<UAnimMontage> AttackMontage;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animation")
	TObjectPtr<UAnimMontage> HitMontage;

	UFUNCTION()
	void OnSwordOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat")
	bool bIsPreparingAttack;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat")
	bool bIsPreparingJump;

	

	// In protected section:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Jump")
	float ProjectileRadius = 5.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Jump")
	float MaxSimTime = 4.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Jump")
	float ProjectileGravityScale = 1.0f;

	// Store the prediction result
	FPredictProjectilePathResult TrajectoryResult;

	void CalculateJumpTrajectory();
	void ClearTrajectory();

	// In protected section:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Jump Camera")
	float JumpCameraOffset = 100.0f; // How far right to move camera

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Jump Camera")
	float JumpCameraArmLength = 400.0f; // Closer zoom distance

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Jump Camera")
	float CameraTransitionSpeed = 5.0f; // How fast to transition

	// Store original camera settings
	float OriginalCameraArmLength;
	FVector OriginalCameraOffset;

	void SetJumpCamera(bool bEnableJumpCamera);
	
public:
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }
};

