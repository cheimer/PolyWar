// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "PolyWarTypes/TeamType.h"
#include "PolyWarHUD.generated.h"

/**
 * 
 */
UCLASS()
class POLYWAR_API APolyWarHUD : public AHUD
{
	GENERATED_BODY()

public:
	virtual void BeginPlay() override;

	void CreateWidgets();
	void ChangeWidget(UUserWidget* NewWidget);

	TObjectPtr<class UCharacterWidget> CharacterWidget;
	TObjectPtr<class UMapWidget> MapWidget;
	TObjectPtr<class UEndMenuWidget> EndMenuWidget;

	void EndMenuScrollAdd(const ETeamType TeamType, const FText& UnitName, const FText& UnitNum);
	void ClearEndMenuScroll();

protected:
	UPROPERTY(EditAnywhere, Category = "Set Should")
	TSubclassOf<UCharacterWidget> CharacterWidgetClass;
	void AddCharacterWidget();

	UPROPERTY(EditAnywhere, Category = "Set Should")
	TSubclassOf<UMapWidget> MapWidgetClass;
	void AddMapWidget();

	UPROPERTY(EditAnywhere, Category = "Set Should")
	TSubclassOf<UEndMenuWidget> EndMenuWidgetClass;
	void AddEndMenuWidget();

	//~ TODO: MapMoveError Temp Solve
	UPROPERTY(EditAnywhere, Category = "Set Should")
	UMaterial* MapMaterial1;
	UPROPERTY(EditAnywhere, Category = "Set Should")
	UMaterial* MapMaterial2;
	//~ TODO END

private:
	TObjectPtr<class APlayerController> OwnerPlayerController;
	TObjectPtr<UUserWidget> CurrentWidget;

	void ChangeCurrentWidget(UUserWidget* ShowingWidget);

};
