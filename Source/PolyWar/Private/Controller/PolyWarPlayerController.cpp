// Fill out your copyright notice in the Description page of Project Settings.


#include "Controller/PolyWarPlayerController.h"

#include "UI/PolyWarHUD.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputMappingContext.h"
#include "InputActionValue.h"
#include "Character/PolyWarAICharacter.h"
#include "Character/PolyWarBaseCharacter.h"
#include "Character/PolyWarPlayerCharacter.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "GameState/PolyWarGameStateBase.h"
#include "Kismet/GameplayStatics.h"
#include "UI/CharacterWidget.h"
#include "UI/MapButton.h"
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
		if(InputUnitNum1)
		{
			EnhancedInputComponent->BindAction(InputUnitNum1, ETriggerEvent::Triggered, this, &ThisClass::ToggleUnitNum1);
		}
		if(InputUnitNum2)
		{
			EnhancedInputComponent->BindAction(InputUnitNum2, ETriggerEvent::Triggered, this, &ThisClass::ToggleUnitNum2);
		}
		if(InputUnitNum3)
		{
			EnhancedInputComponent->BindAction(InputUnitNum3, ETriggerEvent::Triggered, this, &ThisClass::ToggleUnitNum3);
		}
		if(InputUnitNum4)
		{
			EnhancedInputComponent->BindAction(InputUnitNum4, ETriggerEvent::Triggered, this, &ThisClass::ToggleUnitNum4);
		}
		if(InputUnitNum5)
		{
			EnhancedInputComponent->BindAction(InputUnitNum5, ETriggerEvent::Triggered, this, &ThisClass::ToggleUnitNum5);
		}
		if(InputUnitNum6)
		{
			EnhancedInputComponent->BindAction(InputUnitNum6, ETriggerEvent::Triggered, this, &ThisClass::ToggleUnitNum6);
		}
		if(InputUnitNum7)
		{
			EnhancedInputComponent->BindAction(InputUnitNum7, ETriggerEvent::Triggered, this, &ThisClass::ToggleUnitNum7);
		}
		if(InputUnitNum8)
		{
			EnhancedInputComponent->BindAction(InputUnitNum8, ETriggerEvent::Triggered, this, &ThisClass::ToggleUnitNum8);
		}
		if(InputOrderAttack)
		{
			EnhancedInputComponent->BindAction(InputOrderAttack, ETriggerEvent::Triggered, this, &ThisClass::ToggleOrderAttack);
		}
		if(InputOrderMove)
		{
			EnhancedInputComponent->BindAction(InputOrderMove, ETriggerEvent::Triggered, this, &ThisClass::ToggleOrderMove);
		}
		if(InputOrderStop)
		{
			EnhancedInputComponent->BindAction(InputOrderStop, ETriggerEvent::Triggered, this, &ThisClass::ToggleOrderStop);
		}
		if(InputOrderRush)
		{
			EnhancedInputComponent->BindAction(InputOrderRush, ETriggerEvent::Triggered, this, &ThisClass::ToggleOrderRush);
		}
		if(InputOrderHold)
		{
			EnhancedInputComponent->BindAction(InputOrderHold, ETriggerEvent::Triggered, this, &ThisClass::ToggleOrderHold);
		}
		if(InputOrderCancel)
		{
			EnhancedInputComponent->BindAction(InputOrderCancel, ETriggerEvent::Triggered, this, &ThisClass::ToggleOrderCancel);
		}
	}
}

