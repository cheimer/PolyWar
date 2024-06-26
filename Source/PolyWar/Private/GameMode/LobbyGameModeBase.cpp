// Fill out your copyright notice in the Description page of Project Settings.


#include "GameMode/LobbyGameModeBase.h"

#include "MultiplayerSessionsSubsystem.h"
#include "GameFramework/GameStateBase.h"

void ALobbyGameModeBase::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);

	int32 NumberOfPlayers = GameState.Get()->PlayerArray.Num();

	UGameInstance* GameInstance = GetGameInstance();
	if (GameInstance)
	{
		UMultiplayerSessionsSubsystem* Subsystem = GameInstance->GetSubsystem<UMultiplayerSessionsSubsystem>();
		check(Subsystem);

		if (NumberOfPlayers == Subsystem->DesiredNumPublicConnections)
		{
			UWorld* World = GetWorld();
			if (World)
			{
				bUseSeamlessTravel = true;

				FString MatchType = Subsystem->DesiredMatchType;

				if(Maps.Contains(MatchType))
				{
					const FString MapPath = FString::Printf(TEXT("/Game/Maps/%s?listen"), *MatchType);
					World->ServerTravel(MapPath);
				}
			}
		}
	}
}
