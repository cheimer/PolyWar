// Fill out your copyright notice in the Description page of Project Settings.


#include "Controller/PolyWarPlayerController.h"

#include "UI/PolyWarHUD.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputMappingContext.h"
#include "InputActionValue.h"
#include "Character/PolyWarAICharacter.h"
#include "Character/PolyWarBaseCharacter.h"
#include "Character/PolyWarPlayerCharacter.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "Engine/StaticMeshActor.h"
#include "Engine/TextureRenderTarget2D.h"
#include "GameMode/PolyWarGameModeBase.h"
#include "GameState/PolyWarGameStateBase.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMaterialLibrary.h"
#include "Kismet/KismetRenderingLibrary.h"
#include "PolyWar/PolyWar.h"
#include "UI/CharacterWidget.h"
#include "UI/EndMenuWidget.h"
#include "UI/MapButton.h"
#include "UI/MapWidget.h"
#include "UI/MenuWidget.h"


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

	if(IsLocalPlayerController())
	{
		CreateMap();
		CreateWidgets();
		CreateFog();

		// Delayed Begin
		FTimerHandle DelayTimer;
		GetWorldTimerManager().SetTimer(DelayTimer, this, &ThisClass::SetAllCharacters, 0.3f, false);
	}

	InitializeUnitMap();


}

void APolyWarPlayerController::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if(IsLocalController())
	{
		UpdateHUDTime(DeltaSeconds);
		UpdateHUDCoolDown();
		UpdateFog();
	}
}

void APolyWarPlayerController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	if (UEnhancedInputLocalPlayerSubsystem* Subsystem =
		ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
	{
		Subsystem->AddMappingContext(ControllerInputMapping, 0);
	}
}

void APolyWarPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	if(!InputComponent) return;
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(InputComponent))
	{
		if(InputUnitNum1)
		{
			EnhancedInputComponent->BindAction(InputUnitNum1, ETriggerEvent::Triggered, this, &ThisClass::ToggleUnitNum1);
		}
		if(InputUnitNum2)
		{
			EnhancedInputComponent->BindAction(InputUnitNum2, ETriggerEvent::Triggered, this, &ThisClass::ToggleUnitNum2);
		}
		if(InputUnitNum3)
		{
			EnhancedInputComponent->BindAction(InputUnitNum3, ETriggerEvent::Triggered, this, &ThisClass::ToggleUnitNum3);
		}
		if(InputUnitNum4)
		{
			EnhancedInputComponent->BindAction(InputUnitNum4, ETriggerEvent::Triggered, this, &ThisClass::ToggleUnitNum4);
		}
		if(InputUnitNum5)
		{
			EnhancedInputComponent->BindAction(InputUnitNum5, ETriggerEvent::Triggered, this, &ThisClass::ToggleUnitNum5);
		}
		if(InputUnitNum6)
		{
			EnhancedInputComponent->BindAction(InputUnitNum6, ETriggerEvent::Triggered, this, &ThisClass::ToggleUnitNum6);
		}
		if(InputUnitNum7)
		{
			EnhancedInputComponent->BindAction(InputUnitNum7, ETriggerEvent::Triggered, this, &ThisClass::ToggleUnitNum7);
		}
		if(InputUnitNum8)
		{
			EnhancedInputComponent->BindAction(InputUnitNum8, ETriggerEvent::Triggered, this, &ThisClass::ToggleUnitNum8);
		}
		if(InputOrderAttack)
		{
			EnhancedInputComponent->BindAction(InputOrderAttack, ETriggerEvent::Triggered, this, &ThisClass::ToggleOrderAttack);
		}
		if(InputOrderMove)
		{
			EnhancedInputComponent->BindAction(InputOrderMove, ETriggerEvent::Triggered, this, &ThisClass::ToggleOrderMove);
		}
		if(InputOrderStop)
		{
			EnhancedInputComponent->BindAction(InputOrderStop, ETriggerEvent::Triggered, this, &ThisClass::ToggleOrderStop);
		}
		if(InputOrderRush)
		{
			EnhancedInputComponent->BindAction(InputOrderRush, ETriggerEvent::Triggered, this, &ThisClass::ToggleOrderRush);
		}
		if(InputOrderHold)
		{
			EnhancedInputComponent->BindAction(InputOrderHold, ETriggerEvent::Triggered, this, &ThisClass::ToggleOrderHold);
		}
		if(InputOrderCancel)
		{
			EnhancedInputComponent->BindAction(InputOrderCancel, ETriggerEvent::Triggered, this, &ThisClass::ToggleOrderCancel);
		}
		if(InputMenu)
		{
			EnhancedInputComponent->BindAction(InputMenu, ETriggerEvent::Triggered, this, &ThisClass::ToggleMenu);
		}
	}
}

void APolyWarPlayerController::InitializeUnitMap()
{
	for(EUnitNum UnitNum : TEnumRange<EUnitNum>())
	{
		UnitMap.Emplace(UnitNum, EMapUnitState::EMUS_UnClicked);
	}
}

void APolyWarPlayerController::CreateWidgets()
{
	PolyWarHUD = PolyWarHUD == nullptr ? Cast<APolyWarHUD>(GetHUD()) : PolyWarHUD;
	if(PolyWarHUD)
	{
		PolyWarHUD->CreateWidgets();
		SetHUDVersusBar();
		SetHUDTeamScroll();
		SetHUDTime();

		UpdateHUD();
	}
}

