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
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	virtual void PossessedBy(AController* NewController) override;
	virtual void PostInitializeComponents() override;

	void ResetMapCameraLocation();
	FVector GetMapCameraPos();

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
	UInputAction* InputMapClick;

	UPROPERTY(EditDefaultsOnly, Category = "EnhancedInput")
	UInputAction* InputMapMove;

	UPROPERTY(EditDefaultsOnly, Category = "EnhancedInput")
	UInputAction* InputMapScroll;

	void MoveForwardRight(const FInputActionValue& Value);
	void LookUpRight(const FInputActionValue& Value);
	virtual void Jump() override;

	void LeftMousePressedAndReleased(const FInputActionValue& Value);

	void Run(const FInputActionValue& Value);
	UFUNCTION(Server, UnReliable)
	void ServerRun(bool InIsRun);

	void Map(const FInputActionValue& Value);

	void MapClick(const FInputActionValue& Value);
	void MapMove(const FInputActionValue& Value);
	void MapScroll(const FInputActionValue& Value);
	//~ End EnhancedInput

private:
	TObjectPtr<class APolyWarPlayerController> PolyWarPlayerController;

	UPROPERTY(EditAnywhere, Category = "Settable")
	float MapDefaultHeight = 2200.0f;
	UPROPERTY(EditAnywhere, Category = "Settable")
	float MapHeightMaxLimit = 5000.0f;
	UPROPERTY(EditAnywhere, Category = "Settable")
	float MapHeightMinLimit = 1000.0f;

	UPROPERTY(EditAnywhere, Category = "Settable")
	float MapMoveSensitive = 100.0f;

	//~ TODO: MapMoveError Temp Solve
	UPROPERTY(EditAnywhere, Category = "Set Should")
	TObjectPtr<class UTextureRenderTarget2D> CanvasRenderTarget1;

	UPROPERTY(EditAnywhere, Category = "Set Should")
	TObjectPtr<class UTextureRenderTarget2D> CanvasRenderTarget2;

	UFUNCTION(Server, Reliable)
	void ServerSetMapCamera();
	//~ TODO END

	//~ Begin EnhancedInput
	bool bHoldMouseClick = false;
	//~ End EnhancedInput

public:
	USpringArmComponent* GetMapSpringArm() const {return MapSpringArm;}
	float GetMapHeightMaxLimit() const {return MapHeightMaxLimit;}
	float GetMapHeightMinLimit() const {return MapHeightMinLimit;}

};