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
	virtual void PostLogin(APlayerController* NewPlayer) override;
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;

	virtual void CharacterDeath(class APolyWarBaseCharacter* DeathCharacter);

	UPROPERTY(EditDefaultsOnly, Category = "Set Should")
	bool bUseTimeLimit = true;

	UPROPERTY(EditDefaultsOnly, Category = "Set Should", meta = (EditCondition = "bUseTimeLimit == true"))
	float TimeLimit = 600.0f;

protected:
	TObjectPtr<class APolyWarGameStateBase> PolyWarGameState;

	void GameEnd(ETeamType WinnerTeam);

private:
	float TimePassed = 0.0f;

	virtual ETeamType TimeOverWinnerTeam();

};
