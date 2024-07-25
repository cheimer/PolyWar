// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/PolyWarAICharacter.h"

#include "BrainComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SphereComponent.h"
#include "Controller/PolyWarAIController.h"

APolyWarAICharacter::APolyWarAICharacter()
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.TickInterval = 0.1f;

	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;

	HighlightMesh = CreateDefaultSubobject<UStaticMeshComponent>("HighlightMesh");
	HighlightMesh->SetupAttachment(GetRootComponent());
	HighlightMesh->SetGenerateOverlapEvents(false);
	HighlightMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	HighlightMesh->SetCollisionResponseToAllChannels(ECR_Ignore);
	HighlightMesh->SetVisibility(false);
}

void APolyWarAICharacter::BeginPlay()
{
	Super::BeginPlay();

	StartOrder(EOrderType::EOD_Hold, GetActorLocation());

	if(GetCapsuleComponent())
	{
		GetCapsuleComponent()->SetComponentTickInterval(0.2f);
	}
	if(VisibleSphere)
	{
		VisibleSphere->SetComponentTickInterval(0.5f);
	}
}

void APolyWarAICharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	PolyWarAIController = PolyWarAIController == nullptr ? Cast<APolyWarAIController>(GetController()) : PolyWarAIController;
	if(PolyWarAIController && !GetIsAttacking() && !IsDead())
	{
		AActor* ClosestEnemy = PolyWarAIController->GetClosestEnemy();
		if(ClosestEnemy)
		{
			const FVector ToVector = ClosestEnemy->GetActorLocation();
			const FVector FromVector = GetActorLocation();
			const FRotator TargetRotator = (ToVector - FromVector).Rotation();
			const FRotator TargetYawRotator = FRotator(0.0f, TargetRotator.Yaw, 0.0f);

			SetActorRotation(FMath::RInterpTo(GetActorRotation(), TargetYawRotator, DeltaSeconds, 5.0f));
		}
	}
}

void APolyWarAICharacter::StartOrder(EOrderType Order, FVector OrderPos)
{
	if(HasAuthority())
	{
		SetDestination(OrderPos);

		LastOrder = Order;
		LastOrderPos = OrderPos;
	}
}

void APolyWarAICharacter::SetDestination(FVector OrderPos)
{
	PolyWarAIController = PolyWarAIController == nullptr ? Cast<APolyWarAIController>(GetController()) : PolyWarAIController;
	if(!PolyWarAIController || !PolyWarAIController->GetBlackboardComponent()) return;

	PolyWarAIController->GetBlackboardComponent()->SetValueAsVector("Destination", OrderPos);
}

void APolyWarAICharacter::SetPlayerDeath()
{
	Super::SetPlayerDeath();

	PolyWarAIController = PolyWarAIController == nullptr ? Cast<APolyWarAIController>(GetController()) : PolyWarAIController;
	if(!PolyWarAIController || !PolyWarAIController->GetBrainComponent()) return;

	PolyWarAIController->GetBrainComponent()->StopLogic("PlayerDeath");
}

void APolyWarAICharacter::SetHighlight(bool bEnable)
{
	if(!HighlightMesh) return;

	HighlightMesh->SetVisibility(bEnable);
}
