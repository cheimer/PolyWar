// Fill out your copyright notice in the Description page of Project Settings.


#include "GameState/PolyWarGameStateBase.h"

#include "Character/PolyWarAICharacter.h"
#include "Kismet/GameplayStatics.h"

void APolyWarGameStateBase::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	TArray<AActor*> AICharacterArray;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), APolyWarAICharacter::StaticClass(), AICharacterArray);

	// Initialize TeamUnitMap

	for(auto Index : AICharacterArray)
	{
		APolyWarAICharacter* AICharacter = Cast<APolyWarAICharacter>(Index);
		if(AICharacter)
		{
			AICharacter->OnCharacterDeathDelegate.AddDynamic(this, &ThisClass::DeathCharacter);

			if(AICharacter->GetTeamType() == ETeamType::ET_BlueTeam)
			{
				if(BlueTeamUnitMap.Contains(AICharacter->GetUnitNum()))
				{
					BlueTeamUnitMap[AICharacter->GetUnitNum()].Add(AICharacter);
				}
				else
				{
					TArray<APolyWarAICharacter*> CreateArray;
					CreateArray.Emplace(AICharacter);
					BlueTeamUnitMap.Emplace(AICharacter->GetUnitNum(), CreateArray);
				}
			}
			else if (AICharacter->GetTeamType() == ETeamType::ET_RedTeam)
			{
				if(RedTeamUnitMap.Contains(AICharacter->GetUnitNum()))
				{
					RedTeamUnitMap[AICharacter->GetUnitNum()].Add(AICharacter);
				}
				else
				{
					TArray<APolyWarAICharacter*> CreateArray;
					CreateArray.Emplace(AICharacter);
					RedTeamUnitMap.Emplace(AICharacter->GetUnitNum(), CreateArray);
				}
			}
			else
			{
				//AICharacter->OnCharacterDeathDelegate.RemoveDynamic(this, &ThisClass::DeathCharacter);
			}
		}
	}

}

void APolyWarGameStateBase::GetTeamArray(ETeamType TeamType, TArray<EUnitNum> UnitNumArray, TArray<APolyWarAICharacter*>& OutArray)
{
	if(TeamType == ETeamType::ET_BlueTeam)
	{
		for(auto Index : UnitNumArray)
		{
			if(BlueTeamUnitMap.Contains(Index))
			{
				OutArray.Append(BlueTeamUnitMap[Index]);
			}
		}
	}
	else if(TeamType == ETeamType::ET_RedTeam)
	{
		for(auto Index : UnitNumArray)
		{
			if(RedTeamUnitMap.Contains(Index))
			{
				OutArray.Append(RedTeamUnitMap[Index]);
			}
		}
	}
}

void APolyWarGameStateBase::DeathCharacter(APolyWarBaseCharacter* Character)
{
	APolyWarAICharacter* AICharacter = Cast<APolyWarAICharacter>(Character);
	if(AICharacter)
	{
		if(AICharacter->GetTeamType() == ETeamType::ET_BlueTeam)
		{
			if(BlueTeamUnitMap.Contains(AICharacter->GetUnitNum()) && BlueTeamUnitMap[AICharacter->GetUnitNum()].Contains(AICharacter))
			{
				BlueTeamUnitMap[AICharacter->GetUnitNum()].Remove(AICharacter);
			}
		}
		else if(AICharacter->GetTeamType() == ETeamType::ET_RedTeam)
		{
			if(RedTeamUnitMap.Contains(AICharacter->GetUnitNum()) && RedTeamUnitMap[AICharacter->GetUnitNum()].Contains(AICharacter))
			{
				RedTeamUnitMap[AICharacter->GetUnitNum()].Remove(AICharacter);
			}
		}
	}
}