void APolyWarPlayerController::CreateMap()
{
	PolyWarPlayerCharacter = PolyWarPlayerCharacter == nullptr ? Cast<APolyWarPlayerCharacter>(GetPawn()) : PolyWarPlayerCharacter;
	if(!PolyWarPlayerCharacter) return;

	if(MapInterface)
	{
		MapMaterial = UKismetMaterialLibrary::CreateDynamicMaterialInstance(this, MapInterface);
	}
	if(MapMaterial)
	{
		MapRender = NewObject<UTextureRenderTarget2D>();
		MapRender->InitAutoFormat(1600, 900);

		MapMaterial->SetTextureParameterValue(FName("Map"), MapRender);
	}
	if(MapRender)
	{
		PolyWarPlayerCharacter->SetMapCameraRender(MapRender);
	}
}

void APolyWarPlayerController::CreateFog()
{
	FogOfWar = GetWorld()->SpawnActor<AStaticMeshActor>(FogOfWarClass);

	FogOfWarRevealRender = NewObject<UTextureRenderTarget2D>();
	FogOfWarRevealRender->InitAutoFormat(1024, 1024);
	UKismetRenderingLibrary::ClearRenderTarget2D(this, FogOfWarRevealRender);

	auto FogMeshMaterial = UKismetMaterialLibrary::CreateDynamicMaterialInstance(this,
		FogOfWar->GetStaticMeshComponent()->GetMaterial(0));
	FogMeshMaterial->SetTextureParameterValue(FName("FogRevealRender"), FogOfWarRevealRender);

	if(bWorldHideStart)
	{
		FogOfWarRender = NewObject<UTextureRenderTarget2D>();
		FogOfWarRender->InitAutoFormat(1024, 1024);
		UKismetRenderingLibrary::ClearRenderTarget2D(this, FogOfWarRender);

		FogMeshMaterial->SetTextureParameterValue(FName("FogRender"), FogOfWarRender);
	}

	FogOfWar->GetStaticMeshComponent()->SetMaterial(0, FogMeshMaterial);

}

void APolyWarPlayerController::SetInputDefault(bool IsUI)
{
	if(!PolyWarPlayerCharacter) return;

	if(IsUI)
	{
		SetInputMode(FInputModeGameAndUI());
		SetShowMouseCursor(true);
		PolyWarPlayerCharacter->SetIsOpenUI(true);
	}
	else
	{
		SetInputMode(FInputModeGameOnly());
		SetShowMouseCursor(false);
		PolyWarPlayerCharacter->SetIsOpenUI(false);
	}
}

void APolyWarPlayerController::SetAllCharacters()
{
	PolyWarPlayerCharacter = PolyWarPlayerCharacter == nullptr ? Cast<APolyWarPlayerCharacter>(GetPawn()) : PolyWarPlayerCharacter;
	if(!PolyWarPlayerCharacter) return;

	TArray<AActor*> AllCharacters;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), APolyWarBaseCharacter::StaticClass(), AllCharacters);

	int32 CharacterNum = 0;
	bool bContinue = true;
	for(auto PolyWarCharacterIndex : AllCharacters)
	{
		APolyWarPlayerCharacter* PolyWarPlayerIndex = Cast<APolyWarPlayerCharacter>(PolyWarCharacterIndex);
		if(PolyWarPlayerIndex)
		{
			CharacterNum++;
			if(PolyWarPlayerIndex->GetTeamType() == ETeamType::ET_NoTeam)
			{
				bContinue = false;
			}
		}
	}
	if(CharacterNum < 2) bContinue = false;

	// Temp Solve: Delay restart
	// if func execute too early, immediately delay a little seconds
	if(!bContinue)
	{
		FTimerHandle DelayTimer;
		GetWorldTimerManager().SetTimer(DelayTimer, this, &ThisClass::SetAllCharacters, 0.1f, false);
		return;
	}

	SetAllTeamCharacterFog(AllCharacters);
	SetAllCharacterVisible(AllCharacters);
	SetAllCharacterCustomDepth(AllCharacters);
}

void APolyWarPlayerController::UpdateHUD()
{
	PolyWarHUD = PolyWarHUD == nullptr ? Cast<APolyWarHUD>(GetHUD()) : PolyWarHUD;
	if(PolyWarHUD)
	{
		UpdateHUDVersusBar();
		UpdateHUDTeamScroll(nullptr);
	}
}

void APolyWarPlayerController::UpdateHUDTime(float DeltaSeconds)
{
	PolyWarHUD = PolyWarHUD == nullptr ? Cast<APolyWarHUD>(GetHUD()) : PolyWarHUD;
	if(!PolyWarHUD) return;

	// bUseTimeLimit == true -> time 999:59 ~ 00:00
	if(bUseTimeLimit)
	{
		CurrentTime -= DeltaSeconds;
	}
	// bUseTimeLimit == false -> time 00:00 ~ 999:59
	else
	{
		CurrentTime += DeltaSeconds;
	}

	if(bUseTimeLimit && CurrentTime < 0.0f)
	{
		TimeEndGame();
	}

	if(PolyWarHUD->CharacterWidget && PolyWarHUD->CharacterWidget->TimeText)
	{
		FText TimeText = FloatToTimeText(CurrentTime);
		PolyWarHUD->CharacterWidget->TimeText->SetText(TimeText);
	}
}

