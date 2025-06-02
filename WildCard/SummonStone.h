// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Projectile.h"
#include "SummonStone.generated.h"

/**
 * 
 */
UCLASS()
class WILDCARD_API ASummonStone : public AProjectile
{
	GENERATED_BODY()

public:	
	ASummonStone();

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	// Override the OnHit function to handle bouncing
	virtual void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit) override;

private:
	// Timer to track how long the stone has been stationary
	float StationaryTime;
	
	// Threshold for considering the stone "not moving"
	UPROPERTY(EditDefaultsOnly, Category = "SummonStone")
	float MinVelocityThreshold = 10.0f;
	
	// Time before destruction when stationary
	UPROPERTY(EditDefaultsOnly, Category = "SummonStone")
	float DestructionDelay = 1.0f;
	
	// Track previous location for velocity calculation
	FVector PreviousLocation;
};
