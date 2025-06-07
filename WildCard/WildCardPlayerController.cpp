// Fill out your copyright notice in the Description page of Project Settings.


#include "WildCardPlayerController.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputMappingContext.h"
#include "InputAction.h"
#include "WildCardGameState.h"
#include "WildCardHUD.h"

AWildCardPlayerController::AWildCardPlayerController()
{
    // Find the existing Input Mapping Context
     
    DefaultMappingContext = LoadObject<UInputMappingContext>(nullptr, TEXT("/game/ThirdPerson/Input/IMC_Default")); 
    if (!DefaultMappingContext)
    { 
        UE_LOG(LogTemp, Error, TEXT("Can't find IMC_Default")); 
    }

    JumpAction = LoadObject<UInputAction>(nullptr, TEXT("/game/ThirdPerson/Input/Actions/IA_Jump")); 
    if (!JumpAction)
    { 
        UE_LOG(LogTemp, Error, TEXT("Can't find IA_Jump")); 
    }
     
    MoveAction = LoadObject<UInputAction>(nullptr, TEXT("/game/ThirdPerson/Input/Actions/IA_Move")); 
    if (!MoveAction)
    {
        UE_LOG(LogTemp, Error, TEXT("Can't find IA_Move")); 
    }

    LookAction = LoadObject<UInputAction>(nullptr, TEXT("/game/ThirdPerson/Input/Actions/IA_Look")); 
    if (!LookAction)
    {
        UE_LOG(LogTemp, Error, TEXT("Can't find IA_Look")); 
    }

    SwitchTurnAction = LoadObject<UInputAction>(nullptr, TEXT("/game/ThirdPerson/Input/Actions/IA_SwitchTurn")); 
    if (!SwitchTurnAction)
    {
        UE_LOG(LogTemp, Error, TEXT("Can't find IA_SwitchTurn")); 
    }

    CastAction = LoadObject<UInputAction>(nullptr, TEXT("/game/ThirdPerson/Input/Actions/IA_Cast")); 
    if (!CastAction)
    {
        UE_LOG(LogTemp, Error, TEXT("Can't find IA_Cast")); 
    }

    AttackAction = LoadObject<UInputAction>(nullptr, TEXT("/game/ThirdPerson/Input/Actions/IA_Attack")); 
    if (!AttackAction)
    {
        UE_LOG(LogTemp, Error, TEXT("Can't find IA_Attack")); 
    }

    CancelAction = LoadObject<UInputAction>(nullptr, TEXT("/game/ThirdPerson/Input/Actions/IA_Cancel")); 
    if (!CancelAction)
    {
        UE_LOG(LogTemp, Error, TEXT("Can't find IA_Cancel")); 
    }

    SummonAction = LoadObject<UInputAction>(nullptr, TEXT("/game/ThirdPerson/Input/Actions/IA_Summon")); 
    if (!SummonAction)
    {
        UE_LOG(LogTemp, Error, TEXT("Can't find IA_Summon")); 
    }
}

void AWildCardPlayerController::BeginPlay()
{
    Super::BeginPlay();
    
    // Setup enhanced input system
    if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
    {
        Subsystem->AddMappingContext(DefaultMappingContext, 0);
    }

    WildCardGameState = Cast<AWildCardGameState>(GetWorld()->GetGameState());
    if (!WildCardGameState)
    {
        UE_LOG(LogTemp, Error, TEXT("Can't find WildCardGameState")); 
    }
}

void AWildCardPlayerController::SetupInputComponent()
{
    Super::SetupInputComponent();
    UE_LOG(LogTemp, Warning, TEXT("SetupInputComponent is called"));
    
    // Set up input bindings
    if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(InputComponent))
    {
        UE_LOG(LogTemp, Warning, TEXT("Binding Actions"));
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Triggered, this, &AWildCardPlayerController::HandleJump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &AWildCardPlayerController::HandleStopJumping);
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AWildCardPlayerController::HandleMove);
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AWildCardPlayerController::HandleLook);
        EnhancedInputComponent->BindAction(SwitchTurnAction, ETriggerEvent::Completed, this, &AWildCardPlayerController::HandleSwitchTurn);
        EnhancedInputComponent->BindAction(CastAction, ETriggerEvent::Completed, this, &AWildCardPlayerController::HandleCast);
        EnhancedInputComponent->BindAction(AttackAction, ETriggerEvent::Completed, this, &AWildCardPlayerController::HandleAttack);
        EnhancedInputComponent->BindAction(CancelAction, ETriggerEvent::Completed, this, &AWildCardPlayerController::HandleCancel);
        EnhancedInputComponent->BindAction(SummonAction, ETriggerEvent::Completed, this, &AWildCardPlayerController::HandleSummon);
    }
}

void AWildCardPlayerController::OnPossess(APawn* InPawn)
{
    Super::OnPossess(InPawn);
    
    // Store the character pointer when we possess it
    WildCardCharacter = Cast<AWildCardCharacter>(InPawn);
    if (WildCardCharacter)
    {
        UE_LOG(LogTemp, Warning, TEXT("Successfully possessed a WildCardCharacter"));
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("Failed to cast possessed pawn to WildCardCharacter"));
    }
}

// And add these handler methods:
void AWildCardPlayerController::HandleJump()
{
    // UE_LOG(LogTemp, Warning, TEXT("Tried to Jump"));
    WildCardCharacter->Jump();
}

void AWildCardPlayerController::HandleStopJumping()
{
    WildCardCharacter->StopJumping();
}

void AWildCardPlayerController::HandleMove(const FInputActionValue& Value)
{
    // UE_LOG(LogTemp, Warning, TEXT("Tried to Move"));
    WildCardCharacter->Move(Value);
}

void AWildCardPlayerController::HandleLook(const FInputActionValue& Value)
{
    WildCardCharacter->Look(Value);
}

void AWildCardPlayerController::HandleSwitchTurn()
{
    UE_LOG(LogTemp, Warning, TEXT("Switching Turn"));
    WildCardGameState->NextTurn();
}

void AWildCardPlayerController::HandleCast()
{
    WildCardCharacter->FireBall();
}

void AWildCardPlayerController::HandleAttack()
{
    WildCardCharacter->Attack();
}

void AWildCardPlayerController::HandleCancel()
{
    WildCardCharacter->Cancel();
}

void AWildCardPlayerController::HandleSummon()
{
    WildCardCharacter->Summon();
}

