// Copyright Epic Games, Inc. All Rights Reserved.

#include "WildCardCharacter.h"

#include "AIController.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/SpringArmComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Components/SceneComponent.h"
#include "NavigationSystem.h"
#include "NavMesh/RecastNavMesh.h"
#include "Detour/DetourNavMesh.h"
#include "NavMesh/RecastHelpers.h"


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
	GetCharacterMovement()->GravityScale = GravityScale;

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
    StaminaPerUnitDistance = 0.5f; // Adjust this to change stamina consumption rate
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
	static ConstructorHelpers::FClassFinder<ASummonStone> SummonStoneObj(TEXT("/Game/ThirdPerson/Blueprints/BP_SummonStone"));
	if (SummonStoneObj.Class != nullptr)
	{
		SummonStoneClass = SummonStoneObj.Class;
		UE_LOG(LogTemp, Warning, TEXT("SummonStoneClass set to %s"), *SummonStoneClass->GetName());
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
	GreatswordMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	// Store original camera settings (do this after CameraBoom is created)
	OriginalCameraArmLength = CameraBoom->TargetArmLength;
	OriginalCameraOffset = CameraBoom->SocketOffset;
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
	UE_LOG(LogTemp, Warning, TEXT("Sword collision state: %d"), 
			  (int32)GreatswordMesh->GetCollisionEnabled());

	AAIController* AIController = Cast<AAIController>(GetController());
	if (AIController != nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("AIController is not null"));
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("AIController is null"));
	}
	
	UE_LOG(LogTemp, Warning, TEXT("testing if code is reflected"));
}

void AWildCardCharacter::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
    
	// Only process stamina and update location if we're on the ground
	if (GetCharacterMovement()->IsMovingOnGround())
	{
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
			UE_LOG(LogTemp, Warning, TEXT("Update stamina to %f"), NewStamina);
		}

		PreviousLocation = CurrentLocation;
	}

	

	if ((bIsPreparingAttack || bIsPreparingJump) && Controller)
	{
		FRotator ControlRotation = Controller->GetControlRotation();
		FRotator NewRotation = FRotator(0, ControlRotation.Yaw, 0);
		SetActorRotation(NewRotation);

		// Update trajectory while aiming jump
		if (bIsPreparingJump)
		{
			CalculateJumpTrajectory();
		}
	}

	// Debug visualization
	if (bIsPreparingJump && TrajectoryResult.PathData.Num() > 1)
	{
		for (int32 i = 0; i < TrajectoryResult.PathData.Num() - 1; i++)
		{
			DrawDebugLine(
				GetWorld(),
				TrajectoryResult.PathData[i].Location,
				TrajectoryResult.PathData[i + 1].Location,
				FColor::Yellow,
				false,
				-1.0f,
				0,
				3.0f
			);
		}
        
		// Draw hit point if we hit something
		if (TrajectoryResult.HitResult.bBlockingHit)
		{
			DrawDebugSphere(
				GetWorld(),
				TrajectoryResult.HitResult.Location,
				20.0f,
				12,
				FColor::Red,
				false,
				-1.0f
			);
		}
	}
}

void AWildCardCharacter::Landed(const FHitResult& Hit)
{
	Super::Landed(Hit);

	// Immediately stop all horizontal movement
	UE_LOG(LogTemp, Warning, TEXT("Landed"));
	GetCharacterMovement()->StopMovementImmediately();

	PreviousLocation = GetActorLocation();

	if (IsEnemy && InTurn)
	{
		OnActionDone.Broadcast();
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
	if (!InTurn)
	{
		UE_LOG(LogTemp, Warning, TEXT("Can't move, not in turn"));
		return;
	}
	if (Stamina <= 0.f)
	{
		UE_LOG(LogTemp, Warning, TEXT("Can't move, no Stamina: %f"), Stamina);
		return;
	}

	if (bIsPreparingAttack)
	{
		UE_LOG(LogTemp, Warning, TEXT("Can't move, is preparing attack"));
		return;
	}

	if (bIsPreparingJump)
	{
		UE_LOG(LogTemp, Warning, TEXT("Can't move, is preparing jump"));
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

void AWildCardCharacter::EnableSwordCollision()
{
	if (GreatswordMesh != nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("SwordCollision Enabled"));
		GreatswordMesh->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("EnableSwordCollision: GreatswordMesh is null"));
	}
}

void AWildCardCharacter::DisableSwordCollision()
{
	if (GreatswordMesh != nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("SwordCollision Disabled"));
		GreatswordMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("DisableSwordCollision: GreatswordMesh is null"));
	}
}

