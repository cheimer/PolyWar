// Fill out your copyright notice in the Description page of Project Settings.


#include "Controller/PolyWarPlayerController.h"

#include "UI/PolyWarHUD.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputMappingContext.h"
#include "InputActionValue.h"
#include "Character/PolyWarBaseCharacter.h"
#include "Character/PolyWarPlayerCharacter.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "UI/CharacterWidget.h"
#include "UI/MapWidget.h"


APolyWarPlayerController::APolyWarPlayerController()
{
	PrimaryActorTick.bCanEverTick = true;
}

void APolyWarPlayerController::BeginPlay()
{
	Super::BeginPlay();

	if (UEnhancedInputLocalPlayerSubsystem* Subsystem =
			ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
	{
		Subsystem->AddMappingContext(ControllerInputMapping, 0);
	}

	CreateWidgets();
	InitializeUnitMap();
}

void APolyWarPlayerController::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

}

void APolyWarPlayerController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	CreateWidgets();

}

void APolyWarPlayerController::CreateWidgets()
{
	PolyWarHUD = PolyWarHUD == nullptr ? Cast<APolyWarHUD>(GetHUD()) : PolyWarHUD;
	if(PolyWarHUD)
	{
		PolyWarHUD->AddCharacterWidget();
		PolyWarHUD->AddMapWidget();
	}
}

void APolyWarPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	if(!InputComponent) return;
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(InputComponent))
	{

	}
}

void APolyWarPlayerController::InitializeUnitMap()
{
	for(EUnitNum UnitNum : TEnumRange<EUnitNum>())
	{
		UnitMap.Emplace(UnitNum, EUnitState::EUS_UnClicked);
	}
}

void APolyWarPlayerController::SetHUDHealth(float CurrentHealth, float MaxHealth)
{
	PolyWarHUD = PolyWarHUD == nullptr ? Cast<APolyWarHUD>(GetHUD()) : PolyWarHUD;

	bool bHUDValid = PolyWarHUD &&
		PolyWarHUD->CharacterWidget &&
		PolyWarHUD->CharacterWidget->HealthBar &&
		PolyWarHUD->CharacterWidget->HealthText;

	if(bHUDValid)
	{
		const float HealthPercent = CurrentHealth / MaxHealth;
		PolyWarHUD->CharacterWidget->HealthBar->SetPercent(HealthPercent);
		const FString HealthText = FString::Printf(TEXT("%d / %d"), FMath::CeilToInt(CurrentHealth), FMath::CeilToInt(MaxHealth));
		PolyWarHUD->CharacterWidget->HealthText->SetText(FText::FromString(HealthText));
	}
}

void APolyWarPlayerController::MapToggle()
{
	PolyWarHUD = PolyWarHUD == nullptr ? Cast<APolyWarHUD>(GetHUD()) : PolyWarHUD;
	PolyWarPlayerCharacter = PolyWarPlayerCharacter == nullptr ? Cast<APolyWarPlayerCharacter>(GetPawn()) : PolyWarPlayerCharacter;
	if(!PolyWarHUD || !PolyWarHUD->MapWidget || !PolyWarPlayerCharacter) return;

	// Map Open
	if(PolyWarHUD->MapWidget->GetVisibility() == ESlateVisibility::Hidden)
	{
		PolyWarHUD->MapWidget->SetVisibility(ESlateVisibility::Visible);
		SetInputMode(FInputModeGameAndUI());
		SetShowMouseCursor(true);
		PolyWarPlayerCharacter->SetIsOpenMap(true);

		PolyWarPlayerCharacter->ResetMapSpringArmLocation();
		ResetMapButtons();
	}
	// Map Close
	else if(PolyWarHUD->MapWidget->GetVisibility() == ESlateVisibility::Visible)
	{
		PolyWarHUD->MapWidget->SetVisibility(ESlateVisibility::Hidden);
		SetInputMode(FInputModeGameOnly());
		SetShowMouseCursor(false);
		PolyWarPlayerCharacter->SetIsOpenMap(false);

		PolyWarPlayerCharacter->ResetMapSpringArmLocation();
		ResetMapButtons();
	}
}

