// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameMode/PolyWarGameModeBase.h"
#include "PolyWarExterminateGameMode.generated.h"

/**
 * 
 */
UCLASS()
class POLYWAR_API APolyWarExterminateGameMode : public APolyWarGameModeBase
{
	GENERATED_BODY()

public:
	virtual void BeginPlay() override;

protected:
	virtual void CharacterDeath(APolyWarBaseCharacter* DeathCharacter) override;

private:
	int32 BlueTeamNum = 1;
	int32 RedTeamNum = 1;

};