void AWildCardCharacter::AttackEnded()
{
	UE_LOG(LogTemp, Warning, TEXT("Broadcasting AttackEnded event"));
	OnActionDone.Broadcast();
}

void AWildCardCharacter::FireBall()
{
	if (!InTurn)
	{
		UE_LOG(LogTemp, Warning, TEXT("Can't FireBall, not in turn"));
		return;
	}
	UE_LOG(LogTemp, Warning, TEXT("Calling FireBall"));
	FVector Location = ProjectileSpawnPoint->GetComponentLocation();
	FVector TargetLocation = FVector(1000.0, 1000.0, 50.0);
	float InitialSpeed = ProjectileClass->GetDefaultObject<AProjectile>()->ProjectileMovementComponent->InitialSpeed;
	FRotator RotationVector = GetLowerArcDirection(Location, TargetLocation, InitialSpeed, CharacterGravity);

	// TArray<FVector> potential_points = GetUniformNavMeshPoints(100.0);
	// for (const FVector& Point : potential_points)
	// {
	// 	DrawDebugSphere(
	// 		GetWorld(),
	// 		Point,
	// 		10.0f,           // Radius
	// 		4,              // Segments
	// 		FColor::Red,     // Color
	// 		false,           // Persistent lines
	// 		50.0f             // Lifetime in seconds
	// 	);
	// }
	
	AProjectile* Projectile = GetWorld()->SpawnActor<AProjectile>(ProjectileClass, Location, RotationVector);
	Projectile->SetOwner(this);
}

void AWildCardCharacter::Attack()
{
	UE_LOG(LogTemp, Warning, TEXT("Calling Attack"));
	
	if (!InTurn)
	{
		UE_LOG(LogTemp, Warning, TEXT("Can't Attack, not in turn"));
		return;
	}
	if (!bIsPreparingAttack)
	{
		// Enter prepare attack state
		bIsPreparingAttack = true;
		UE_LOG(LogTemp, Warning, TEXT("Entering prepare attack state"));
		return;
	}
	if (AttackMontage && GetMesh() && GetMesh()->GetAnimInstance())
	{
		GetMesh()->GetAnimInstance()->Montage_Play(AttackMontage);
		float NewStamina = FMath::Max(0.0f, Stamina - 30.0f);
		UpdateStamina(NewStamina);
	}
	bIsPreparingAttack = false;
}

// Since OnSwordOverlap is triggered by begin overlap, there's no need to de-dupe collision events.
void AWildCardCharacter::OnSwordOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	UE_LOG(LogTemp, Warning, TEXT("OnSwordOverlap Called"));
	if (OtherActor == this) return;

	AWildCardCharacter* OtherCharacter = Cast<AWildCardCharacter>(OtherActor);
	if (OtherCharacter == nullptr) return;
	
	if (OtherComp != OtherCharacter->GetCapsuleComponent()) return;
	UE_LOG(LogTemp, Warning, TEXT("Sword hit component: %s"), *OtherComp->GetName());

	OtherCharacter->Hit();
}

void AWildCardCharacter::Hit()
{
	UE_LOG(LogTemp, Warning, TEXT("Calling Hit"));
	if (HitMontage && GetMesh() && GetMesh()->GetAnimInstance())
	{
		GetMesh()->GetAnimInstance()->Montage_Play(HitMontage);
	}

	float CurrentHealth = GetHealth();
	float NewHealth = FMath::Max(0, CurrentHealth - 20.f);
	UpdateHealth(NewHealth);
}

void AWildCardCharacter::Cancel()
{
	if (bIsPreparingAttack)
	{
		bIsPreparingAttack = false;
	}
	if (bIsPreparingJump)
	{
		bIsPreparingJump = false;
		ClearTrajectory(); // Hide trajectory when canceling
		SetJumpCamera(false); // Reset camera
	}
}

void AWildCardCharacter::Summon()
{
	if (!InTurn)
	{
		UE_LOG(LogTemp, Warning, TEXT("Can't Summon, not in turn"));
		return;
	}
	UE_LOG(LogTemp, Warning, TEXT("Calling Summon"));
	FVector Location = ProjectileSpawnPoint->GetComponentLocation();
	FRotator Rotation = GetControlRotation();
	ASummonStone* SummonStone = GetWorld()->SpawnActor<ASummonStone>(SummonStoneClass, Location, Rotation);
	SummonStone->SetOwner(this);
}

