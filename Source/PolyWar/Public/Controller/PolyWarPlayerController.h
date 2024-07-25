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
#include "PolyWarTypes/WeaponSkill.h"
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

	void SetInputDefault(bool IsUI);

	void SetHUDHealth(float CurrentHealth, float MaxHealth);
	void SetHUDDeathCharacter(class APolyWarBaseCharacter* DeathCharacter);

	void MapToggle();
	void MapUnitToggle(EUnitNum UnitNum, class UTextBlock* UnitText);
	void MapOrderToggle(EOrderType OrderType, class UTextBlock* OrderText);
	void MapImageClick(const FVector2D StartPos, const FVector2D Size, const FVector2D ClickPos);

	void TimeEndGame();
	void SurrenderGame();
	UFUNCTION(Server, Reliable)
	void ServerSurrenderGame();

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

	UPROPERTY(EditDefaultsOnly, Category = "EnhancedInput")
	class UInputAction* InputMenu;

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

	void ToggleMenu(const FInputActionValue& Value);
	//~ End Enhanced Input

private:
	TObjectPtr<class APolyWarPlayerCharacter> PolyWarPlayerCharacter;
	TObjectPtr<class APolyWarHUD> PolyWarHUD;
	TObjectPtr<class APolyWarGameStateBase> PolyWarGameState;

	void SetAllCharacters();

	UFUNCTION(Server, Reliable)
	void ServerSetHUDTime();
	UFUNCTION(Client, Reliable)
	void ClientSetHUDTime(bool InbUseTimeLimit, float InCurrentTime, float ServerTimeSeconds);

	void UpdateHUD();
	void UpdateHUDTime(float DeltaSeconds);
	void UpdateHUDCoolDown();
	void UpdateHUDVersusBar();
	void UpdateHUDTeamScroll(APolyWarBaseCharacter* DeathCharacter);

	void CreateWidgets();

	UFUNCTION(Client, Reliable)
	void ClientGameEnd(ETeamType WinnerTeam);

	void InitializeUnitMap();
	void ResetMapButtons();

	void CreateMap();

	UPROPERTY(EditAnywhere, Category ="Set Should")
	UMaterialInterface* MapInterface;
	UPROPERTY()
	UMaterialInstanceDynamic* MapMaterial;
	UPROPERTY()
	UTextureRenderTarget2D* MapRender;

	TMap<EUnitNum, EMapUnitState> UnitMap;
	TArray<UTextBlock*> StoreUnitTextBlocks;
	EOrderType CurrentOrder = EOrderType::EOD_MAX;
	UPROPERTY()
	UTextBlock* CurrentOrderText = nullptr;

	void SetHighlightUnit(bool bEnable, EUnitNum UnitNum);

	void GetMapUnitStateArray(EMapUnitState MapUnitState, TArray<EUnitNum>& OutUnitNumArray);
	FVector MapImageClickToWorldPosition(const FVector2D StartPos, const FVector2D Size, const FVector2D ClickPos);

	void GetClickedTeam(TArray<class APolyWarAICharacter*>& OutTeamArray);

	void StartOrder(EOrderType Order, FVector OrderPos = FVector::ZeroVector);

	UFUNCTION(Server, Reliable)
	void ServerStartOrder(EOrderType Order, FVector_NetQuantize OrderPos, const TArray<APolyWarAICharacter*>& TeamArray);

	void SetHUDSkillBar(class UProgressBar* WeaponSkillBar, EWeaponSkill WeaponSkill);
	void SetHUDSkillBar(UProgressBar* SpellBar, TSubclassOf<class ASpell> Spell);
	void SetHUDWinText(ETeamType WinTeam);
	void SetHUDTime();
	void SetHUDVersusBar();
	void SetHUDTeamScroll();

	void TeamUnitTypes(ETeamType TeamType, TMap<EUnitType, int32>& OutTeamUnitTypes);

	float BlueTeamNum = 1;
	float RedTeamNum = 1;

	FText FloatToTimeText(float TimeSeconds);

	bool bUseTimeLimit = false;
	float CurrentTime = 0.0f;

	//~ Begin Fog

	/*
	 * if true - FogOfWarClass's material need set 'FogOfWarWorldHide'
	 * if false - FogOfWarClass's material need set 'FogOfWarWorldReveal'
	 */
	UPROPERTY(EditDefaultsOnly, Category = "Set Should")
	bool bWorldHideStart = false;

	UPROPERTY(EditAnywhere, Category = "Set Should")
	TSubclassOf<class AStaticMeshActor> FogOfWarClass;
	UPROPERTY(EditAnywhere, Category = "Set Should")
	float FogHeightAdjust = 500.0f;
	UPROPERTY()
	AStaticMeshActor* FogOfWar;

	UPROPERTY(EditAnywhere, Category ="Set Should")
	UMaterialInterface* FogOfWarInterface;
	UPROPERTY(EditAnywhere, Category ="Set Should")
	UMaterialInterface* FogOfWarRevealInterface;
	UPROPERTY()
	UTextureRenderTarget2D* FogOfWarRender;
	UPROPERTY()
	UTextureRenderTarget2D* FogOfWarRevealRender;

	void CreateFog();
	void SetAllTeamCharacterFog(const TArray<AActor*>& AllCharacters);
	//~ End Fog

	void SetAllCharacterVisible(const TArray<AActor*>& AllCharacters);
	void SetAllCharacterCustomDepth(const TArray<AActor*>& AllCharacters);

public:
	EOrderType GetCurrentOrder() const {return CurrentOrder;}
	UMaterialInstanceDynamic* GetMapMaterial() const {return MapMaterial;}

};
