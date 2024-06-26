// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/MapWidget.h"

#include "Components/Button.h"
#include "Components/Image.h"
#include "Controller/PolyWarPlayerController.h"
#include "UI/MapButton.h"

void UMapWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	PlayerController = PlayerController == nullptr ? Cast<APolyWarPlayerController>(GetOwningPlayer()) : PlayerController;

	if(ExitButton)
	{
		ExitButton->OnClicked.AddDynamic(this, &ThisClass::ExitMap);
	}
	if(MapImage)
	{
		MapImage->OnMouseButtonDownEvent.BindDynamic(this, &ThisClass::MapClick);
	}

	//~ Begin UnitButton
	if(Unit1Button)
	{
		SetUnitButton(Unit1Button, EUnitNum::EUN_Unit1, Unit1Text);
	}
	if(Unit2Button)
	{
		SetUnitButton(Unit2Button, EUnitNum::EUN_Unit2, Unit2Text);
	}
	if(Unit3Button)
	{
		SetUnitButton(Unit3Button, EUnitNum::EUN_Unit3, Unit3Text);
	}
	if(Unit4Button)
	{
		SetUnitButton(Unit4Button, EUnitNum::EUN_Unit4, Unit4Text);
	}
	if(Unit5Button)
	{
		SetUnitButton(Unit5Button, EUnitNum::EUN_Unit5, Unit5Text);
	}
	if(Unit6Button)
	{
		SetUnitButton(Unit6Button, EUnitNum::EUN_Unit6, Unit6Text);
	}
	if(Unit7Button)
	{
		SetUnitButton(Unit7Button, EUnitNum::EUN_Unit7, Unit7Text);
	}
	if(Unit8Button)
	{
		SetUnitButton(Unit8Button, EUnitNum::EUN_Unit8, Unit8Text);
	}
	//~ End UnitButton

	//~ Begin OrderButton
	if(MoveButton)
	{
		SetOrderButton(MoveButton, EOrderType::EOD_Move, MoveText);
	}
	if(AttackButton)
	{
		SetOrderButton(AttackButton, EOrderType::EOD_Attack, AttackText);
	}
	if(HoldButton)
	{
		SetOrderButton(HoldButton, EOrderType::EOD_Hold, HoldText);
	}
	if(RushButton)
	{
		SetOrderButton(RushButton, EOrderType::EOD_Rush, RushText);
	}
	if(StopButton)
	{
		SetOrderButton(StopButton, EOrderType::EOD_Stop, StopText);
	}
	if(CancelButton)
	{
		SetOrderButton(CancelButton, EOrderType::EOD_Cancel, CancelText);
	}
	//~ End OrderButton
}

void UMapWidget::SetUnitButton(UMapButton* UnitButton, EUnitNum UnitNum, UTextBlock* TextBlock)
{
	UnitButton->bIsUnitButton = true;
	UnitButton->UnitNum = UnitNum;
	UnitButton->OnUnitButtonClicked.AddDynamic(this, &ThisClass::UnitButtonClicked);

	UnitNumMapButtons.Emplace(UnitNum, UnitButton);
	UnitNumTextBlocks.Emplace(UnitNum, TextBlock);
}

void UMapWidget::SetOrderButton(UMapButton* OrderButton, EOrderType OrderType, UTextBlock* TextBlock)
{
	OrderButton->bIsOrderButton = true;
	OrderButton->OrderType = OrderType;
	OrderButton->OnOrderButtonClicked.AddDynamic(this, &ThisClass::OrderButtonClicked);

	OrderMapButtons.Emplace(OrderType, OrderButton);
	OrderTextBlocks.Emplace(OrderType, TextBlock);
}

void UMapWidget::UnitButtonClicked(EUnitNum UnitNum)
{
	PlayerController = PlayerController == nullptr ? Cast<APolyWarPlayerController>(GetOwningPlayer()) : PlayerController;
	if(!PlayerController) return;

	PlayerController->MapUnitToggle(UnitNum, UnitNumTextBlocks[UnitNum]);
}

void UMapWidget::OrderButtonClicked(EOrderType OrderType)
{
	PlayerController = PlayerController == nullptr ? Cast<APolyWarPlayerController>(GetOwningPlayer()) : PlayerController;
	if(!PlayerController) return;

	PlayerController->MapOrderToggle(OrderType, OrderTextBlocks[OrderType]);
}

void UMapWidget::ExitMap()
{
	PlayerController = PlayerController == nullptr ? Cast<APolyWarPlayerController>(GetOwningPlayer()) : PlayerController;
	if(!PlayerController) return;

	PlayerController->MapToggle();
}

FEventReply UMapWidget::MapClick(FGeometry MyGeometry, const FPointerEvent& MouseEvent)
{
	if(!MouseEvent.GetPressedButtons().Find(FName("LeftMouseButton"))) return FEventReply(false);

	PlayerController = PlayerController == nullptr ? Cast<APolyWarPlayerController>(GetOwningPlayer()) : PlayerController;
	if(!PlayerController) FEventReply(false);

	PlayerController->MapImageClick(
		MyGeometry.GetAbsolutePosition(),
		MyGeometry.GetAbsoluteSize(),
		MouseEvent.GetScreenSpacePosition());
	return FEventReply(false);
}