bool AWildCardCharacter::CalculateProjectileLaunchRotation(
    const FVector& StartPoint,
    const FVector& TargetPoint,
    float InitialSpeed,
    float Gravity,
    FRotator& OutRotation)
{
	// Calculate displacement vector
	FVector Displacement = TargetPoint - StartPoint;
    
	// Calculate horizontal distance (XY plane)
	float HorizontalDistance = FMath::Sqrt(Displacement.X * Displacement.X + Displacement.Y * Displacement.Y);
    
	// Calculate vertical displacement (Z axis)
	float VerticalDisplacement = Displacement.Z;
    
	float x = HorizontalDistance;
	float y = VerticalDisplacement;
	float v0_squared = InitialSpeed * InitialSpeed;
	float g = Gravity;
	
	float a = (g * g) / 4.0;
	float b = y * g - v0_squared;
	float c = x * x + y * y;
    
	// Calculate discriminant
	float discriminant = b * b - 4.0f * a * c;
    
	// Check if solution exists
	if (discriminant < 0.0f)
	{
		// UE_LOG(LogTemp, Error, TEXT("discriminant is less than 0!"));
		return false;
	}

	float sqrt_discriminant = FMath::Sqrt(discriminant);
	float T1 = (-b + sqrt_discriminant) / (2.0f * a);
	float T2 = (-b - sqrt_discriminant) / (2.0f * a);
	float t1 = FMath::Sqrt(T1);
	float t2 = FMath::Sqrt(T2);

	if (t1 <= 0 || t2 <= 0)
	{
		UE_LOG(LogTemp, Error, TEXT("either t1 or t2 is 0"));
	}

	float vx = x / t2;
	float vy = (y / t2) + 0.5 * (g * t2);
	
	float launchAngle = FMath::Acos(vx / InitialSpeed);
	if (vy < 0)
	{
		launchAngle = 2 * UE_PI - launchAngle;
	}
    
	// Calculate azimuth angle (horizontal direction)
	float azimuthAngle = FMath::Atan2(Displacement.Y, Displacement.X);
    
	// Convert to degrees and create rotation
	float pitchDegrees = FMath::RadiansToDegrees(launchAngle);
	float yawDegrees = FMath::RadiansToDegrees(azimuthAngle);
    
	OutRotation = FRotator(pitchDegrees, yawDegrees, 0.0f);
    
	return true;
}

FRotator AWildCardCharacter::GetLowerArcDirection(FVector StartPoint, FVector TargetPoint, float InitialSpeed, float Gravity)
{
	FRotator OutRotation;
	bool bSuccess = CalculateProjectileLaunchRotation(
		StartPoint, TargetPoint, InitialSpeed, Gravity, OutRotation);
    
	if (!bSuccess)
	{
		// Return zero rotation if no valid trajectory
		UE_LOG(LogTemp, Warning, TEXT("No valid projectile trajectory found!"));
		return FRotator::ZeroRotator;
	}
    
	return OutRotation;
}

void AWildCardCharacter::Jump()
{
	if (!InTurn)
	{
		UE_LOG(LogTemp, Warning, TEXT("Can't Jump, not in turn"));
		return;
	}
    
	if (!bIsPreparingJump)
	{
		// Enter prepare jump state
		bIsPreparingJump = true;
		CalculateJumpTrajectory();
		SetJumpCamera(true); // Adjust camera for aiming
		UE_LOG(LogTemp, Warning, TEXT("Entering prepare jump state"));
		return;
	}
    
	// Execute the jump
	if (!Controller)
	{
		return;
	}
	
	FRotator ControlRotation = Controller->GetControlRotation();
	ControlRotation.Pitch = ControlRotation.Pitch * 2.0 + 45.0;
	FVector LaunchDirection = ControlRotation.Vector();
	ActualJump(LaunchDirection * JumpSpeed);
	
	bIsPreparingJump = false;
	ClearTrajectory();
	SetJumpCamera(false);
}

void AWildCardCharacter::ActualJump(FVector Velocity)
{
	LaunchCharacter(Velocity, false, false);
	float NewStamina = FMath::Max(0.0f, Stamina - 20.0f);
	UpdateStamina(NewStamina);
}

