// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/PolyWarHUD.h"
#include "UI/CharacterWidget.h"

void APolyWarHUD::BeginPlay()
{
	Super::BeginPlay();

	OwnerPlayerController = GetOwningPlayerController();
}

void APolyWarHUD::AddCharacterWidget()
{
	OwnerPlayerController = OwnerPlayerController == nullptr ? GetOwningPlayerController() : OwnerPlayerController;

	if(OwnerPlayerController && CharacterWidgetClass && !CharacterWidget)
	{
		CharacterWidget = CreateWidget<UCharacterWidget>(OwnerPlayerController, CharacterWidgetClass);
		CharacterWidget->AddToViewport();
	}
}
