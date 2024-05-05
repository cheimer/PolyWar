// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
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

	UPROPERTY(EditAnywhere, Category = "Set Should")
	TSubclassOf<class UCharacterWidget> CharacterWidgetClass;
	TObjectPtr<class UCharacterWidget> CharacterWidget;
	void AddCharacterWidget();

	UPROPERTY(EditAnywhere, Category = "Set Should")
	TSubclassOf<class UMapWidget> MapWidgetClass;
	TObjectPtr<class UMapWidget> MapWidget;
	void AddMapWidget();

	//~ TODO: MapMoveError Temp Solve
	UPROPERTY(EditAnywhere, Category = "Set Should")
	UMaterial* MapMaterial1;
	UPROPERTY(EditAnywhere, Category = "Set Should")
	UMaterial* MapMaterial2;
	//~ TODO END

private:
	TObjectPtr<class APlayerController> OwnerPlayerController;

};