TArray<FVector> AWildCardCharacter::GetUniformNavMeshPoints(float GridSpacing)
{
	TArray<FVector> UniformPoints;
    
	// Get navigation system
	UNavigationSystemV1* NavSys = FNavigationSystem::GetCurrent<UNavigationSystemV1>(GetWorld());
	if (!NavSys)
	{
		return UniformPoints;
	}
    
	ARecastNavMesh* NavMesh = Cast<ARecastNavMesh>(NavSys->GetDefaultNavDataInstance());
	if (!NavMesh)
	{
		return UniformPoints;
	}
    
	// Get nav mesh bounds
	FBox NavBounds = NavMesh->GetNavMeshBounds();

	float SpawnPointOffset = ProjectileSpawnPoint->GetRelativeLocation().Z;
    
	// Sample points in a grid pattern
	for (float X = NavBounds.Min.X; X <= NavBounds.Max.X; X += GridSpacing)
	{
		for (float Y = NavBounds.Min.Y; Y <= NavBounds.Max.Y; Y += GridSpacing)
		{
			FVector TestPoint = FVector(X, Y, NavBounds.Max.Z + 100.0f); // Start above nav mesh
            
			// Project point down to nav mesh
			FNavLocation NavLocation;
			if (NavSys->ProjectPointToNavigation(TestPoint, NavLocation, FVector(100.0f, 100.0f, 1000000.0f)))
			{
				// FRotator OutRotation;
				// if (CalculateProjectileLaunchRotation(NavLocation.Location + FVector(0, 0, SpawnPointOffset), TargetPoint, InitialSpeed, Gravity, false, OutRotation))
				// {
				// 	UniformPoints.Add(NavLocation.Location);
				// }
				UniformPoints.Add(NavLocation.Location);
			}
		}
	}
    
	return UniformPoints;
}

void AWildCardCharacter::CalculateJumpTrajectory()
{
	if (!Controller)
		return;
    
	// Get launch parameters
	FVector ActorLocation = GetActorLocation();  // Center of capsule (pelvis level)
	float CapsuleHalfHeight = GetCapsuleComponent()->GetScaledCapsuleHalfHeight();
	FVector StartLocation = ActorLocation - FVector(0, 0, CapsuleHalfHeight * 0.99);  // Bottom of capsule (feet level)
	
	FRotator ControlRotation = Controller->GetControlRotation();
	ControlRotation.Pitch = ControlRotation.Pitch * 2.0 + 45.0;
	FVector LaunchDirection = ControlRotation.Vector();
	
	FVector LaunchVelocity = LaunchDirection * JumpSpeed;
    
	// Set up prediction parameters
	FPredictProjectilePathParams PredictParams;
	PredictParams.StartLocation = StartLocation;
	PredictParams.LaunchVelocity = LaunchVelocity;
	PredictParams.bTraceWithCollision = true;
	PredictParams.MaxSimTime = MaxSimTime;
	PredictParams.ProjectileRadius = 0.1f; // Use actual radius instead of 0
	PredictParams.bTraceComplex = false;   // Use simple collision
	PredictParams.ActorsToIgnore.Add(this);
	PredictParams.DrawDebugTime = 0.0f; // Set to > 0 for debug visualization

	// Add collision detection
	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
	ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_WorldStatic));  // Ground/walls
	ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_Pawn));        // Characters
	PredictParams.ObjectTypes = ObjectTypes;
    
	// Predict the path
	bool bHit = UGameplayStatics::PredictProjectilePath(
		GetWorld(),
		PredictParams,
		TrajectoryResult
	);
}

void AWildCardCharacter::ClearTrajectory()
{
	TrajectoryResult = FPredictProjectilePathResult();
}

void AWildCardCharacter::SetJumpCamera(bool bEnableJumpCamera)
{
	if (!CameraBoom)
		return;
        
	if (bEnableJumpCamera)
	{
		// Move camera to the right and zoom in for better arc visibility
		CameraBoom->SocketOffset = FVector(0, JumpCameraOffset, 0); // Move right
		CameraBoom->TargetArmLength = JumpCameraArmLength; // Zoom closer
	}
	else
	{
		// Reset to original camera settings
		CameraBoom->SocketOffset = OriginalCameraOffset;
		CameraBoom->TargetArmLength = OriginalCameraArmLength;
	}
}


