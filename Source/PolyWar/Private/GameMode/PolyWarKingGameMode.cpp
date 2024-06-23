// Fill out your copyright notice in the Description page of Project Settings.


#include "GameMode/PolyWarKingGameMode.h"
#include "Character/PolyWarPlayerCharacter.h"

void APolyWarKingGameMode::CharacterDeath(APolyWarBaseCharacter* DeathCharacter)
{
	Super::CharacterDeath(DeathCharacter);

	APolyWarPlayerCharacter* PlayerCharacter = Cast<APolyWarPlayerCharacter>(DeathCharacter);
	if(PlayerCharacter)
	{
		ETeamType LoserTeam = PlayerCharacter->GetTeamType();
		if(LoserTeam == ETeamType::ET_RedTeam)
		{
			GameEnd(ETeamType::ET_BlueTeam);
		}
		else if(LoserTeam == ETeamType::ET_BlueTeam)
		{
			GameEnd(ETeamType::ET_RedTeam);
		}
	}
}
