// Fill out your copyright notice in the Description page of Project Settings.


#include "Controller/PolyWarPlayerController.h"

#include "UI/PolyWarHUD.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputMappingContext.h"
#include "InputActionValue.h"
#include "Character/PolyWarBaseCharacter.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "UI/CharacterWidget.h"


APolyWarPlayerController::APolyWarPlayerController()
{
	PrimaryActorTick.bCanEverTick = true;
}

void APolyWarPlayerController::BeginPlay()
{
	Super::BeginPlay();

	if (UEnhancedInputLocalPlayerSubsystem* Subsystem =
			ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
	{
		Subsystem->AddMappingContext(ControllerInputMapping, 0);
	}

}

void APolyWarPlayerController::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	PollInit();
}

void APolyWarPlayerController::PollInit()
{
	if(!bSetHUD)
	{
		PolyWarHUD = PolyWarHUD == nullptr ? Cast<APolyWarHUD>(GetHUD()) : PolyWarHUD;
		if(PolyWarHUD)
		{
			bSetHUD = true;
			PolyWarHUD->AddCharacterWidget();
		}
	}

	if(bSetHUD && !bSetOwner)
	{
		APolyWarBaseCharacter* PolyCharacter = Cast<APolyWarBaseCharacter>(GetOwner());
		if(PolyCharacter)
		{
			bSetOwner = true;
			SetHUDHealth(PolyCharacter->GetCurrentHealth(), PolyCharacter->GetMaxHealth());
		}
	}
}

void APolyWarPlayerController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

}

void APolyWarPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	if(!InputComponent) return;

	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(InputComponent))
	{
		;
	}
}

void APolyWarPlayerController::SetHUDHealth(float CurrentHealth, float MaxHealth)
{
	PolyWarHUD = PolyWarHUD == nullptr ? Cast<APolyWarHUD>(GetHUD()) : PolyWarHUD;

	bool bHUDValid = PolyWarHUD &&
		PolyWarHUD->CharacterWidget &&
		PolyWarHUD->CharacterWidget->HealthBar &&
		PolyWarHUD->CharacterWidget->HealthText;

	if(bHUDValid)
	{
		const float HealthPercent = CurrentHealth / MaxHealth;
		PolyWarHUD->CharacterWidget->HealthBar->SetPercent(HealthPercent);
		const FString HealthText = FString::Printf(TEXT("%d / %d"), FMath::CeilToInt(CurrentHealth), FMath::CeilToInt(MaxHealth));
		PolyWarHUD->CharacterWidget->HealthText->SetText(FText::FromString(HealthText));
	}
}
