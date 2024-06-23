// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameMode/PolyWarGameModeBase.h"
#include "PolyWarKingGameMode.generated.h"

/**
 * 
 */
UCLASS()
class POLYWAR_API APolyWarKingGameMode : public APolyWarGameModeBase
{
	GENERATED_BODY()

protected:
	virtual void CharacterDeath(APolyWarBaseCharacter* DeathCharacter) override;

};