// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CharacterWidget.generated.h"

/**
 * 
 */
UCLASS()
class POLYWAR_API UCharacterWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(meta = (BindWidget))
	class UProgressBar* HealthBar;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* HealthText;

	UPROPERTY(meta = (BindWidget))
	class UProgressBar* VersusBar;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* TimeText;

	UPROPERTY(meta = (BindWidget))
	class UProgressBar* WeaponSkillFirstBar;

	UPROPERTY(meta = (BindWidget))
	class UProgressBar* WeaponSkillSecondBar;

	UPROPERTY(meta = (BindWidget))
	class UProgressBar* SpellFirstBar;

	UPROPERTY(meta = (BindWidget))
	class UProgressBar* SpellSecondBar;

	UPROPERTY(meta = (BindWidget))
	class UProgressBar* SpellUltBar;

};
