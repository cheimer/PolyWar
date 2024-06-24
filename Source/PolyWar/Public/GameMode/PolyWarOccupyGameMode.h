// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameMode/PolyWarGameModeBase.h"
#include "PolyWarOccupyGameMode.generated.h"

/**
 * 
 */
UCLASS()
class POLYWAR_API APolyWarOccupyGameMode : public APolyWarGameModeBase
{
	GENERATED_BODY()

public:
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, Category = "Set Should")
	float MaxOccupyTime = 600.0f;

private:
	TArray<class AOccupyArea*> OccupyAreas;

	float BlueTeamOccupyTime = 0.0f;
	float RedTeamOccupyTime = 0.0f;

	UFUNCTION()
	void AddOccupyTime(ETeamType Team);
};
