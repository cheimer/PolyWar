// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "PolyWarCharacterAnimInstance.generated.h"

/**
 * 
 */
UCLASS()
class POLYWAR_API UPolyWarCharacterAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public:
	virtual void NativeInitializeAnimation() override;
	virtual void NativeUpdateAnimation(float DeltaTime) override;

private:
	UPROPERTY(BlueprintReadOnly, Category = "Character", meta = (AllowPrivateAccess = "true"))
	class APolyWarBaseCharacter* PolyWarCharacter;

	UPROPERTY(BlueprintReadOnly, Category = "PolyWar", meta = (AllowPrivateAccess = "true"))
	float Speed;

	UPROPERTY(BlueprintReadOnly, Category = "PolyWar", meta = (AllowPrivateAccess = "true"))
	bool bIsRunning;

	UPROPERTY(BlueprintReadOnly, Category = "PolyWar", meta = (AllowPrivateAccess = "true"))
	bool bIsAttacking;

	UPROPERTY(BlueprintReadOnly, Category = "PolyWar", meta = (AllowPrivateAccess = "true"))
	bool bIsOpenMap = false;

	UPROPERTY(BlueprintReadOnly, Category = "PolyWar", meta = (AllowPrivateAccess = "true"))
	bool bUseMirror = false;

};
