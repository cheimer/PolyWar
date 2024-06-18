// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UnitInfoWidget.generated.h"

/**
 * 
 */
UCLASS()
class POLYWAR_API UUnitInfoWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, meta = (BindWidget))
	class UTextBlock* UnitNameText;

	UPROPERTY(EditDefaultsOnly, meta = (BindWidget))
	class UTextBlock* UnitNumText;
	
};
