// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "PolyWarTypes/OrderType.h"
#include "PolyWarTypes/UnitNum.h"
#include "PolyWarTypes/UnitState.h"
#include "PolyWarPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class POLYWAR_API APolyWarPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	APolyWarPlayerController();
	virtual void OnPossess(APawn* InPawn) override;
	virtual void Tick(float DeltaSeconds) override;

	void SetHUDHealth(float CurrentHealth, float MaxHealth);
	void MapToggle();
	void MapUnitToggle(EUnitNum UnitNum, class UTextBlock* UnitText);
	void MapOrderToggle(EOrderType OrderType, class UTextBlock* OrderText);
	void MapImageClick(const FVector2D StartPos, const FVector2D ClickPos);

protected:
	virtual void BeginPlay() override;
	virtual void SetupInputComponent() override;

	//~ Begin Enhanced Input
	UPROPERTY(EditDefaultsOnly, Category = "EnhancedInput")
	class UInputMappingContext* ControllerInputMapping;

	UPROPERTY(EditDefaultsOnly, Category = "EnhancedInput")
	class UInputAction* QuitAction;
	//~ End Enhanced Input

private:
	TObjectPtr<class APolyWarPlayerCharacter> PolyWarPlayerCharacter;
	TObjectPtr<class APolyWarHUD> PolyWarHUD;
	TObjectPtr<class UCharacterWidget> CharacterWidget;

	void CreateWidgets();
	void InitializeUnitMap();
	void ResetMapButtons();

	TMap<EUnitNum, EUnitState> UnitMap;
	EOrderType CurrentOrder = EOrderType::EOD_MAX;
	UTextBlock* CurrentOrderText = nullptr;
	TArray<UTextBlock*> StoreUnitTextBlocks;

	void StartOrder(EOrderType Order);

public:
	EOrderType GetCurrentOrder() const {return CurrentOrder;}

};