void APolyWarPlayerController::InitializeUnitMap()
{
	for(EUnitNum UnitNum : TEnumRange<EUnitNum>())
	{
		UnitMap.Emplace(UnitNum, EMapUnitState::EMUS_UnClicked);
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

		PolyWarPlayerCharacter->ResetMapCameraLocation();
		ResetMapButtons();
	}
	// Map Close
	else if(PolyWarHUD->MapWidget->GetVisibility() == ESlateVisibility::Visible)
	{
		PolyWarHUD->MapWidget->SetVisibility(ESlateVisibility::Hidden);
		SetInputMode(FInputModeGameOnly());
		SetShowMouseCursor(false);
		PolyWarPlayerCharacter->SetIsOpenMap(false);

		PolyWarPlayerCharacter->ResetMapCameraLocation();
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
			UnitMap[UnitNum] = EMapUnitState::EMUS_UnClicked;
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

void APolyWarPlayerController::GetMapUnitStateArray(EMapUnitState MapUnitState, TArray<EUnitNum>& OutUnitNumArray)
{
	for(EUnitNum UnitNum : TEnumRange<EUnitNum>())
	{
		if(UnitMap.Contains(UnitNum) && UnitMap[UnitNum] == MapUnitState)
		{
			OutUnitNumArray.Add(UnitNum);
		}
	}
}

void APolyWarPlayerController::MapUnitToggle(EUnitNum UnitNum, UTextBlock* UnitText)
{
	if(!UnitText || !UnitMap.Find(UnitNum)) return;
	if(!StoreUnitTextBlocks.Contains(UnitText))
	{
		StoreUnitTextBlocks.Emplace(UnitText);
	}

	if(UnitMap[UnitNum] == EMapUnitState::EMUS_UnClicked)
	{
		UnitMap[UnitNum] = EMapUnitState::EMUS_Clicked;
		UnitText->SetColorAndOpacity(FColor::Green);
	}
	else if(UnitMap[UnitNum] == EMapUnitState::EMUS_Clicked)
	{
		UnitMap[UnitNum] = EMapUnitState::EMUS_UnClicked;
		UnitText->SetColorAndOpacity(FColor::White);
	}
	else if(UnitMap[UnitNum] == EMapUnitState::EMUS_Removed)
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
		StartOrder(OrderType);
		break;
	case EOrderType::EOD_Cancel :
		ResetMapButtons();
		break;
	}
}

void APolyWarPlayerController::MapImageClick(const FVector2D StartPos, const FVector2D Size, const FVector2D ClickPos)
{
	if(CurrentOrder == EOrderType::EOD_MAX || !CurrentOrderText) return;

	FVector MapPosition = MapImageClickToWorldPosition(StartPos, Size, ClickPos);
	StartOrder(CurrentOrder, MapPosition);
}

FVector APolyWarPlayerController::MapImageClickToWorldPosition(const FVector2D StartPos, const FVector2D Size, const FVector2D ClickPos)
{
	PolyWarPlayerCharacter = PolyWarPlayerCharacter == nullptr ? Cast<APolyWarPlayerCharacter>(GetPawn()) : PolyWarPlayerCharacter;
	if(!PolyWarPlayerCharacter) return FVector();

	FVector2D Rate((StartPos + Size - ClickPos) / Size);
	Rate.X = 0.5 - Rate.X; Rate.Y = 0.5 - Rate.Y;

	FVector MapCameraPos = PolyWarPlayerCharacter->GetMapCameraPos();

	FVector TraceStartPos;
	TraceStartPos.X = MapCameraPos.X - Rate.Y * MapCameraPos.Z * 1.1f;
	TraceStartPos.Y = MapCameraPos.Y + Rate.X * MapCameraPos.Z * 2.0f;
	TraceStartPos.Z = MapCameraPos.Z;

	FVector TraceEndPos = TraceStartPos;
	TraceEndPos.Z = TraceStartPos.Z - PolyWarPlayerCharacter->GetMapHeightMaxLimit() * 2.0f;
	FHitResult HitResult;
	GetWorld()->LineTraceSingleByChannel(HitResult, TraceStartPos, TraceEndPos, ECC_WorldStatic);

	return HitResult.ImpactPoint;
}

void APolyWarPlayerController::StartOrder(EOrderType Order, FVector OrderPos)
{
	TArray<APolyWarAICharacter*> TeamArray = GetMyTeam();

	if(HasAuthority())
	{
		for(auto TeamAI : TeamArray)
		{
			switch (Order)
			{
			case EOrderType::EOD_Move :
			case EOrderType::EOD_Attack :
			case EOrderType::EOD_Rush :
				TeamAI->StartOrder(Order, OrderPos);
				break;
			case EOrderType::EOD_Hold :
			case EOrderType::EOD_Stop :
				TeamAI->StartOrder(Order, TeamAI->GetActorLocation());
				break;
			case EOrderType::EOD_Cancel :
				break;
			}
		}
	}
	else
	{
		ServerStartOrder(Order, OrderPos, TeamArray);
	}

	ResetMapButtons();
}

void APolyWarPlayerController::ServerStartOrder_Implementation(EOrderType Order, FVector_NetQuantize OrderPos,
	const TArray<APolyWarAICharacter*>& TeamArray)
{
	for(auto TeamAI : TeamArray)
	{
		switch (Order)
		{
		case EOrderType::EOD_Move :
		case EOrderType::EOD_Attack :
		case EOrderType::EOD_Rush :
			TeamAI->StartOrder(Order, OrderPos);
			break;
		case EOrderType::EOD_Hold :
		case EOrderType::EOD_Stop :
			TeamAI->StartOrder(Order, TeamAI->GetActorLocation());
			break;
		case EOrderType::EOD_Cancel :
			break;
		}
	}

}

TArray<APolyWarAICharacter*> APolyWarPlayerController::GetMyTeam()
{
	TArray<APolyWarAICharacter*> TeamArray;
	PolyWarPlayerCharacter = PolyWarPlayerCharacter == nullptr ? Cast<APolyWarPlayerCharacter>(GetPawn()) : PolyWarPlayerCharacter;
	PolyWarGameState = PolyWarGameState == nullptr ?
		PolyWarGameState = Cast<APolyWarGameStateBase>(UGameplayStatics::GetGameState(GetWorld())) : PolyWarGameState;
	if(!PolyWarPlayerCharacter || !PolyWarGameState) return TeamArray;

	TArray<EUnitNum> UnitNums;
	GetMapUnitStateArray(EMapUnitState::EMUS_Clicked, UnitNums);
	PolyWarGameState->GetTeamArray(PolyWarPlayerCharacter->GetTeamType(), UnitNums, TeamArray);

	return TeamArray;
}

void APolyWarPlayerController::ToggleUnitNum(EUnitNum UnitNum)
{
	PolyWarHUD = PolyWarHUD == nullptr ? Cast<APolyWarHUD>(GetHUD()) : PolyWarHUD;
	if(!PolyWarHUD) return;

	if(!PolyWarHUD->MapWidget || !PolyWarHUD->MapWidget->UnitNumMapButtons[UnitNum]) return;
	PolyWarHUD->MapWidget->UnitNumMapButtons[UnitNum]->OnUnitButtonClicked.Broadcast(UnitNum);
}

void APolyWarPlayerController::ToggleOrder(EOrderType Order)
{
	PolyWarHUD = PolyWarHUD == nullptr ? Cast<APolyWarHUD>(GetHUD()) : PolyWarHUD;
	if(!PolyWarHUD) return;

	if(!PolyWarHUD->MapWidget || !PolyWarHUD->MapWidget->OrderMapButtons[Order]) return;
	PolyWarHUD->MapWidget->OrderMapButtons[Order]->OnOrderButtonClicked.Broadcast(Order);
}

void APolyWarPlayerController::ToggleUnitNum1(const FInputActionValue& Value)
{
	ToggleUnitNum(EUnitNum::EUN_Unit1);
}

void APolyWarPlayerController::ToggleUnitNum2(const FInputActionValue& Value)
{
	ToggleUnitNum(EUnitNum::EUN_Unit2);
}

void APolyWarPlayerController::ToggleUnitNum3(const FInputActionValue& Value)
{
	ToggleUnitNum(EUnitNum::EUN_Unit3);
}

void APolyWarPlayerController::ToggleUnitNum4(const FInputActionValue& Value)
{
	ToggleUnitNum(EUnitNum::EUN_Unit4);
}

void APolyWarPlayerController::ToggleUnitNum5(const FInputActionValue& Value)
{
	ToggleUnitNum(EUnitNum::EUN_Unit5);
}

void APolyWarPlayerController::ToggleUnitNum6(const FInputActionValue& Value)
{
	ToggleUnitNum(EUnitNum::EUN_Unit6);
}

void APolyWarPlayerController::ToggleUnitNum7(const FInputActionValue& Value)
{
	ToggleUnitNum(EUnitNum::EUN_Unit7);
}

void APolyWarPlayerController::ToggleUnitNum8(const FInputActionValue& Value)
{
	ToggleUnitNum(EUnitNum::EUN_Unit8);
}

void APolyWarPlayerController::ToggleOrderAttack(const FInputActionValue& Value)
{
	ToggleOrder(EOrderType::EOD_Attack);
}

void APolyWarPlayerController::ToggleOrderStop(const FInputActionValue& Value)
{
	ToggleOrder(EOrderType::EOD_Stop);
}

void APolyWarPlayerController::ToggleOrderHold(const FInputActionValue& Value)
{
	ToggleOrder(EOrderType::EOD_Hold);
}

void APolyWarPlayerController::ToggleOrderRush(const FInputActionValue& Value)
{
	ToggleOrder(EOrderType::EOD_Rush);
}

void APolyWarPlayerController::ToggleOrderMove(const FInputActionValue& Value)
{
	ToggleOrder(EOrderType::EOD_Move);
}

void APolyWarPlayerController::ToggleOrderCancel(const FInputActionValue& Value)
{
	ToggleOrder(EOrderType::EOD_Cancel);
}
