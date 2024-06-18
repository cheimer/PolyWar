// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/PolyWarHUD.h"

#include "Components/Image.h"
#include "Components/ScrollBox.h"
#include "Components/TextBlock.h"
#include "UI/CharacterWidget.h"
#include "UI/EndMenuWidget.h"
#include "UI/MapWidget.h"
#include "UI/UnitInfoWidget.h"

void APolyWarHUD::BeginPlay()
{
	Super::BeginPlay();

	OwnerPlayerController = GetOwningPlayerController();
}

void APolyWarHUD::CreateWidgets()
{
	AddCharacterWidget();
	AddMapWidget();
	AddEndMenuWidget();
}

void APolyWarHUD::ChangeWidget(UUserWidget* NewWidget)
{
	if(!NewWidget) return;

	if(NewWidget->IsA(UCharacterWidget::StaticClass()))
	{
		ChangeCurrentWidget(CharacterWidget);
	}
	else if(NewWidget->IsA(UEndMenuWidget::StaticClass()))
	{
		ChangeCurrentWidget(EndMenuWidget);
	}
}

void APolyWarHUD::ChangeCurrentWidget(UUserWidget* ShowingWidget)
{
	CurrentWidget->SetVisibility(ESlateVisibility::Hidden);
	CurrentWidget = ShowingWidget;
	CurrentWidget->SetVisibility(ESlateVisibility::Visible);
}

void APolyWarHUD::EndMenuScrollAdd(const ETeamType TeamType, const FText& UnitName, const FText& UnitNum)
{
	OwnerPlayerController = OwnerPlayerController == nullptr ? GetOwningPlayerController() : OwnerPlayerController;

	if(OwnerPlayerController && EndMenuWidget && EndMenuWidget->UnitInfoWidgetClass && EndMenuWidget->BlueTeamScroll && EndMenuWidget->RedTeamScroll)
	{
		UUnitInfoWidget* UnitInfoWidget = CreateWidget<UUnitInfoWidget>(OwnerPlayerController, EndMenuWidget->UnitInfoWidgetClass);
		UnitInfoWidget->UnitNameText->SetText(UnitName);
		UnitInfoWidget->UnitNumText->SetText(UnitNum);

		if(TeamType == ETeamType::ET_BlueTeam)
		{
			EndMenuWidget->BlueTeamScroll->AddChild(UnitInfoWidget);
		}
		else if(TeamType == ETeamType::ET_RedTeam)
		{
			EndMenuWidget->RedTeamScroll->AddChild(UnitInfoWidget);
		}
	}
}

void APolyWarHUD::ClearEndMenuScroll()
{
	if(EndMenuWidget && EndMenuWidget->BlueTeamScroll && EndMenuWidget->RedTeamScroll)
	{
		EndMenuWidget->BlueTeamScroll->ClearChildren();
		EndMenuWidget->RedTeamScroll->ClearChildren();
	}
}

void APolyWarHUD::AddCharacterWidget()
{
	OwnerPlayerController = OwnerPlayerController == nullptr ? GetOwningPlayerController() : OwnerPlayerController;

	if(OwnerPlayerController && CharacterWidgetClass && !CharacterWidget)
	{
		CharacterWidget = CreateWidget<UCharacterWidget>(OwnerPlayerController, CharacterWidgetClass);
		CharacterWidget->AddToViewport();
		CurrentWidget = CharacterWidget;
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

void APolyWarHUD::AddEndMenuWidget()
{
	OwnerPlayerController = OwnerPlayerController == nullptr ? GetOwningPlayerController() : OwnerPlayerController;

	if(OwnerPlayerController && EndMenuWidgetClass && !EndMenuWidget)
	{
		EndMenuWidget = CreateWidget<UEndMenuWidget>(OwnerPlayerController, EndMenuWidgetClass);
		EndMenuWidget->AddToViewport();
		EndMenuWidget->SetVisibility(ESlateVisibility::Hidden);
	}
}
