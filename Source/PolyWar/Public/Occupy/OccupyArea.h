// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PolyWarTypes/TeamType.h"
#include "OccupyArea.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnOccupyByTeamDelegate, ETeamType, Team);

UCLASS()
class POLYWAR_API AOccupyArea : public AActor
{
	GENERATED_BODY()
	
public:
	AOccupyArea();
	virtual void Tick(float DeltaSeconds) override;

	FOnOccupyByTeamDelegate OnOccupyByTeamDelegate;

	UPROPERTY(EditAnywhere, Category = "Set Should")
	float MaxOccupyTime = 30.0f;

protected:
	virtual void BeginPlay() override;

	UFUNCTION()
	virtual void OnAreaBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	virtual void OnAreaEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

private:
	UPROPERTY(EditAnywhere)
	class USphereComponent* SphereArea;

	TArray<class APolyWarBaseCharacter*> OverlapCharacters;
	int32 BlueTeamNum = 0;
	int32 RedTeamNum = 0;

	void CalcOccupy(float DeltaSeconds);

	bool IsOccupy = false;
	ETeamType CurrentOccupyTeam = ETeamType::ET_NoTeam;
	float CurrentOccupyTime = 0.0f;

	FTimerHandle OccupyTimer;

	void OccupyTimerStart();

};
