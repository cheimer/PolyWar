// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "EndMenuWidget.generated.h"

/**
 * 
 */
UCLASS()
class POLYWAR_API UEndMenuWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* WinText;

	UPROPERTY(meta = (BindWidget))
	class UProgressBar* VersusBar;

	UPROPERTY(meta = (BindWidget))
	class UScrollBox* BlueTeamScroll;

	UPROPERTY(meta = (BindWidget))
	class UScrollBox* RedTeamScroll;

	UPROPERTY(EditDefaultsOnly, Category = "Settable")
	FSlateColor BlueTeamColor;

	UPROPERTY(EditDefaultsOnly, Category = "Settable")
	FSlateColor RedTeamColor;

	UPROPERTY(EditDefaultsOnly, Category = "Settable")
	TSubclassOf<class UUnitInfoWidget> UnitInfoWidgetClass;

};
