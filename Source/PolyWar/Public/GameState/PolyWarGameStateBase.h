// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "PolyWarTypes/TeamType.h"
#include "PolyWarTypes/UnitNum.h"
#include "PolyWarGameStateBase.generated.h"

/**
 * 
 */
UCLASS()
class POLYWAR_API APolyWarGameStateBase : public AGameStateBase
{
	GENERATED_BODY()

public:
	virtual void PostInitializeComponents() override;

	virtual void BeginPlay() override;

	void SetPlayerTeam(class APolyWarPlayerCharacter* PlayerCharacter);

	void GetTeamArray(ETeamType TeamType, TArray<EUnitNum> UnitNumArray, TArray<class APolyWarAICharacter*>& OutArray);

	void GetTeam(ETeamType TeamType, TArray<class APolyWarBaseCharacter*>& OutTeamArray, bool bIncludePlayer = false);

private:
	TMap<EUnitNum, TArray<APolyWarAICharacter*>> BlueTeamUnitMap;
	TMap<EUnitNum, TArray<APolyWarAICharacter*>> RedTeamUnitMap;

	TObjectPtr<class APolyWarPlayerCharacter> BluePlayer = nullptr;
	TObjectPtr<APolyWarPlayerCharacter> RedPlayer = nullptr;

	UFUNCTION()
	void DeathCharacter(class APolyWarBaseCharacter* Character);

};
