// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InputActionValue.h"
#include "GameFramework/PlayerController.h"
#include "PolyWarTypes/OrderType.h"
#include "PolyWarTypes/UnitNum.h"
#include "PolyWarTypes/MapUnitState.h"
#include "PolyWarTypes/TeamType.h"
#include "PolyWarTypes/UnitType.h"
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
	void SetHUDDeathCharacter(class APolyWarBaseCharacter* DeathCharacter);

	void MapToggle();
	void MapUnitToggle(EUnitNum UnitNum, class UTextBlock* UnitText);
	void MapOrderToggle(EOrderType OrderType, class UTextBlock* OrderText);
	void MapImageClick(const FVector2D StartPos, const FVector2D Size, const FVector2D ClickPos);

	void GameEnd(ETeamType WinnerTeam);

protected:
	virtual void BeginPlay() override;
	virtual void SetupInputComponent() override;

	//~ Begin Enhanced Input
	UPROPERTY(EditDefaultsOnly, Category = "EnhancedInput")
	class UInputMappingContext* ControllerInputMapping;

	UPROPERTY(EditDefaultsOnly, Category = "EnhancedInput")
	class UInputAction* InputUnitNum1;
	UPROPERTY(EditDefaultsOnly, Category = "EnhancedInput")
	class UInputAction* InputUnitNum2;
	UPROPERTY(EditDefaultsOnly, Category = "EnhancedInput")
	class UInputAction* InputUnitNum3;
	UPROPERTY(EditDefaultsOnly, Category = "EnhancedInput")
	class UInputAction* InputUnitNum4;
	UPROPERTY(EditDefaultsOnly, Category = "EnhancedInput")
	class UInputAction* InputUnitNum5;
	UPROPERTY(EditDefaultsOnly, Category = "EnhancedInput")
	class UInputAction* InputUnitNum6;
	UPROPERTY(EditDefaultsOnly, Category = "EnhancedInput")
	class UInputAction* InputUnitNum7;
	UPROPERTY(EditDefaultsOnly, Category = "EnhancedInput")
	class UInputAction* InputUnitNum8;

	UPROPERTY(EditDefaultsOnly, Category = "EnhancedInput")
	class UInputAction* InputOrderAttack;
	UPROPERTY(EditDefaultsOnly, Category = "EnhancedInput")
	class UInputAction* InputOrderStop;
	UPROPERTY(EditDefaultsOnly, Category = "EnhancedInput")
	class UInputAction* InputOrderHold;
	UPROPERTY(EditDefaultsOnly, Category = "EnhancedInput")
	class UInputAction* InputOrderRush;
	UPROPERTY(EditDefaultsOnly, Category = "EnhancedInput")
	class UInputAction* InputOrderMove;
	UPROPERTY(EditDefaultsOnly, Category = "EnhancedInput")
	class UInputAction* InputOrderCancel;

	void ToggleUnitNum1(const FInputActionValue& Value);
	void ToggleUnitNum2(const FInputActionValue& Value);
	void ToggleUnitNum3(const FInputActionValue& Value);
	void ToggleUnitNum4(const FInputActionValue& Value);
	void ToggleUnitNum5(const FInputActionValue& Value);
	void ToggleUnitNum6(const FInputActionValue& Value);
	void ToggleUnitNum7(const FInputActionValue& Value);
	void ToggleUnitNum8(const FInputActionValue& Value);
	void ToggleUnitNum(EUnitNum UnitNum);

	void ToggleOrderAttack(const FInputActionValue& Value);
	void ToggleOrderStop(const FInputActionValue& Value);
	void ToggleOrderHold(const FInputActionValue& Value);
	void ToggleOrderRush(const FInputActionValue& Value);
	void ToggleOrderMove(const FInputActionValue& Value);
	void ToggleOrderCancel(const FInputActionValue& Value);
	void ToggleOrder(EOrderType Order);
	//~ End Enhanced Input

private:
	TObjectPtr<class APolyWarPlayerCharacter> PolyWarPlayerCharacter;
	TObjectPtr<class APolyWarHUD> PolyWarHUD;
	TObjectPtr<class APolyWarGameStateBase> PolyWarGameState;

	void UpdateHUD();
	void UpdateHUDVersusBar();
	void UpdateHUDTeamScroll(APolyWarBaseCharacter* DeathCharacter);

	void CreateWidgets();

	UFUNCTION(Client, Reliable)
	void ClientGameEnd(ETeamType WinnerTeam);

	void InitializeUnitMap();
	void ResetMapButtons();

	TMap<EUnitNum, EMapUnitState> UnitMap;
	TArray<UTextBlock*> StoreUnitTextBlocks;
	EOrderType CurrentOrder = EOrderType::EOD_MAX;
	UTextBlock* CurrentOrderText = nullptr;

	void GetMapUnitStateArray(EMapUnitState MapUnitState, TArray<EUnitNum>& OutUnitNumArray);
	FVector MapImageClickToWorldPosition(const FVector2D StartPos, const FVector2D Size, const FVector2D ClickPos);

	void GetMyTeam(TArray<class APolyWarAICharacter*>& OutTeamArray);

	void StartOrder(EOrderType Order, FVector OrderPos = FVector::ZeroVector);

	UFUNCTION(Server, Reliable)
	void ServerStartOrder(EOrderType Order, FVector_NetQuantize OrderPos, const TArray<APolyWarAICharacter*>& TeamArray);

	void SetHUDWinText(ETeamType WinTeam);
	void SetHUDVersusBar();
	void SetHUDTeamScroll();

	void TeamUnitTypes(ETeamType TeamType, TMap<EUnitType, int32>& OutTeamUnitTypes);

	float BlueTeamNum = 1;
	float RedTeamNum = 1;

public:
	EOrderType GetCurrentOrder() const {return CurrentOrder;}

};