void APolyWarPlayerController::ResetMapButtons()
{
	// UnitMap Reset
	for(EUnitNum UnitNum : TEnumRange<EUnitNum>())
	{
		if(UnitMap.Find(UnitNum))
		{
			UnitMap[UnitNum] = EUnitState::EUS_UnClicked;
		}
	}
	// UnitTextBlock Reset
	for(UTextBlock* TextBlock : StoreUnitTextBlocks)
	{
		TextBlock->SetColorAndOpacity(FColor::White);
	}
	StoreUnitTextBlocks.Empty();

	// Order Reset
	CurrentOrder = EOrderType::EOD_MAX;
	if(CurrentOrderText != nullptr)
	{
		CurrentOrderText->SetColorAndOpacity(FColor::White);
		CurrentOrderText = nullptr;
	}
}

void APolyWarPlayerController::MapUnitToggle(EUnitNum UnitNum, UTextBlock* UnitText)
{
	if(!UnitText || !UnitMap.Find(UnitNum)) return;
	if(!StoreUnitTextBlocks.Contains(UnitText))
	{
		StoreUnitTextBlocks.Emplace(UnitText);
	}

	if(UnitMap[UnitNum] == EUnitState::EUS_UnClicked)
	{
		UnitMap[UnitNum] = EUnitState::EUS_Clicked;
		UnitText->SetColorAndOpacity(FColor::Green);
	}
	else if(UnitMap[UnitNum] == EUnitState::EUS_Clicked)
	{
		UnitMap[UnitNum] = EUnitState::EUS_UnClicked;
		UnitText->SetColorAndOpacity(FColor::White);
	}
	else if(UnitMap[UnitNum] == EUnitState::EUS_Removed)
	{
		;
	}
}

void APolyWarPlayerController::MapOrderToggle(EOrderType OrderType, UTextBlock* OrderText)
{
	if(!OrderText) return;
	if(OrderType == CurrentOrder)
	{
		CurrentOrder = EOrderType::EOD_MAX;
		OrderText->SetColorAndOpacity(FColor::White);
		CurrentOrderText = OrderText;
		return;
	}

	// Before TextBlock Color Reset
	if(OrderType != CurrentOrder && CurrentOrder != EOrderType::EOD_MAX)
	{
		if(CurrentOrderText)
		{
			CurrentOrderText->SetColorAndOpacity(FColor::White);
		}
	}

	switch(OrderType)
	{
	// Apply When Click Map
	case EOrderType::EOD_Move :
	case EOrderType::EOD_Attack :
	case EOrderType::EOD_Rush :
		CurrentOrder = OrderType;
		CurrentOrderText = OrderText;
		CurrentOrderText->SetColorAndOpacity(FColor::Green);
		break;

	// Apply Immediately
	case EOrderType::EOD_Hold :
	case EOrderType::EOD_Stop :
	case EOrderType::EOD_Cancel :
		StartOrder(OrderType);
		break;
	}
}

void APolyWarPlayerController::StartOrder(EOrderType Order)
{
	switch (Order)
	{
	case EOrderType::EOD_Move :
		break;
	case EOrderType::EOD_Attack :
		break;
	case EOrderType::EOD_Rush :
		break;
	case EOrderType::EOD_Hold :
		break;
	case EOrderType::EOD_Stop :
		break;
	case EOrderType::EOD_Cancel :
		break;
	}

	ResetMapButtons();
}

void APolyWarPlayerController::MapImageClick(const FVector2D StartPos, const FVector2D ClickPos)
{
	if(CurrentOrder == EOrderType::EOD_MAX || !CurrentOrderText) return;

	EOrderType TempOrderStore = CurrentOrder;
	CurrentOrder = EOrderType::EOD_MAX;
	CurrentOrderText->SetColorAndOpacity(FColor::White);
	CurrentOrderText = nullptr;
	StartOrder(TempOrderStore);
}
