// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PolyWarTypes/OrderType.h"
#include "PolyWarTypes/UnitNum.h"
#include "MapWidget.generated.h"

class UButton;
class UTextBlock;
class UImage;

/**
 * 
 */
UCLASS()
class POLYWAR_API UMapWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(meta = (BindWidget))
	UButton* ExitButton;

	UPROPERTY(meta = (BindWidget))
	UImage* MapImage;

	UPROPERTY(meta = (BindWidget))
	class UMapButton* Unit1Button;
	UPROPERTY(meta = (BindWidget))
	UTextBlock* Unit1Text;
	UPROPERTY(meta = (BindWidget))
	UMapButton* Unit2Button;
	UPROPERTY(meta = (BindWidget))
	UTextBlock* Unit2Text;
	UPROPERTY(meta = (BindWidget))
	UMapButton* Unit3Button;
	UPROPERTY(meta = (BindWidget))
	UTextBlock* Unit3Text;
	UPROPERTY(meta = (BindWidget))
	UMapButton* Unit4Button;
	UPROPERTY(meta = (BindWidget))
	UTextBlock* Unit4Text;
	UPROPERTY(meta = (BindWidget))
	UMapButton* Unit5Button;
	UPROPERTY(meta = (BindWidget))
	UTextBlock* Unit5Text;
	UPROPERTY(meta = (BindWidget))
	UMapButton* Unit6Button;
	UPROPERTY(meta = (BindWidget))
	UTextBlock* Unit6Text;
	UPROPERTY(meta = (BindWidget))
	UMapButton* Unit7Button;
	UPROPERTY(meta = (BindWidget))
	UTextBlock* Unit7Text;
	UPROPERTY(meta = (BindWidget))
	UMapButton* Unit8Button;
	UPROPERTY(meta = (BindWidget))
	UTextBlock* Unit8Text;

	UPROPERTY(meta = (BindWidget))
	UMapButton* MoveButton;
	UPROPERTY(meta = (BindWidget))
	UTextBlock* MoveText;
	UPROPERTY(meta = (BindWidget))
	UMapButton* AttackButton;
	UPROPERTY(meta = (BindWidget))
	UTextBlock* AttackText;
	UPROPERTY(meta = (BindWidget))
	UMapButton* HoldButton;
	UPROPERTY(meta = (BindWidget))
	UTextBlock* HoldText;
	UPROPERTY(meta = (BindWidget))
	UMapButton* RushButton;
	UPROPERTY(meta = (BindWidget))
	UTextBlock* RushText;
	UPROPERTY(meta = (BindWidget))
	UMapButton* StopButton;
	UPROPERTY(meta = (BindWidget))
	UTextBlock* StopText;
	UPROPERTY(meta = (BindWidget))
	UMapButton* CancelButton;
	UPROPERTY(meta = (BindWidget))
	UTextBlock* CancelText;

protected:
	virtual void NativeOnInitialized() override;

private:
	TObjectPtr<class APolyWarPlayerController> PlayerController;

	UFUNCTION()
	void ExitMap();

	UFUNCTION()
	void UnitButtonClicked(EUnitNum UnitNum, UTextBlock* TextBlock);
	void SetUnitButton(UMapButton* UnitButton, EUnitNum UnitNum, UTextBlock* TextBlock);

	UFUNCTION()
	void OrderButtonClicked(EOrderType OrderType, UTextBlock* TextBlock);
	void SetOrderButton(UMapButton* OrderButton, EOrderType OrderType, UTextBlock* TextBlock);


};
