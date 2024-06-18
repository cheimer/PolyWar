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

void APolyWarGameModeBase::BeginPlay()
{
	Super::BeginPlay();

}

void APolyWarGameModeBase::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	TimePassed += DeltaSeconds;

	if(bUseTimeLimit && TimePassed > TimeLimit)
	{
		GameEnd(TimeOverWinnerTeam());
	}

}

ETeamType APolyWarGameModeBase::TimeOverWinnerTeam()
{
	TArray<AActor*> AllCharacters;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), APolyWarBaseCharacter::StaticClass(), AllCharacters);

	int32 BlueTeamNum = 0, RedTeamNum = 0;

	for(AActor* CharacterIndex : AllCharacters)
	{
		APolyWarBaseCharacter* PolyWarCharacter = Cast<APolyWarBaseCharacter>(CharacterIndex);
		if(PolyWarCharacter)
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
		return ETeamType::ET_BlueTeam;
	}
	else if(RedTeamNum > BlueTeamNum)
	{
		return ETeamType::ET_RedTeam;
	}
	else if(RedTeamNum == BlueTeamNum)
	{
		return ETeamType::ET_NoTeam;
	}

	return ETeamType::ET_NoTeam;
}

void APolyWarGameModeBase::RegisterPlayer(APolyWarPlayerCharacter* PlayerCharacter)
{
	if(!PlayerCharacter) return;
	PlayerCharacter->OnCharacterDeathDelegate.AddDynamic(this, &ThisClass::PlayerDeath);

}

void APolyWarGameModeBase::PlayerDeath(APolyWarBaseCharacter* DeathCharacter)
{

}

void APolyWarGameModeBase::GameEnd(ETeamType WinnerTeam)
{
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
