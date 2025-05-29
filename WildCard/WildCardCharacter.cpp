// Copyright Epic Games, Inc. All Rights Reserved.

#include "WildCardCharacter.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/SpringArmComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Components/SceneComponent.h"


//////////////////////////////////////////////////////////////////////////
// AWildCardCharacter

AWildCardCharacter::AWildCardCharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	RootComponent = GetCapsuleComponent();
		
	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f); // ...at this rotation rate

	// Note: For faster iteration times these variables, and many more, can be tweaked in the Character Blueprint
	// instead of recompiling to adjust them
	GetCharacterMovement()->JumpZVelocity = 400.f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = 380.f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 400.0f; // The camera follows at this distance behind the character	
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	// Set up initial character stats
	MaxStamina = 100.0f;
    Stamina = MaxStamina;
    StaminaPerUnitDistance = 0.03f; // Adjust this to change stamina consumption rate
	MaxHealth = 100.0f;
	Health = MaxHealth;

	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named ThirdPersonCharacter (to avoid direct content references in C++)

	static ConstructorHelpers::FClassFinder<AProjectile> ProjectileObj(TEXT("/Game/ThirdPerson/Blueprints/BP_Projectile"));
    if (ProjectileObj.Class != nullptr)
    {
        ProjectileClass = ProjectileObj.Class;
        UE_LOG(LogTemp, Warning, TEXT("ProjectileClass set to %s"), *ProjectileClass->GetName());
    }

	ProjectileSpawnPoint = CreateDefaultSubobject<USceneComponent>(TEXT("ProjectileSpawnPoint"));
	ProjectileSpawnPoint->SetupAttachment(RootComponent);

	HealthBarWidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("HealthBarWidgetComponent"));
	HealthBarWidgetComponent->SetupAttachment(RootComponent);
	HealthBarWidgetComponent->SetRelativeLocation(FVector(0.0f, 0.0f, 120.0f)); // Adjust position above character
	HealthBarWidgetComponent->SetWidgetSpace(EWidgetSpace::Screen);
	HealthBarWidgetComponent->SetDrawSize(FVector2D(150.0f, 15.0f)); 

	static ConstructorHelpers::FClassFinder<UWildCardUserWidget> OverHeadHeathBarObj(TEXT("/Game/ThirdPerson/Blueprints/WBP_Health"));
	if (OverHeadHeathBarObj.Class != nullptr)
	{
		OverHeadHealthBarClass = OverHeadHeathBarObj.Class;
		UE_LOG(LogTemp, Warning, TEXT("OverHeadHealthBarClass set to %s"), *OverHeadHealthBarClass->GetName());
	}

	GreatswordMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Greatsword"));
	GreatswordMesh->SetupAttachment(GetMesh(), FName(TEXT("hand_l")));
	
}

void AWildCardCharacter::BeginPlay()
{
	// Call the base class  
	Super::BeginPlay();
    PreviousLocation = GetActorLocation();

	if (OverHeadHealthBarClass != nullptr && HealthBarWidgetComponent != nullptr)
	{
		HealthBarWidgetComponent->SetWidgetClass(OverHeadHealthBarClass);
		OverHeadHealthBar = Cast<UWildCardUserWidget>(HealthBarWidgetComponent->GetUserWidgetObject());
		if (OverHeadHealthBar)
		{
			UE_LOG(LogTemp, Warning, TEXT("Refreshing OverHeadHealthBar"));
			OverHeadHealthBar->UpdateMaxHealth(MaxHealth);
			OverHeadHealthBar->UpdateHealth(Health);
			OverHeadHealthBar->UpdateOverHeadHealthBar();
		}
	}

	GreatswordMesh->OnComponentBeginOverlap.AddDynamic(this, &AWildCardCharacter::OnSwordOverlap);
}

void AWildCardCharacter::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
    
    // Get current location
    FVector CurrentLocation = GetActorLocation();
    
    // Calculate distance moved since last tick (in Unreal units)
    float DistanceMoved = FVector::Dist2D(CurrentLocation, PreviousLocation);
    
    // Only consume stamina if we've moved a meaningful distance
    if (DistanceMoved > 1.0f)
    {
        // Decrease stamina based on distance moved
        float StaminaDecrease = DistanceMoved * StaminaPerUnitDistance;
		float NewStamina = FMath::Max(0.0f, Stamina - StaminaDecrease);
        UpdateStamina(NewStamina);
        
        // Optional: Log stamina for debugging
        // UE_LOG(LogTemp, Warning, TEXT("Distance: %f, Stamina: %f"), DistanceMoved, Stamina);
        
        // Update previous location
        PreviousLocation = CurrentLocation;
    }
}

void AWildCardCharacter::UpdateStamina(float NewStamina)
{
	Stamina = NewStamina;
	OnStaminaChanged.Broadcast(NewStamina);
}

void AWildCardCharacter::UpdateHealth(float NewHealth)
{
	Health = NewHealth;
	UE_LOG(LogTemp, Warning, TEXT("Character Health updated %f"), Health);
	// Broadcast event
	OnHealthChanged.Broadcast(NewHealth);
	if (OverHeadHealthBar != nullptr)
	{
		OverHeadHealthBar->UpdateHealth(Health);
		OverHeadHealthBar->UpdateOverHeadHealthBar();
	}
}

float AWildCardCharacter::GetHealth()
{
	return Health;
}

void AWildCardCharacter::Move(const FInputActionValue& Value)
{
	if (Stamina <= 0.f) {
		UE_LOG(LogTemp, Warning, TEXT("Can't move, no Stamina: %f"), Stamina);
		return;
	}
	// input is a Vector2D
	FVector2D MovementVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	
		// get right vector 
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		// add movement 
		AddMovementInput(ForwardDirection, MovementVector.Y);
		AddMovementInput(RightDirection, MovementVector.X);
	}
}

void AWildCardCharacter::Look(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// add yaw and pitch input to controller
		AddControllerYawInput(LookAxisVector.X);
		AddControllerPitchInput(LookAxisVector.Y);
	}
}

void AWildCardCharacter::FireBall()
{
	UE_LOG(LogTemp, Warning, TEXT("Calling FireBall"));
	FVector Location = ProjectileSpawnPoint->GetComponentLocation();
	FRotator Rotation = GetControlRotation();
	AProjectile* Projectile = GetWorld()->SpawnActor<AProjectile>(ProjectileClass, Location, Rotation);
	Projectile->SetOwner(this);
}

void AWildCardCharacter::Attack()
{
	UE_LOG(LogTemp, Warning, TEXT("Calling Attack version 2"));
	if (AttackMontage && GetMesh() && GetMesh()->GetAnimInstance())
	{
		GetMesh()->GetAnimInstance()->Montage_Play(AttackMontage);
	}
}

void AWildCardCharacter::OnSwordOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor == this) return;

	AWildCardCharacter* OtherCharacter = Cast<AWildCardCharacter>(OtherActor);
	if (OtherCharacter == nullptr) return;
	
	if (OtherComp != OtherCharacter->GetCapsuleComponent()) return;
	UE_LOG(LogTemp, Warning, TEXT("Sword hit component: %s"), *OtherComp->GetName());

	
	UE_LOG(LogTemp, Warning, TEXT("OnSwordHit Called"));
}



