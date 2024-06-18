// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "LobbyGameModeBase.generated.h"

/**
 * 
 */
UCLASS()
class POLYWAR_API ALobbyGameModeBase : public AGameModeBase
{
	GENERATED_BODY()

public:
	virtual void PostLogin(APlayerController* NewPlayer) override;

private:
	UPROPERTY(EditDefaultsOnly, Category = "Set Should")
	TArray<FString> Maps;

};
