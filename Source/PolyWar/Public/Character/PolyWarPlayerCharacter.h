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
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	virtual void PossessedBy(AController* NewController) override;

protected:
	virtual void BeginPlay() override;

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

	void MoveForwardRight(const FInputActionValue& Value);
	void LookUpRight(const FInputActionValue& Value);
	virtual void Jump() override;
	void LeftMousePressedAndReleased(const FInputActionValue& Value);
	//~ End EnhancedInput

};