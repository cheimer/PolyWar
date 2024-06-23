// Fill out your copyright notice in the Description page of Project Settings.


#include "GameMode/PolyWarExterminateGameMode.h"

#include "Character/PolyWarBaseCharacter.h"
#include "GameState/PolyWarGameStateBase.h"

void APolyWarExterminateGameMode::BeginPlay()
{
	Super::BeginPlay();

	PolyWarGameState = PolyWarGameState == nullptr ? GetGameState<APolyWarGameStateBase>() : PolyWarGameState;
	if(PolyWarGameState)
	{
		TArray<APolyWarBaseCharacter*> BlueTeamArray, RedTeamArray;
		PolyWarGameState->GetTeam(ETeamType::ET_BlueTeam, BlueTeamArray);
		PolyWarGameState->GetTeam(ETeamType::ET_RedTeam, RedTeamArray);

		BlueTeamNum += BlueTeamArray.Num();
		RedTeamNum += RedTeamArray.Num();
	}
}

void APolyWarExterminateGameMode::CharacterDeath(APolyWarBaseCharacter* DeathCharacter)
{
	Super::CharacterDeath(DeathCharacter);

	if(!DeathCharacter) return;
	if(DeathCharacter->GetTeamType() == ETeamType::ET_BlueTeam)
	{
		BlueTeamNum--;
	}
	else if(DeathCharacter->GetTeamType() == ETeamType::ET_RedTeam)
	{
		RedTeamNum--;
	}

	if(BlueTeamNum <= 0)
	{
		GameEnd(ETeamType::ET_RedTeam);
	}
	else if(RedTeamNum <= 0)
	{
		GameEnd(ETeamType::ET_BlueTeam);
	}
}
