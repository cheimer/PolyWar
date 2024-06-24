// Fill out your copyright notice in the Description page of Project Settings.


#include "Occupy/OccupyArea.h"

#include "Character/PolyWarBaseCharacter.h"
#include "Components/SphereComponent.h"

AOccupyArea::AOccupyArea()
{
	PrimaryActorTick.bCanEverTick = true;

	SphereArea = CreateDefaultSubobject<USphereComponent>("SphereArea");
	SetRootComponent(SphereArea);
}

void AOccupyArea::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if(CurrentOccupyTeam == ETeamType::ET_NoTeam)
	{
		if(BlueTeamNum > 0)
		{
			CurrentOccupyTeam = ETeamType::ET_BlueTeam;
		}
		else if(RedTeamNum > 0)
		{
			CurrentOccupyTeam = ETeamType::ET_RedTeam;
		}
		else
		{
			return;
		}
	}

	CalcOccupy(DeltaSeconds);

	if(IsOccupy)
	{
		if(!GetWorldTimerManager().IsTimerActive(OccupyTimer))
		{
			GetWorldTimerManager().ClearTimer(OccupyTimer);
			GetWorldTimerManager().SetTimer(OccupyTimer, this, &ThisClass::OccupyTimerStart, 1.0f, true);
		}
	}
	else
	{
		if(GetWorldTimerManager().IsTimerActive(OccupyTimer))
		{
			GetWorldTimerManager().PauseTimer(OccupyTimer);
		}
	}

}

void AOccupyArea::CalcOccupy(float DeltaSeconds)
{
	ETeamType EnemyTeam = ETeamType::ET_NoTeam;
	int32 OccupyNum = -1, EnemyNum = -1;
	if(CurrentOccupyTeam == ETeamType::ET_BlueTeam)
	{
		OccupyNum = BlueTeamNum;
		EnemyTeam = ETeamType::ET_RedTeam;
		EnemyNum = RedTeamNum;
	}
	else if(CurrentOccupyTeam == ETeamType::ET_RedTeam)
	{
		OccupyNum = RedTeamNum;
		EnemyTeam = ETeamType::ET_BlueTeam;
		EnemyNum = BlueTeamNum;
	}

	if(IsOccupy)
	{
		// case: OnOccupy, Enemy is null
		if(EnemyNum == 0)
		{
			CurrentOccupyTime = FMath::Clamp(CurrentOccupyTime + DeltaSeconds, 0.0f, MaxOccupyTime);
			if(FMath::IsNearlyEqual(CurrentOccupyTime, MaxOccupyTime))
			{
				IsOccupy = true;
			}
		}
		// case: OnOccupy, Enemy take it
		if(OccupyNum == 0 && EnemyNum > 0)
		{
			CurrentOccupyTime = FMath::Clamp(CurrentOccupyTime - DeltaSeconds, 0.0f, MaxOccupyTime);
			if(FMath::IsNearlyZero(CurrentOccupyTime))
			{
				IsOccupy = false;
				CurrentOccupyTeam = EnemyTeam;
				CurrentOccupyTime = 0.0f;
			}
		}
	}
	else
	{
		// case: not Occupy, OccupyTeam take it, Enemy is null
		if(OccupyNum > 0 && EnemyNum == 0)
		{
			CurrentOccupyTime = FMath::Clamp(CurrentOccupyTime + DeltaSeconds, 0.0f, MaxOccupyTime);
			if(FMath::IsNearlyEqual(CurrentOccupyTime, MaxOccupyTime))
			{
				IsOccupy = true;
			}
		}
		// case: not Occupy, Enemy take it
		if(OccupyNum == 0 && EnemyNum > 0)
		{
			CurrentOccupyTime = FMath::Clamp(CurrentOccupyTime - DeltaSeconds, 0.0f, MaxOccupyTime);
			if(FMath::IsNearlyZero(CurrentOccupyTime))
			{
				IsOccupy = false;
				CurrentOccupyTeam = EnemyTeam;
				CurrentOccupyTime = 0.0f;
			}
		}
		// case: not Occupy, OccupyTeam is null, Enemy is null
		if(OccupyNum == 0 && EnemyNum == 0)
		{
			CurrentOccupyTime = FMath::Clamp(CurrentOccupyTime - DeltaSeconds, 0.0f, MaxOccupyTime);
			if(FMath::IsNearlyZero(CurrentOccupyTime))
			{
				IsOccupy = false;
				CurrentOccupyTeam = ETeamType::ET_NoTeam;
				CurrentOccupyTime = 0.0f;
			}
		}
	}
}

void AOccupyArea::BeginPlay()
{
	Super::BeginPlay();

	SphereArea->OnComponentBeginOverlap.AddDynamic(this, &ThisClass::OnAreaBeginOverlap);
	SphereArea->OnComponentEndOverlap.AddDynamic(this, &ThisClass::OnAreaEndOverlap);
	
}

void AOccupyArea::OccupyTimerStart()
{
	OnOccupyByTeamDelegate.Broadcast(CurrentOccupyTeam);
}

void AOccupyArea::OnAreaBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	APolyWarBaseCharacter* OverlapCharacter = Cast<APolyWarBaseCharacter>(OtherActor);
	if(!OverlapCharacter) return;

	if(!OverlapCharacters.Contains(OverlapCharacter))
	{
		OverlapCharacters.Emplace(OverlapCharacter);
		if(OverlapCharacter->GetTeamType() == ETeamType::ET_BlueTeam)
		{
			BlueTeamNum++;
		}
		else if(OverlapCharacter->GetTeamType() == ETeamType::ET_RedTeam)
		{
			RedTeamNum++;
		}
	}

}

void AOccupyArea::OnAreaEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	APolyWarBaseCharacter* OverlapCharacter = Cast<APolyWarBaseCharacter>(OtherActor);
	if(!OverlapCharacter) return;

	if(OverlapCharacters.Contains(OverlapCharacter))
	{
		if(OverlapCharacter->GetTeamType() == ETeamType::ET_BlueTeam)
		{
			BlueTeamNum--;
		}
		else if(OverlapCharacter->GetTeamType() == ETeamType::ET_RedTeam)
		{
			RedTeamNum--;
		}
		OverlapCharacters.Remove(OverlapCharacter);

	}
}