void APolyWarPlayerController::UpdateHUDCoolDown()
{
	PolyWarHUD = PolyWarHUD == nullptr ? Cast<APolyWarHUD>(GetHUD()) : PolyWarHUD;
	if(!PolyWarHUD) return;
	PolyWarPlayerCharacter = PolyWarPlayerCharacter == nullptr ? Cast<APolyWarPlayerCharacter>(GetPawn()) : PolyWarPlayerCharacter;
	if(!PolyWarPlayerCharacter) return;

	// Update WeaponSkill cooldown
	if(PolyWarHUD->CharacterWidget && PolyWarHUD->CharacterWidget->WeaponSkillFirstBar)
	{
		SetHUDSkillBar(PolyWarHUD->CharacterWidget->WeaponSkillFirstBar, PolyWarPlayerCharacter->GetWeaponSkillFirst());
	}
	if(PolyWarHUD->CharacterWidget && PolyWarHUD->CharacterWidget->WeaponSkillSecondBar)
	{
		SetHUDSkillBar(PolyWarHUD->CharacterWidget->WeaponSkillSecondBar, PolyWarPlayerCharacter->GetWeaponSkillSecond());
	}

	// Update Spell cooldown
	if(PolyWarHUD->CharacterWidget && PolyWarHUD->CharacterWidget->SpellFirstBar)
	{
		SetHUDSkillBar(PolyWarHUD->CharacterWidget->SpellFirstBar, PolyWarPlayerCharacter->GetSpellFirst());
	}
	if(PolyWarHUD->CharacterWidget && PolyWarHUD->CharacterWidget->SpellSecondBar)
	{
		SetHUDSkillBar(PolyWarHUD->CharacterWidget->SpellSecondBar, PolyWarPlayerCharacter->GetSpellSecond());
	}
	if(PolyWarHUD->CharacterWidget && PolyWarHUD->CharacterWidget->SpellUltBar)
	{
		SetHUDSkillBar(PolyWarHUD->CharacterWidget->SpellUltBar, PolyWarPlayerCharacter->GetSpellUlt());
	}

}

void APolyWarPlayerController::UpdateHUDVersusBar()
{
	PolyWarHUD = PolyWarHUD == nullptr ? Cast<APolyWarHUD>(GetHUD()) : PolyWarHUD;
	if(!PolyWarHUD) return;

	if(PolyWarHUD->CharacterWidget && PolyWarHUD->CharacterWidget->VersusBar)
	{
		PolyWarHUD->CharacterWidget->VersusBar->SetPercent(BlueTeamNum / (BlueTeamNum + RedTeamNum));
	}

	if(PolyWarHUD->EndMenuWidget && PolyWarHUD->EndMenuWidget->VersusBar)
	{
		PolyWarHUD->EndMenuWidget->VersusBar->SetPercent(BlueTeamNum / (BlueTeamNum + RedTeamNum));
	}
}

void APolyWarPlayerController::UpdateHUDTeamScroll(APolyWarBaseCharacter* DeathCharacter)
{
	if(!DeathCharacter || Cast<APolyWarPlayerCharacter>(DeathCharacter)) return;

	PolyWarHUD = PolyWarHUD == nullptr ? Cast<APolyWarHUD>(GetHUD()) : PolyWarHUD;
	if(!PolyWarHUD) return;

	PolyWarHUD->EndMenuScrollMinus(DeathCharacter->GetTeamType(), DeathCharacter->GetUnitType());
}

void APolyWarPlayerController::UpdateFog()
{
	if(FogOfWarRevealRender)
	{
		UKismetRenderingLibrary::ClearRenderTarget2D(this, FogOfWarRevealRender);
	}
}

void APolyWarPlayerController::SetAllTeamCharacterFog(const TArray<AActor*>& AllCharacters)
{
	PolyWarPlayerCharacter = PolyWarPlayerCharacter == nullptr ? Cast<APolyWarPlayerCharacter>(GetPawn()) : PolyWarPlayerCharacter;
	if(!PolyWarPlayerCharacter) return;

	for(auto CharacterIndex : AllCharacters)
	{
		auto PolyCharacterIndex = Cast<APolyWarBaseCharacter>(CharacterIndex);
		if(PolyCharacterIndex && PolyCharacterIndex->GetTeamType() == PolyWarPlayerCharacter->GetTeamType())
		{
			if(bWorldHideStart)
			{
				PolyCharacterIndex->CreateFogOfWar(FogOfWarInterface,FogOfWarRender,
					FogOfWarRevealInterface, FogOfWarRevealRender);
			}
			else
			{
				PolyCharacterIndex->CreateFogOfWar(FogOfWarRevealInterface, FogOfWarRevealRender);
			}
		}
	}
}

void APolyWarPlayerController::SetAllCharacterVisible(const TArray<AActor*>& AllCharacters)
{
	PolyWarPlayerCharacter = PolyWarPlayerCharacter == nullptr ? Cast<APolyWarPlayerCharacter>(GetPawn()) : PolyWarPlayerCharacter;
	if(!PolyWarPlayerCharacter) return;

	for(auto CharacterIndex : AllCharacters)
	{
		auto PolyCharacterIndex = Cast<APolyWarBaseCharacter>(CharacterIndex);
		if(PolyCharacterIndex && PolyCharacterIndex->GetTeamType() != PolyWarPlayerCharacter->GetTeamType())
		{
			PolyCharacterIndex->GetMesh()->SetVisibility(false, true);
		}
	}
}

