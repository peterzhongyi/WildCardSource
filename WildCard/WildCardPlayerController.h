// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "WildCardCharacter.h"
#include "WildCardPlayerController.generated.h"

DECLARE_DELEGATE_RetVal(AWildCardCharacter*, FOnSwitchTurnDelegate);

UCLASS()
class WILDCARD_API AWildCardPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
    AWildCardPlayerController();

	UPROPERTY()
    AWildCardCharacter* WildCardCharacter;

	UPROPERTY()
	class AWildCardGameState* WildCardGameState;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Input, meta=(AllowPrivateAccess = "true"))
	class UInputMappingContext* DefaultMappingContext;

	/** Jump Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* JumpAction;

	/** Move Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* MoveAction;

	/** Look Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* LookAction;

	/** Look Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* SwitchTurnAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* CastAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* AttackAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* CancelAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* SummonAction;
    
    
    virtual void BeginPlay() override;
    virtual void SetupInputComponent() override;
	virtual void OnPossess(APawn* InPawn) override;

	void HandleJump();
	void HandleStopJumping();
	void HandleMove(const FInputActionValue& Value);
	void HandleLook(const FInputActionValue& Value);
	void HandleSwitchTurn();
	void HandleCast();
	void HandleAttack();
	void HandleCancel();
	void HandleSummon();

	FOnSwitchTurnDelegate OnSwitchTurn;
};
