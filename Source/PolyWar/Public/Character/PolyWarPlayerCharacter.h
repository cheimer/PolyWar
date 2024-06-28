// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/PolyWarBaseCharacter.h"
#include "InputActionValue.h"
#include "PolyWarPlayerCharacter.generated.h"

/**
 * 
 */
UCLASS()
class POLYWAR_API APolyWarPlayerCharacter : public APolyWarBaseCharacter
{
	GENERATED_BODY()
	
public:
	APolyWarPlayerCharacter();
	virtual void PossessedBy(AController* NewController) override;
	virtual void PostInitializeComponents() override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void Tick(float DeltaSeconds) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual void SetPlayerDeath() override;

	void ResetMapCamera();
	void SetMapCameraRender(UTextureRenderTarget2D* MapRender);
	FTransform GetMainCameraTransform();
	FVector GetMapCameraPos();
	FVector GetMapCameraForward();
	FVector GetMapCameraRight();
	FVector GetMapCameraUp();

	EWeaponSkill GetWeaponSkillFirst();
	EWeaponSkill GetWeaponSkillSecond();

	float GetWeaponSkillCoolDown(EWeaponSkill WeaponSkill);
	float GetWeaponSkillRemainCoolDown(EWeaponSkill WeaponSkill);

	TSubclassOf<ASpell> GetSpellFirst();
	TSubclassOf<ASpell> GetSpellSecond();
	TSubclassOf<ASpell> GetSpellUlt();

	float GetSpellCoolDown(TSubclassOf<ASpell> Spell);
	float GetSpellRemainCoolDown(TSubclassOf<ASpell> Spell);

	UPROPERTY(EditDefaultsOnly, Category = "Set Should")
	TSubclassOf<APolyWarPlayerCharacter> SpectatorClass;

	void SpectatorSettings(APolyWarPlayerCharacter* BeforeCharacter);

protected:
	virtual void BeginPlay() override;

	//~ Begin Components

	UPROPERTY(VisibleAnywhere, Category = "Camera")
	class USpringArmComponent* CharacterSpringArm;
	UPROPERTY(VisibleAnywhere, Category = "Camera")
	class UCameraComponent* MainCamera;

	UPROPERTY(VisibleAnywhere, Category = "Camera")
	USpringArmComponent* MapSpringArm;
	UPROPERTY(VisibleAnywhere, Category = "Camera")
	class USceneCaptureComponent2D* MapCamera;
	//~ End Components

	//~ Begin EnhancedInput

	UPROPERTY(EditDefaultsOnly, Category = "EnhancedInput")
	class UInputMappingContext* PlayerInputMapping;

	UPROPERTY(EditDefaultsOnly, Category = "EnhancedInput")
	class UInputAction* InputMove;

	UPROPERTY(EditDefaultsOnly, Category = "EnhancedInput")
	UInputAction* InputLook;

	UPROPERTY(EditDefaultsOnly, Category = "EnhancedInput")
	UInputAction* InputJump;

	UPROPERTY(EditDefaultsOnly, Category = "EnhancedInput")
	UInputAction* InputLeftMouse;

	UPROPERTY(EditDefaultsOnly, Category = "EnhancedInput")
	UInputAction* InputRun;

	UPROPERTY(EditDefaultsOnly, Category = "EnhancedInput")
	UInputAction* InputMap;

	UPROPERTY(EditDefaultsOnly, Category = "EnhancedInput")
	UInputAction* InputMapLeftClick;

	UPROPERTY(EditDefaultsOnly, Category = "EnhancedInput")
	UInputAction* InputMapRightClick;

	UPROPERTY(EditDefaultsOnly, Category = "EnhancedInput")
	UInputAction* InputMapMove;

	UPROPERTY(EditDefaultsOnly, Category = "EnhancedInput")
	UInputAction* InputMapScroll;

	UPROPERTY(EditDefaultsOnly, Category = "EnhancedInput")
	UInputAction* InputWeaponSkillFirst;

	UPROPERTY(EditDefaultsOnly, Category = "EnhancedInput")
	UInputAction* InputWeaponSkillSecond;

	UPROPERTY(EditDefaultsOnly, Category = "EnhancedInput")
	UInputAction* InputSpellFirst;

	UPROPERTY(EditDefaultsOnly, Category = "EnhancedInput")
	UInputAction* InputSpellSecond;

	UPROPERTY(EditDefaultsOnly, Category = "EnhancedInput")
	UInputAction* InputSpellUlt;

	void MoveForwardRight(const FInputActionValue& Value);
	void LookUpRight(const FInputActionValue& Value);
	virtual void Jump() override;

	void LeftMousePressedAndReleased(const FInputActionValue& Value);

	void Run(const FInputActionValue& Value);
	UFUNCTION(Server, UnReliable)
	void ServerRun(bool InIsRun);

	void Map(const FInputActionValue& Value);

	void MapLeftClick(const FInputActionValue& Value);
	void MapRightClick(const FInputActionValue& Value);
	void MapMove(const FInputActionValue& Value);
	void MapScroll(const FInputActionValue& Value);

	void WeaponSkillFirst(const FInputActionValue& Value);
	void WeaponSkillSecond(const FInputActionValue& Value);

	void SpellFirst(const FInputActionValue& Value);
	void SpellSecond(const FInputActionValue& Value);
	void SpellUlt(const FInputActionValue& Value);
	//~ End EnhancedInput

private:
	TObjectPtr<class APolyWarPlayerController> PolyWarPlayerController;

	void SetPlayerTeam();

	UPROPERTY(EditAnywhere, Category = "Settable")
	float MapDefaultHeight = 4000.0f;
	UPROPERTY(EditAnywhere, Category = "Settable")
	float MapHeightMaxLimit = 10000.0f;
	UPROPERTY(EditAnywhere, Category = "Settable")
	float MapHeightMinLimit = 1000.0f;

	UPROPERTY(EditAnywhere, Category = "Settable")
	float MapMoveSensitive = 100.0f;
	UPROPERTY(EditAnywhere, Category = "Settable")
	float MapRotateSensitive = 1.0f;
	UPROPERTY(EditAnywhere, Category = "Settable")
	float MapScrollSensitive = 500.0f;

	//~ Begin EnhancedInput

	bool bHoldLeftMouseClick = false;
	bool bHoldRightMouseClick = false;

	FRotator MapCameraDefaultRotator;
	//~ End EnhancedInput

	UPROPERTY(Replicated)
	bool bFocusOnScreen = false;

	UFUNCTION(Server, Unreliable)
	void ServerSetActorRotation(const FRotator& RotateDirection);

public:
	USpringArmComponent* GetMapSpringArm() const {return MapSpringArm;}
	float GetMapHeightMaxLimit() const {return MapHeightMaxLimit;}
	float GetMapHeightMinLimit() const {return MapHeightMinLimit;}
	void SetIsFocusOnScreen(const bool InIsFocus) {bFocusOnScreen = InIsFocus;}

};