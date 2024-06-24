// Fill out your copyright notice in the Description page of Project Settings.


#include "GameMode/PolyWarOccupyGameMode.h"

#include "Kismet/GameplayStatics.h"
#include "Occupy/OccupyArea.h"

void APolyWarOccupyGameMode::BeginPlay()
{
	Super::BeginPlay();

	TArray<AActor*> AllAreas;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AOccupyArea::StaticClass(), AllAreas);

	for(auto Area : AllAreas)
	{
		AOccupyArea* OccupyArea = Cast<AOccupyArea>(Area);
		if(OccupyArea)
		{
			OccupyArea->OnOccupyByTeamDelegate.AddDynamic(this, &ThisClass::AddOccupyTime);
		}
	}
}

void APolyWarOccupyGameMode::AddOccupyTime(ETeamType Team)
{
	if(Team == ETeamType::ET_BlueTeam)
	{
		BlueTeamOccupyTime++;
	}
	else if(Team == ETeamType::ET_RedTeam)
	{
		RedTeamOccupyTime++;
	}

	if(BlueTeamOccupyTime >= MaxOccupyTime)
	{
		GameEnd(ETeamType::ET_BlueTeam);
	}
	else if(RedTeamOccupyTime >= MaxOccupyTime)
	{
		GameEnd(ETeamType::ET_RedTeam);
	}
}
