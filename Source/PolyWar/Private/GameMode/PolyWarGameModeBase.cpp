// Copyright Epic Games, Inc. All Rights Reserved.


#include "GameMode/PolyWarGameModeBase.h"

#include "Character/PolyWarPlayerCharacter.h"
#include "Controller/PolyWarPlayerController.h"
#include "GameState/PolyWarGameStateBase.h"
#include "Kismet/GameplayStatics.h"

APolyWarGameModeBase::APolyWarGameModeBase()
{
	PrimaryActorTick.bCanEverTick = true;

	GameStateClass = APolyWarGameStateBase::StaticClass();
	PlayerControllerClass = nullptr;
	HUDClass = nullptr;
	DefaultPawnClass = nullptr;

	bUseSeamlessTravel = true;
}

FString APolyWarGameModeBase::InitNewPlayer(APlayerController* NewPlayerController,
	const FUniqueNetIdRepl& UniqueId, const FString& Options, const FString& Portal)
{
	UE_LOG(LogTemp, Warning, TEXT("Time GameModeInit"));
	PolyWarGameState = PolyWarGameState == nullptr ? GetGameState<APolyWarGameStateBase>() : PolyWarGameState;
	if(PolyWarGameState)
	{
		FString Team;
		if(PolyWarGameState->IsTeamExistPlayer(ETeamType::ET_BlueTeam))
		{
			Team = "RedTeam";
		}
		else
		{
			Team = "BlueTeam";
		}

		return Super::InitNewPlayer(NewPlayerController, UniqueId, Options, Team);
	}

	return Super::InitNewPlayer(NewPlayerController, UniqueId, Options, Portal);
}

void APolyWarGameModeBase::CharacterDeath(APolyWarBaseCharacter* DeathCharacter)
{

}

void APolyWarGameModeBase::GameEnd(ETeamType WinnerTeam)
{
	if(bIsGameEnd) return;
	bIsGameEnd = true;

	TArray<AActor*> AllController;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), APolyWarPlayerController::StaticClass(), AllController);

	for(AActor* ControllerIndex : AllController)
	{
		APolyWarPlayerController* PolyWarController = Cast<APolyWarPlayerController>(ControllerIndex);
		if(PolyWarController)
		{
			PolyWarController->GameEnd(WinnerTeam);
		}
	}
}

void APolyWarGameModeBase::TimeEnd()
{
	TArray<AActor*> AllCharacters;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), APolyWarBaseCharacter::StaticClass(), AllCharacters);

	int32 BlueTeamNum = 0, RedTeamNum = 0;

	for(AActor* CharacterIndex : AllCharacters)
	{
		APolyWarBaseCharacter* PolyWarCharacter = Cast<APolyWarBaseCharacter>(CharacterIndex);
		if(PolyWarCharacter && !PolyWarCharacter->IsDead())
		{
			if(PolyWarCharacter->GetTeamType() == ETeamType::ET_BlueTeam)
			{
				BlueTeamNum++;
			}
			else if(PolyWarCharacter->GetTeamType() == ETeamType::ET_RedTeam)
			{
				RedTeamNum++;
			}
		}
	}

	if(BlueTeamNum > RedTeamNum)
	{
		GameEnd(ETeamType::ET_BlueTeam);
	}
	else if(RedTeamNum > BlueTeamNum)
	{
		GameEnd(ETeamType::ET_RedTeam);
	}
	else if(RedTeamNum == BlueTeamNum)
	{
		GameEnd(ETeamType::ET_NoTeam);
	}
}
