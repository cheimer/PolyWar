// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "Perception/AIPerceptionTypes.h"
#include "PolyWarAIController.generated.h"

struct FActorPerceptionUpdateInfo;
/**
 * 
 */
UCLASS()
class POLYWAR_API APolyWarAIController : public AAIController
{
	GENERATED_BODY()

public:
	APolyWarAIController();

	bool IsEnemyInSight();
	AActor* GetClosestEnemy();

protected:
	virtual void BeginPlay() override;
	virtual void OnPossess(APawn* InPawn) override;

	UPROPERTY(EditDefaultsOnly, Category = "Set Should")
	UBehaviorTree* BehaviorTree;

private:
	TObjectPtr<class APolyWarAICharacter> PolyWarAICharacter;

	TArray<class APolyWarBaseCharacter*> InSightEnemies;

	UFUNCTION()
	void OnPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus);

	UFUNCTION()
	void OnPerceptedActorDeath(class APolyWarBaseCharacter* DeathCharacter);

};
