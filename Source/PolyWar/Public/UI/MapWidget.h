// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
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
	UButton* Unit1Button;
	UPROPERTY(meta = (BindWidget))
	UTextBlock* Unit1Text;
	UPROPERTY(meta = (BindWidget))
	UButton* Unit2Button;
	UPROPERTY(meta = (BindWidget))
	UTextBlock* Unit2Text;
	UPROPERTY(meta = (BindWidget))
	UButton* Unit3Button;
	UPROPERTY(meta = (BindWidget))
	UTextBlock* Unit3Text;
	UPROPERTY(meta = (BindWidget))
	UButton* Unit4Button;
	UPROPERTY(meta = (BindWidget))
	UTextBlock* Unit4Text;
	UPROPERTY(meta = (BindWidget))
	UButton* Unit5Button;
	UPROPERTY(meta = (BindWidget))
	UTextBlock* Unit5Text;
	UPROPERTY(meta = (BindWidget))
	UButton* Unit6Button;
	UPROPERTY(meta = (BindWidget))
	UTextBlock* Unit6Text;
	UPROPERTY(meta = (BindWidget))
	UButton* Unit7Button;
	UPROPERTY(meta = (BindWidget))
	UTextBlock* Unit7Text;
	UPROPERTY(meta = (BindWidget))
	UButton* Unit8Button;
	UPROPERTY(meta = (BindWidget))
	UTextBlock* Unit8Text;

	UPROPERTY(meta = (BindWidget))
	UButton* MoveButton;
	UPROPERTY(meta = (BindWidget))
	UTextBlock* MoveText;
	UPROPERTY(meta = (BindWidget))
	UButton* AttackButton;
	UPROPERTY(meta = (BindWidget))
	UTextBlock* AttackText;
	UPROPERTY(meta = (BindWidget))
	UButton* HoldButton;
	UPROPERTY(meta = (BindWidget))
	UTextBlock* HoldText;
	UPROPERTY(meta = (BindWidget))
	UButton* RushButton;
	UPROPERTY(meta = (BindWidget))
	UTextBlock* RushText;
	UPROPERTY(meta = (BindWidget))
	UButton* StopButton;
	UPROPERTY(meta = (BindWidget))
	UTextBlock* StopText;
	UPROPERTY(meta = (BindWidget))
	UButton* CancelButton;
	UPROPERTY(meta = (BindWidget))
	UTextBlock* CancelText;

protected:
	virtual void NativeOnInitialized() override;

private:
	TObjectPtr<class APolyWarPlayerController> PlayerController;

	UFUNCTION()
	void ExitMap();

};
