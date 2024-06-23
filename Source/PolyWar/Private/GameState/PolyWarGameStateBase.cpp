// Fill out your copyright notice in the Description page of Project Settings.


#include "GameState/PolyWarGameStateBase.h"

#include "Character/PolyWarAICharacter.h"
#include "Character/PolyWarPlayerCharacter.h"
#include "Controller/PolyWarPlayerController.h"
#include "GameFramework/PlayerState.h"
#include "GameMode/PolyWarGameModeBase.h"
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
			// if use other team. not Blue or Red
			else
			{
				//AICharacter->OnCharacterDeathDelegate.RemoveDynamic(this, &ThisClass::DeathCharacter);
			}
		}
	}
}

void APolyWarGameStateBase::BeginPlay()
{
	Super::BeginPlay();
}

void APolyWarGameStateBase::RegisterPlayer(APolyWarPlayerCharacter* PlayerCharacter)
{
	if(!PlayerCharacter) return;

	if(!BluePlayer && PlayerCharacter->GetTeamType() == ETeamType::ET_BlueTeam)
	{
		BluePlayer = PlayerCharacter;
		if(HasAuthority())
		{
			PlayerCharacter->OnCharacterDeathDelegate.AddDynamic(this, &ThisClass::DeathCharacter);
		}
	}
	else if(!RedPlayer && PlayerCharacter->GetTeamType() == ETeamType::ET_RedTeam)
	{
		RedPlayer = PlayerCharacter;
		if(HasAuthority())
		{
			PlayerCharacter->OnCharacterDeathDelegate.AddDynamic(this, &ThisClass::DeathCharacter);
		}
	}
	else
	{
		// if Player respawned, fill out that case
	}

}

bool APolyWarGameStateBase::IsTeamExistPlayer(ETeamType TeamType)
{
	if(BluePlayer && TeamType == ETeamType::ET_BlueTeam)
	{
		return true;
	}
	else if(RedPlayer && TeamType == ETeamType::ET_RedTeam)
	{
		return true;
	}

	return false;
}

void APolyWarGameStateBase::GetTeam(ETeamType TeamType, TArray<APolyWarBaseCharacter*>& OutTeamArray)
{
	if(TeamType == ETeamType::ET_BlueTeam)
	{
		for(EUnitNum Index : TEnumRange<EUnitNum>())
		{
			if(BlueTeamUnitMap.Contains(Index))
			{
				for(auto IndexCharacter : BlueTeamUnitMap[Index])
				{
					OutTeamArray.Add(IndexCharacter);
				}
			}
		}
	}
	else if(TeamType == ETeamType::ET_RedTeam)
	{
		for(EUnitNum Index : TEnumRange<EUnitNum>())
		{
			if(RedTeamUnitMap.Contains(Index))
			{
				for(auto IndexCharacter : RedTeamUnitMap[Index])
				{
					OutTeamArray.Add(IndexCharacter);
				}
			}
		}
	}
}

void APolyWarGameStateBase::GetTeamByUnitNums(ETeamType TeamType, TArray<EUnitNum> UnitNumArray, TArray<APolyWarAICharacter*>& OutArray)
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
	// Player Death
	APolyWarPlayerCharacter* PlayerCharacter = Cast<APolyWarPlayerCharacter>(Character);
	if(PlayerCharacter)
	{
		if(PlayerCharacter == BluePlayer)
		{
			BluePlayer = nullptr;
		}
		else if(PlayerCharacter == RedPlayer)
		{
			RedPlayer = nullptr;
		}
	}

	// AI Death
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

	APolyWarPlayerController* PolyWarPlayerController = Cast<APolyWarPlayerController>(PlayerArray[0]->GetOwner());
	if(PolyWarPlayerController)
	{
		PolyWarPlayerController->SetHUDDeathCharacter(Character);
	}

	APolyWarGameModeBase* GameMode = Cast<APolyWarGameModeBase>(GetWorld()->GetAuthGameMode());
	if(GameMode)
	{
		GameMode->CharacterDeath(Character);
	}

}
