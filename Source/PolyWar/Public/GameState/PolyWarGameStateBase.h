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

	void RegisterPlayer(class APolyWarPlayerCharacter* PlayerCharacter);

	void GetTeam(ETeamType TeamType, TArray<class APolyWarBaseCharacter*>& OutTeamArray);
	void GetTeamByUnitNums(ETeamType TeamType, TArray<EUnitNum> UnitNumArray, TArray<class APolyWarAICharacter*>& OutArray);

	bool IsTeamExistPlayer(ETeamType TeamType);

	ETeamType GetLocalPlayerTeam();

	UFUNCTION()
	void DeathCharacter(class APolyWarBaseCharacter* Character);

private:
	TMap<EUnitNum, TArray<APolyWarAICharacter*>> BlueTeamUnitMap;
	TMap<EUnitNum, TArray<APolyWarAICharacter*>> RedTeamUnitMap;

	UPROPERTY()
	APolyWarPlayerCharacter* BluePlayer;
	UPROPERTY()
	APolyWarPlayerCharacter* RedPlayer;

};
