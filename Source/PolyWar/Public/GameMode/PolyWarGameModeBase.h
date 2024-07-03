// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "PolyWarTypes/TeamType.h"
#include "PolyWarGameModeBase.generated.h"

/**
 * 
 */
UCLASS()
class POLYWAR_API APolyWarGameModeBase : public AGameModeBase
{
	GENERATED_BODY()

public:
	APolyWarGameModeBase();

	virtual void CharacterDeath(class APolyWarBaseCharacter* DeathCharacter) {}

	void GameEnd(ETeamType WinnerTeam);

	virtual void TimeEnd();

	UPROPERTY(EditDefaultsOnly, Category = "Set Should")
	bool bUseTimeLimit = true;

	UPROPERTY(EditDefaultsOnly, Category = "Set Should", meta = (EditCondition = "bUseTimeLimit == true"))
	float TimeLimit = 600.0f;

protected:
	TObjectPtr<class APolyWarGameStateBase> PolyWarGameState;

	virtual FString InitNewPlayer(APlayerController* NewPlayerController,
		const FUniqueNetIdRepl& UniqueId, const FString& Options, const FString& Portal) override;

private:
	bool bIsGameEnd = false;

};
