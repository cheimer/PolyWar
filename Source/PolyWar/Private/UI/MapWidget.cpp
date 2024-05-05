// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/MapWidget.h"

#include "Components/Button.h"
#include "Controller/PolyWarPlayerController.h"

void UMapWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	PlayerController = PlayerController == nullptr ? Cast<APolyWarPlayerController>(GetOwningPlayer()) : PlayerController;

	if(ExitButton)
	{
		ExitButton->OnClicked.AddDynamic(this, &ThisClass::ExitMap);
	}
}

void UMapWidget::ExitMap()
{
	PlayerController = PlayerController == nullptr ? Cast<APolyWarPlayerController>(GetOwningPlayer()) : PlayerController;
	if(!PlayerController) return;

	PlayerController->MapToggle();
}
