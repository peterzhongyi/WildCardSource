// Fill out your copyright notice in the Description page of Project Settings.


#include "WildCardPlayerController.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputMappingContext.h"
#include "InputAction.h"

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
}

void AWildCardPlayerController::BeginPlay()
{
    Super::BeginPlay();
    
    // Setup enhanced input system
    if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
    {
        Subsystem->AddMappingContext(DefaultMappingContext, 0);
    }
}

void AWildCardPlayerController::SetupInputComponent()
{
    Super::SetupInputComponent();
    UE_LOG(LogTemp, Warning, TEXT("SetupInputComponent is called"));
    
    // Set up input bindings
    if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(InputComponent))
    {
        UE_LOG(LogTemp, Warning, TEXT("Entered BindActions"));
        // Jumping
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Triggered, this, &AWildCardPlayerController::HandleJump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &AWildCardPlayerController::HandleStopJumping);

		// Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AWildCardPlayerController::HandleMove);

		// Looking
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AWildCardPlayerController::HandleLook);

        // Switch Turn
        EnhancedInputComponent->BindAction(SwitchTurnAction, ETriggerEvent::Completed, this, &AWildCardPlayerController::HandleSwitchTurn);
    }
}

void AWildCardPlayerController::OnPossess(APawn* InPawn)
{
    Super::OnPossess(InPawn);
    
    // Store the character pointer when we possess it
    WildCardCharacter = Cast<AWildCardCharacter>(InPawn);
    
    // Debug message to ensure the character was found and casted correctly
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
    UE_LOG(LogTemp, Warning, TEXT("Tried to Jump"));
    if (WildCardCharacter)
    {
        WildCardCharacter->Jump();
    }
}

void AWildCardPlayerController::HandleStopJumping()
{
    if (WildCardCharacter)
    {
        WildCardCharacter->StopJumping();
    }
}

void AWildCardPlayerController::HandleMove(const FInputActionValue& Value)
{
    UE_LOG(LogTemp, Warning, TEXT("Tried to Move"));
    if (WildCardCharacter)
    {
        WildCardCharacter->Move(Value);
    }
}

void AWildCardPlayerController::HandleLook(const FInputActionValue& Value)
{
    if (WildCardCharacter)
    {
        WildCardCharacter->Look(Value);
    }
}

void AWildCardPlayerController::HandleSwitchTurn()
{
    UE_LOG(LogTemp, Warning, TEXT("Switching Turn"));
    OnSwitchTurn.ExecuteIfBound();
}