void APolyWarPlayerController::SetAllCharacterCustomDepth(const TArray<AActor*>& AllCharacters)
{
	PolyWarPlayerCharacter = PolyWarPlayerCharacter == nullptr ? Cast<APolyWarPlayerCharacter>(GetPawn()) : PolyWarPlayerCharacter;
	if(!PolyWarPlayerCharacter) return;

	for(auto CharacterIndex : AllCharacters)
	{
		auto PolyAICharacterIndex = Cast<APolyWarAICharacter>(CharacterIndex);
		if(PolyAICharacterIndex)
		{
			PolyAICharacterIndex->SetCustomDepth(true, PolyAICharacterIndex->GetTeamType());
		}
		else
		{
			auto PolyPlayerCharacterIndex = Cast<APolyWarPlayerCharacter>(CharacterIndex);
			if(PolyPlayerCharacterIndex)
			{
				if(PolyPlayerCharacterIndex->GetTeamType() != PolyWarPlayerCharacter->GetTeamType())
				{
					PolyPlayerCharacterIndex->SetCustomDepth(true, PolyPlayerCharacterIndex->GetTeamType());
				}
			}
		}
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

void APolyWarPlayerController::SetHUDDeathCharacter(APolyWarBaseCharacter* DeathCharacter)
{
	if(!DeathCharacter || Cast<APolyWarPlayerCharacter>(DeathCharacter)) return;

	if(DeathCharacter->GetTeamType() == ETeamType::ET_BlueTeam)
	{
		BlueTeamNum--;
	}
	else if(DeathCharacter->GetTeamType() == ETeamType::ET_RedTeam)
	{
		RedTeamNum--;
	}

	UpdateHUDVersusBar();
	UpdateHUDTeamScroll(DeathCharacter);
}

void APolyWarPlayerController::SetHUDSkillBar(UProgressBar* WeaponSkillBar, EWeaponSkill WeaponSkill)
{
	if(!WeaponSkillBar) return;

	const float WeaponSkillFirstCoolTime = PolyWarPlayerCharacter->GetWeaponSkillCoolDown(WeaponSkill);
	const float WeaponSkillFirstRemainCoolTime = PolyWarPlayerCharacter->GetWeaponSkillRemainCoolDown(WeaponSkill);

	WeaponSkillBar->SetPercent(WeaponSkillFirstRemainCoolTime / WeaponSkillFirstCoolTime);
}

void APolyWarPlayerController::SetHUDSkillBar(UProgressBar* SpellBar, TSubclassOf<ASpell> Spell)
{
	if(!SpellBar) return;

	const float SpellFirstCoolTime = PolyWarPlayerCharacter->GetSpellCoolDown(Spell);
	const float SpellFirstRemainCoolTime = PolyWarPlayerCharacter->GetSpellRemainCoolDown(Spell);

	SpellBar->SetPercent(SpellFirstRemainCoolTime / SpellFirstCoolTime);
}

void APolyWarPlayerController::SetHUDWinText(ETeamType WinTeam)
{
	PolyWarHUD = PolyWarHUD == nullptr ? Cast<APolyWarHUD>(GetHUD()) : PolyWarHUD;

	bool bHUDValid = PolyWarHUD &&
		PolyWarHUD->EndMenuWidget &&
		PolyWarHUD->EndMenuWidget->WinText;

	if(bHUDValid)
	{
		FString WinText;
		FSlateColor WinColor;
		if(WinTeam == ETeamType::ET_BlueTeam)
		{
			WinText = FString::Printf(TEXT("Blue Team Win"));
			WinColor = PolyWarHUD->EndMenuWidget->BlueTeamColor;
		}
		else if(WinTeam == ETeamType::ET_RedTeam)
		{
			WinText = FString::Printf(TEXT("Red Team Win"));
			WinColor = PolyWarHUD->EndMenuWidget->RedTeamColor;
		}
		else
		{
			WinText = FString::Printf(TEXT("??? Team Win"));
		}
		PolyWarHUD->EndMenuWidget->WinText->SetText(FText::FromString(WinText));
		PolyWarHUD->EndMenuWidget->WinText->SetColorAndOpacity(WinColor);
	}
}

void APolyWarPlayerController::SetHUDTime()
{
	PolyWarHUD = PolyWarHUD == nullptr ? Cast<APolyWarHUD>(GetHUD()) : PolyWarHUD;
	if(!PolyWarHUD || !PolyWarHUD->CharacterWidget) return;

	if(HasAuthority())
	{
		APolyWarGameModeBase* GameMode = Cast<APolyWarGameModeBase>(GetWorld()->GetAuthGameMode());
		if(!GameMode) return;

		bUseTimeLimit = GameMode->bUseTimeLimit;
		if(bUseTimeLimit)
		{
			CurrentTime = GameMode->TimeLimit;
		}
	}
	else
	{
		ServerSetHUDTime();
	}

}

void APolyWarPlayerController::ServerSetHUDTime_Implementation()
{
	APolyWarGameModeBase* GameMode = Cast<APolyWarGameModeBase>(GetWorld()->GetAuthGameMode());
	if(!GameMode) return;

	float InCurrentTime = 0.0f;
	if(GameMode->bUseTimeLimit)
	{
		InCurrentTime = GameMode->TimeLimit;
	}
	ClientSetHUDTime(GameMode->bUseTimeLimit, InCurrentTime, GetWorld()->GetTimeSeconds());
}

void APolyWarPlayerController::ClientSetHUDTime_Implementation(bool InbUseTimeLimit, float InCurrentTime, float ServerTimeSeconds)
{
	bUseTimeLimit = InbUseTimeLimit;
	CurrentTime = InCurrentTime;
	if(bUseTimeLimit)
	{
		CurrentTime -= ServerTimeSeconds;
	}
	else
	{
		CurrentTime += ServerTimeSeconds;
	}

}

void APolyWarPlayerController::SetHUDVersusBar()
{
	PolyWarHUD = PolyWarHUD == nullptr ? Cast<APolyWarHUD>(GetHUD()) : PolyWarHUD;

	bool bHUDValid = PolyWarHUD &&
		PolyWarHUD->EndMenuWidget &&
		PolyWarHUD->EndMenuWidget->VersusBar;

	if(bHUDValid)
	{
		PolyWarGameState = PolyWarGameState == nullptr ? Cast<APolyWarGameStateBase>(UGameplayStatics::GetGameState(GetWorld())) : PolyWarGameState;
		if(!PolyWarGameState) return;

		TArray<APolyWarBaseCharacter*> BlueTeamArray;
		PolyWarGameState->GetTeam(ETeamType::ET_BlueTeam, BlueTeamArray);
		BlueTeamNum = BlueTeamArray.Num();

		TArray<APolyWarBaseCharacter*> RedTeamArray;
		PolyWarGameState->GetTeam(ETeamType::ET_RedTeam, RedTeamArray);
		RedTeamNum = RedTeamArray.Num();

		PolyWarHUD->EndMenuWidget->VersusBar->SetPercent(BlueTeamNum / (BlueTeamNum + RedTeamNum));
	}
}

void APolyWarPlayerController::SetHUDTeamScroll()
{
	PolyWarHUD = PolyWarHUD == nullptr ? Cast<APolyWarHUD>(GetHUD()) : PolyWarHUD;

	bool bHUDValid = PolyWarHUD &&
		PolyWarHUD->EndMenuWidget &&
		PolyWarHUD->EndMenuWidget->BlueTeamScroll &&
		PolyWarHUD->EndMenuWidget->RedTeamScroll;

	if(bHUDValid)
	{
		PolyWarHUD->ClearEndMenuScroll();

		TMap<EUnitType, int32> BlueTeamUnitTypes;
		TeamUnitTypes(ETeamType::ET_BlueTeam, BlueTeamUnitTypes);

		for(auto UnitInfo : BlueTeamUnitTypes)
		{
			PolyWarHUD->EndMenuScrollAdd(ETeamType::ET_BlueTeam, UnitInfo.Key, UnitInfo.Value);
		}

		TMap<EUnitType, int32> RedTeamUnitTypes;
		TeamUnitTypes(ETeamType::ET_RedTeam, RedTeamUnitTypes);

		for(auto UnitInfo : RedTeamUnitTypes)
		{
			PolyWarHUD->EndMenuScrollAdd(ETeamType::ET_RedTeam, UnitInfo.Key, UnitInfo.Value);
		}

	}
}

void APolyWarPlayerController::TeamUnitTypes(ETeamType TeamType, TMap<EUnitType, int32>& OutTeamUnitTypes)
{
	TArray<APolyWarBaseCharacter*> TeamArray;
	PolyWarGameState->GetTeam(TeamType, TeamArray);
	for(auto TeamCharacter : TeamArray)
	{
		if(OutTeamUnitTypes.Contains(TeamCharacter->GetUnitType()))
		{
			OutTeamUnitTypes[TeamCharacter->GetUnitType()]++;
		}
		else
		{
			OutTeamUnitTypes.Add(TeamCharacter->GetUnitType(), 1);
		}
	}
}

void APolyWarPlayerController::TimeEndGame()
{
	if(HasAuthority())
	{
		APolyWarGameModeBase* GameMode = Cast<APolyWarGameModeBase>(GetWorld()->GetAuthGameMode());
		if(GameMode)
		{
			GameMode->TimeEnd();
		}
	}
}

void APolyWarPlayerController::SurrenderGame()
{
	PolyWarPlayerCharacter = PolyWarPlayerCharacter == nullptr ? Cast<APolyWarPlayerCharacter>(GetPawn()) : PolyWarPlayerCharacter;
	if(!PolyWarPlayerCharacter) return;

	const ETeamType WinnerTeam = PolyWarPlayerCharacter->GetTeamType() == ETeamType::ET_BlueTeam ? ETeamType::ET_RedTeam : ETeamType::ET_BlueTeam;

	if(HasAuthority())
	{
		APolyWarGameModeBase* GameMode = Cast<APolyWarGameModeBase>(GetWorld()->GetAuthGameMode());
		if(GameMode)
		{
			GameMode->GameEnd(WinnerTeam);
		}
	}
	else
	{
		ServerSurrenderGame();
	}
}

void APolyWarPlayerController::ServerSurrenderGame_Implementation()
{
	SurrenderGame();
}

void APolyWarPlayerController::GameEnd(ETeamType WinnerTeam)
{
	if(HasAuthority())
	{
		if(IsLocalController())
		{
			PolyWarHUD = PolyWarHUD == nullptr ? Cast<APolyWarHUD>(GetHUD()) : PolyWarHUD;
			if(!PolyWarHUD) return;

			PolyWarPlayerCharacter = PolyWarPlayerCharacter == nullptr ? Cast<APolyWarPlayerCharacter>(GetPawn()) : PolyWarPlayerCharacter;
			if(!PolyWarPlayerCharacter) return;

			PolyWarPlayerCharacter->SetIsGameEnd(true);

			SetInputDefault(true);
			SetHUDWinText(WinnerTeam);

			PolyWarHUD->ChangeWidget(PolyWarHUD->EndMenuWidget);
		}
		else
		{
			ClientGameEnd(WinnerTeam);
		}
	}

}

void APolyWarPlayerController::ClientGameEnd_Implementation(ETeamType WinnerTeam)
{
	if(IsLocalController())
	{
		PolyWarHUD = PolyWarHUD == nullptr ? Cast<APolyWarHUD>(GetHUD()) : PolyWarHUD;
		if(!PolyWarHUD) return;

		PolyWarPlayerCharacter = PolyWarPlayerCharacter == nullptr ? Cast<APolyWarPlayerCharacter>(GetPawn()) : PolyWarPlayerCharacter;
		if(!PolyWarPlayerCharacter) return;

		PolyWarPlayerCharacter->SetIsGameEnd(true);

		SetInputDefault(true);
		SetHUDWinText(WinnerTeam);

		PolyWarHUD->ChangeWidget(PolyWarHUD->EndMenuWidget);
	}
}

void APolyWarPlayerController::MapToggle()
{
	PolyWarHUD = PolyWarHUD == nullptr ? Cast<APolyWarHUD>(GetHUD()) : PolyWarHUD;
	PolyWarPlayerCharacter = PolyWarPlayerCharacter == nullptr ? Cast<APolyWarPlayerCharacter>(GetPawn()) : PolyWarPlayerCharacter;
	if(!PolyWarHUD || !PolyWarHUD->MapWidget || !PolyWarPlayerCharacter) return;

	// Map Close
	if(PolyWarHUD->IsCurrentWidget(PolyWarHUD->MapWidget))
	{
		PolyWarHUD->SetDefaultScreenWidget();

		SetInputDefault(false);
		PolyWarPlayerCharacter->ResetMapCamera();
		ResetMapButtons();
	}
	// Map Open
	else
	{
		PolyWarHUD->ChangeWidget(PolyWarHUD->MapWidget);

		SetInputDefault(true);
		PolyWarPlayerCharacter->ResetMapCamera();
		ResetMapButtons();
	}
}

void APolyWarPlayerController::ResetMapButtons()
{
	// UnitMap Reset
	for(EUnitNum UnitNum : TEnumRange<EUnitNum>())
	{
		if(UnitMap.Find(UnitNum))
		{
			if(UnitMap[UnitNum] == EMapUnitState::EMUS_Clicked)
			{
				UnitMap[UnitNum] = EMapUnitState::EMUS_UnClicked;
				SetHighlightUnit(false, UnitNum);
			}
		}
	}
	// UnitTextBlock Reset
	for(UTextBlock* TextBlock : StoreUnitTextBlocks)
	{
		TextBlock->SetColorAndOpacity(FColor::White);
	}
	StoreUnitTextBlocks.Empty();

	// Order Reset
	CurrentOrder = EOrderType::EOD_MAX;
	if(CurrentOrderText != nullptr)
	{
		CurrentOrderText->SetColorAndOpacity(FColor::White);
		CurrentOrderText = nullptr;
	}
}

void APolyWarPlayerController::SetHighlightUnit(bool bEnable, EUnitNum UnitNum)
{
	PolyWarPlayerCharacter = PolyWarPlayerCharacter == nullptr ? Cast<APolyWarPlayerCharacter>(GetPawn()) : PolyWarPlayerCharacter;
	PolyWarGameState = PolyWarGameState == nullptr ? Cast<APolyWarGameStateBase>(UGameplayStatics::GetGameState(GetWorld())) : PolyWarGameState;
	if(!PolyWarPlayerCharacter || !PolyWarGameState) return;

	TArray<EUnitNum> UnitNums;
	UnitNums.Add(UnitNum);

	TArray<APolyWarAICharacter*> TeamAIs;
	PolyWarGameState->GetTeamByUnitNums(PolyWarPlayerCharacter->GetTeamType(), UnitNums, TeamAIs);

	for (auto TeamAI : TeamAIs)
	{
		if(TeamAI->GetUnitNum() == UnitNum)
		{
			TeamAI->SetHighlight(bEnable);
		}
	}
}

void APolyWarPlayerController::GetMapUnitStateArray(EMapUnitState MapUnitState, TArray<EUnitNum>& OutUnitNumArray)
{
	for(EUnitNum UnitNum : TEnumRange<EUnitNum>())
	{
		if(UnitMap.Contains(UnitNum) && UnitMap[UnitNum] == MapUnitState)
		{
			OutUnitNumArray.Add(UnitNum);
		}
	}
}

void APolyWarPlayerController::MapUnitToggle(EUnitNum UnitNum, UTextBlock* UnitText)
{
	if(!UnitText || !UnitMap.Find(UnitNum)) return;
	if(!StoreUnitTextBlocks.Contains(UnitText))
	{
		StoreUnitTextBlocks.Emplace(UnitText);
	}

	if(UnitMap[UnitNum] == EMapUnitState::EMUS_UnClicked)
	{
		UnitMap[UnitNum] = EMapUnitState::EMUS_Clicked;
		UnitText->SetColorAndOpacity(FColor::Green);
		SetHighlightUnit(true, UnitNum);
	}
	else if(UnitMap[UnitNum] == EMapUnitState::EMUS_Clicked)
	{
		UnitMap[UnitNum] = EMapUnitState::EMUS_UnClicked;
		UnitText->SetColorAndOpacity(FColor::White);
		SetHighlightUnit(false, UnitNum);
	}
	else if(UnitMap[UnitNum] == EMapUnitState::EMUS_Removed)
	{
		;
	}
}

void APolyWarPlayerController::MapOrderToggle(EOrderType OrderType, UTextBlock* OrderText)
{
	if(!OrderText) return;
	if(OrderType == CurrentOrder)
	{
		CurrentOrder = EOrderType::EOD_MAX;
		OrderText->SetColorAndOpacity(FColor::White);
		CurrentOrderText = OrderText;
		return;
	}

	// Before TextBlock Color Reset
	if(OrderType != CurrentOrder && CurrentOrder != EOrderType::EOD_MAX)
	{
		if(CurrentOrderText)
		{
			CurrentOrderText->SetColorAndOpacity(FColor::White);
		}
	}

	switch(OrderType)
	{
	// Apply When Click Map
	case EOrderType::EOD_Move :
	case EOrderType::EOD_Attack :
	case EOrderType::EOD_Rush :
		CurrentOrder = OrderType;
		CurrentOrderText = OrderText;
		CurrentOrderText->SetColorAndOpacity(FColor::Green);
		break;

	// Apply Immediately
	case EOrderType::EOD_Hold :
	case EOrderType::EOD_Stop :
		StartOrder(OrderType);
		break;
	case EOrderType::EOD_Cancel :
		ResetMapButtons();
		break;
	}
}

void APolyWarPlayerController::MapImageClick(const FVector2D StartPos, const FVector2D Size, const FVector2D ClickPos)
{
	if(CurrentOrder == EOrderType::EOD_MAX || !CurrentOrderText) return;

	FVector MapPosition = MapImageClickToWorldPosition(StartPos, Size, ClickPos);
	StartOrder(CurrentOrder, MapPosition);
}

FVector APolyWarPlayerController::MapImageClickToWorldPosition(const FVector2D StartPos, const FVector2D Size, const FVector2D ClickPos)
{
	PolyWarPlayerCharacter = PolyWarPlayerCharacter == nullptr ? Cast<APolyWarPlayerCharacter>(GetPawn()) : PolyWarPlayerCharacter;
	if(!PolyWarPlayerCharacter) return FVector();

	// -0.5 < Rate < 0.5
	// Cursor pos rate based on screen size. if left top, rate(-0.x, -0.x). if right bottom, rate(0.x, 0.x)
	FVector2D Rate((StartPos + Size - ClickPos) / Size);
	Rate.X = 0.5 - Rate.X; Rate.Y = 0.5 - Rate.Y;

	FVector MapCameraPos = PolyWarPlayerCharacter->GetMapCameraPos();

	FVector TraceStartPos = MapCameraPos;
	TraceStartPos += -PolyWarPlayerCharacter->GetMapCameraUp() * Rate.Y * MapCameraPos.Z * 1.1f;
	TraceStartPos += PolyWarPlayerCharacter->GetMapCameraRight() * Rate.X * MapCameraPos.Z * 2.0f;

	FVector TraceEndPos = TraceStartPos + PolyWarPlayerCharacter->GetMapCameraForward() * PolyWarPlayerCharacter->GetMapHeightMaxLimit() * 2.0f;

	FHitResult HitResult;
	GetWorld()->LineTraceSingleByChannel(HitResult, TraceStartPos, TraceEndPos, ECC_WorldStatic);

	return HitResult.ImpactPoint;
}

void APolyWarPlayerController::StartOrder(EOrderType Order, FVector OrderPos)
{
	TArray<APolyWarAICharacter*> TeamArray;
	GetClickedTeam(TeamArray);

	if(HasAuthority())
	{
		for(auto TeamAI : TeamArray)
		{
			if(!TeamAI) continue;

			switch (Order)
			{
			case EOrderType::EOD_Move :
			case EOrderType::EOD_Attack :
			case EOrderType::EOD_Rush :
				TeamAI->StartOrder(Order, OrderPos);
				break;
			case EOrderType::EOD_Hold :
			case EOrderType::EOD_Stop :
				TeamAI->StartOrder(Order, TeamAI->GetActorLocation());
				break;
			case EOrderType::EOD_Cancel :
				break;
			}
		}
	}
	else
	{
		ServerStartOrder(Order, OrderPos, TeamArray);
	}

	ResetMapButtons();
}

void APolyWarPlayerController::ServerStartOrder_Implementation(EOrderType Order, FVector_NetQuantize OrderPos, const TArray<APolyWarAICharacter*>& TeamArray)
{
	for(auto TeamAI : TeamArray)
	{
		if(!TeamAI) continue;

		switch (Order)
		{
		case EOrderType::EOD_Move :
		case EOrderType::EOD_Attack :
		case EOrderType::EOD_Rush :
			TeamAI->StartOrder(Order, OrderPos);
			break;
		case EOrderType::EOD_Hold :
		case EOrderType::EOD_Stop :
			TeamAI->StartOrder(Order, TeamAI->GetActorLocation());
			break;
		case EOrderType::EOD_Cancel :
			break;
		}
	}
}

void APolyWarPlayerController::GetClickedTeam(TArray<APolyWarAICharacter*>& OutTeamArray)
{
	PolyWarPlayerCharacter = PolyWarPlayerCharacter == nullptr ? Cast<APolyWarPlayerCharacter>(GetPawn()) : PolyWarPlayerCharacter;
	PolyWarGameState = PolyWarGameState == nullptr ? Cast<APolyWarGameStateBase>(UGameplayStatics::GetGameState(GetWorld())) : PolyWarGameState;
	if(!PolyWarPlayerCharacter || !PolyWarGameState) return;

	TArray<EUnitNum> UnitNums;
	GetMapUnitStateArray(EMapUnitState::EMUS_Clicked, UnitNums);
	PolyWarGameState->GetTeamByUnitNums(PolyWarPlayerCharacter->GetTeamType(), UnitNums, OutTeamArray);
}

FText APolyWarPlayerController::FloatToTimeText(float TimeSeconds)
{
	if(TimeSeconds < 0.0f)
	{
		return FText::FromString("00 : 00");
	}

	int32 CeilTimeSeconds = FMath::CeilToInt32(TimeSeconds);
	int32 Minutes = CeilTimeSeconds / 60;
	int32 Seconds = CeilTimeSeconds % 60;

	FString TimeString;
	TimeString.Append(FString::FromInt(Minutes)).Append(" : ");
	if(Seconds >= 10)
	{
		TimeString.Append(FString::FromInt(Seconds));
	}
	else
	{
		TimeString.Append("0").Append(FString::FromInt(Seconds));
	}

	return FText::FromString(TimeString);
}

void APolyWarPlayerController::ToggleUnitNum(EUnitNum UnitNum)
{
	PolyWarHUD = PolyWarHUD == nullptr ? Cast<APolyWarHUD>(GetHUD()) : PolyWarHUD;
	if(!PolyWarHUD) return;

	if(!PolyWarHUD->MapWidget || !PolyWarHUD->MapWidget->UnitNumMapButtons[UnitNum]) return;
	if(!PolyWarHUD->IsCurrentWidget(PolyWarHUD->MapWidget)) return;

	PolyWarHUD->MapWidget->UnitNumMapButtons[UnitNum]->OnUnitButtonClicked.Broadcast(UnitNum);
}

void APolyWarPlayerController::ToggleOrder(EOrderType Order)
{
	PolyWarHUD = PolyWarHUD == nullptr ? Cast<APolyWarHUD>(GetHUD()) : PolyWarHUD;
	if(!PolyWarHUD) return;

	if(!PolyWarHUD->MapWidget || !PolyWarHUD->MapWidget->OrderMapButtons[Order]) return;
	if(!PolyWarHUD->IsCurrentWidget(PolyWarHUD->MapWidget)) return;

	PolyWarHUD->MapWidget->OrderMapButtons[Order]->OnOrderButtonClicked.Broadcast(Order);
}

void APolyWarPlayerController::ToggleMenu(const FInputActionValue& Value)
{
	PolyWarHUD = PolyWarHUD == nullptr ? Cast<APolyWarHUD>(GetHUD()) : PolyWarHUD;
	PolyWarPlayerCharacter = PolyWarPlayerCharacter == nullptr ? Cast<APolyWarPlayerCharacter>(GetPawn()) : PolyWarPlayerCharacter;
	if(!PolyWarHUD || !PolyWarHUD->MenuWidget || !PolyWarPlayerCharacter) return;

	// Menu Close
	if(PolyWarHUD->IsCurrentWidget(PolyWarHUD->MenuWidget))
	{
		PolyWarHUD->SetDefaultScreenWidget();

		SetInputDefault(false);
	}
	// Menu Open
	else
	{
		PolyWarHUD->ChangeWidget(PolyWarHUD->MenuWidget);

		SetInputDefault(true);
	}
}

void APolyWarPlayerController::ToggleUnitNum1(const FInputActionValue& Value)
{
	ToggleUnitNum(EUnitNum::EUN_Unit1);
}

void APolyWarPlayerController::ToggleUnitNum2(const FInputActionValue& Value)
{
	ToggleUnitNum(EUnitNum::EUN_Unit2);
}

void APolyWarPlayerController::ToggleUnitNum3(const FInputActionValue& Value)
{
	ToggleUnitNum(EUnitNum::EUN_Unit3);
}

void APolyWarPlayerController::ToggleUnitNum4(const FInputActionValue& Value)
{
	ToggleUnitNum(EUnitNum::EUN_Unit4);
}

void APolyWarPlayerController::ToggleUnitNum5(const FInputActionValue& Value)
{
	ToggleUnitNum(EUnitNum::EUN_Unit5);
}

void APolyWarPlayerController::ToggleUnitNum6(const FInputActionValue& Value)
{
	ToggleUnitNum(EUnitNum::EUN_Unit6);
}

void APolyWarPlayerController::ToggleUnitNum7(const FInputActionValue& Value)
{
	ToggleUnitNum(EUnitNum::EUN_Unit7);
}

void APolyWarPlayerController::ToggleUnitNum8(const FInputActionValue& Value)
{
	ToggleUnitNum(EUnitNum::EUN_Unit8);
}

void APolyWarPlayerController::ToggleOrderAttack(const FInputActionValue& Value)
{
	ToggleOrder(EOrderType::EOD_Attack);
}

void APolyWarPlayerController::ToggleOrderStop(const FInputActionValue& Value)
{
	ToggleOrder(EOrderType::EOD_Stop);
}

void APolyWarPlayerController::ToggleOrderHold(const FInputActionValue& Value)
{
	ToggleOrder(EOrderType::EOD_Hold);
}

void APolyWarPlayerController::ToggleOrderRush(const FInputActionValue& Value)
{
	ToggleOrder(EOrderType::EOD_Rush);
}

void APolyWarPlayerController::ToggleOrderMove(const FInputActionValue& Value)
{
	ToggleOrder(EOrderType::EOD_Move);
}

void APolyWarPlayerController::ToggleOrderCancel(const FInputActionValue& Value)
{
	ToggleOrder(EOrderType::EOD_Cancel);
}
