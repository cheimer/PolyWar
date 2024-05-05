// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/PolyWarHUD.h"

#include "Components/Image.h"
#include "UI/CharacterWidget.h"
#include "UI/MapWidget.h"

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

void APolyWarHUD::AddMapWidget()
{
	OwnerPlayerController = OwnerPlayerController == nullptr ? GetOwningPlayerController() : OwnerPlayerController;

	if(OwnerPlayerController && MapWidgetClass && !MapWidget)
	{
		MapWidget = CreateWidget<UMapWidget>(OwnerPlayerController, MapWidgetClass);
		MapWidget->AddToViewport();
		MapWidget->SetVisibility(ESlateVisibility::Hidden);

		if(GetOwner() && MapWidget && MapWidget->MapImage)
		{
			if(GetOwner()->HasAuthority() && MapMaterial1)
			{
				MapWidget->MapImage->SetBrushFromMaterial(MapMaterial1);
			}
			else if(!GetOwner()->HasAuthority() && MapMaterial2)
			{
				MapWidget->MapImage->SetBrushFromMaterial(MapMaterial2);
			}
		}

	}
}
