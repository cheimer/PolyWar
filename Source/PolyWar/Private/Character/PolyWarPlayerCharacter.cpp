// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/PolyWarPlayerCharacter.h"

#include "EnhancedInputComponent.h"
#include "InputMappingContext.h"
#include "InputActionValue.h"
#include "EnhancedInputSubsystems.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SceneCaptureComponent2D.h"
#include "Controller/PolyWarPlayerController.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameMode/PolyWarGameModeBase.h"
#include "GameState/PolyWarGameStateBase.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"
#include "Perception/AIPerceptionStimuliSourceComponent.h"
#include "Perception/AISense_Sight.h"
#include "PolyWarComponent/CombatComponent.h"
#include "PolyWarComponent/SpellComponent.h"
#include "Spell/Spell.h"
#include "Weapon/Weapon.h"

APolyWarPlayerCharacter::APolyWarPlayerCharacter()
{
	CharacterSpringArm = CreateDefaultSubobject<USpringArmComponent>("CharacterSpringArm");
	CharacterSpringArm->SetupAttachment(GetMesh());
	CharacterSpringArm->TargetArmLength = 400.0f;
	CharacterSpringArm->bUsePawnControlRotation = true;
	CharacterSpringArm->bDoCollisionTest = false;
	CharacterSpringArm->SocketOffset.Y = 75.0f;
	CharacterSpringArm->SocketOffset.Z = 75.0f;
	CharacterSpringArm->SetRelativeLocation(FVector(0.0f, 0.0f, 90.0f));

	MainCamera = CreateDefaultSubobject<UCameraComponent>("MainCamera");
	MainCamera->SetupAttachment(CharacterSpringArm);
	MainCamera->bUsePawnControlRotation = false;
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
	GetMesh()->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);

	MapSpringArm = CreateDefaultSubobject<USpringArmComponent>("MapSpringArm");
	MapSpringArm->SetupAttachment(GetMesh());
	MapSpringArm->TargetArmLength = MapDefaultHeight;
	MapSpringArm->bUsePawnControlRotation = false;
	MapSpringArm->SetRelativeLocation(FVector(0.0f, 0.0f, 0.0f));
	MapSpringArm->SetWorldRotation(FRotator(-75.0f, 0.0f, 0.0f));
	MapSpringArm->bDoCollisionTest = false;

	MapCamera = CreateDefaultSubobject<USceneCaptureComponent2D>("MapCamera");
	MapCamera->SetupAttachment(MapSpringArm);

	bUseControllerRotationYaw = false;
	GetCharacterMovement()->SetIsReplicated(true);
	GetCharacterMovement()->bOrientRotationToMovement = true;

	GetCharacterMovement()->GravityScale = 3.0f;
	GetCharacterMovement()->JumpZVelocity = 800.0f;
	GetCharacterMovement()->BrakingFriction = 20.0f;
	GetCharacterMovement()->AirControl = 0.5f;

	TeamType = ETeamType::ET_NoTeam;
}

void APolyWarPlayerCharacter::BeginPlay()
{
	Super::BeginPlay();

	// Client Input Setting
	if(!HasAuthority())
	{
		if(const APlayerController* PlayerController = Cast<APlayerController>(GetController()))
		{
			if(UEnhancedInputLocalPlayerSubsystem* Subsystem =
				ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
			{
				Subsystem->AddMappingContext(PlayerInputMapping, 0);
			}
		}
	}

	SetPlayerTeam();

	APolyWarGameStateBase* PolyWarGameState = Cast<APolyWarGameStateBase>(UGameplayStatics::GetGameState(GetWorld()));
	if(PolyWarGameState)
	{
		PolyWarGameState->RegisterPlayer(this);
	}
}

void APolyWarPlayerCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	// Server Input Setting
	if(HasAuthority())
	{
		if(const APlayerController* PlayerController = Cast<APlayerController>(GetController()))
		{
			if(UEnhancedInputLocalPlayerSubsystem* Subsystem =
				ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
			{
				Subsystem->AddMappingContext(PlayerInputMapping, 0);
			}
		}
	}

}

void APolyWarPlayerCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	ResetMapCamera();

}

void APolyWarPlayerCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(APolyWarPlayerCharacter, bFocusOnScreen);
	DOREPLIFETIME(APolyWarPlayerCharacter, TeamType);
}

void APolyWarPlayerCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if(bFocusOnScreen)
	{
		FVector CenterWorldPosition;
		FVector CenterWorldDirection;
		bool bScreenToWorld = GetViewportCenter(CenterWorldPosition, CenterWorldDirection);
		if(!bScreenToWorld) return;

		FRotator Rotator = CenterWorldDirection.Rotation();
		FRotator RotateDirection = FRotator(0.0f, Rotator.Yaw, 0.0f);


		SetActorRotation(RotateDirection);
		if(!HasAuthority())
		{
			ServerSetActorRotation(RotateDirection);
		}
	}
}

void APolyWarPlayerCharacter::ServerSetActorRotation_Implementation(const FRotator& RotateDirection)
{
	SetActorRotation(RotateDirection);
}

void APolyWarPlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if(UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		if(InputLook)
		{
			EnhancedInputComponent->BindAction(InputLook, ETriggerEvent::Triggered, this, &ThisClass::LookUpRight);
		}
		if(InputMove)
		{
			EnhancedInputComponent->BindAction(InputMove, ETriggerEvent::Triggered, this, &ThisClass::MoveForwardRight);
		}
		if(InputLeftMouse)
		{
			EnhancedInputComponent->BindAction(InputLeftMouse, ETriggerEvent::Triggered, this, &ThisClass::LeftMousePressedAndReleased);
		}
		if(InputJump)
		{
			EnhancedInputComponent->BindAction(InputJump, ETriggerEvent::Triggered, this, &ThisClass::Jump);
		}
		if(InputRun)
		{
			EnhancedInputComponent->BindAction(InputRun, ETriggerEvent::Triggered, this, &ThisClass::Run);
		}
		if(InputMap)
		{
			EnhancedInputComponent->BindAction(InputMap, ETriggerEvent::Triggered, this, &ThisClass::Map);
		}
		if(InputMapLeftClick)
		{
			EnhancedInputComponent->BindAction(InputMapLeftClick, ETriggerEvent::Triggered, this, &ThisClass::MapLeftClick);
		}
		if(InputMapRightClick)
		{
			EnhancedInputComponent->BindAction(InputMapRightClick, ETriggerEvent::Triggered, this, &ThisClass::MapRightClick);
		}
		if(InputMapMove)
		{
			EnhancedInputComponent->BindAction(InputMapMove, ETriggerEvent::Triggered, this, &ThisClass::MapMove);
		}
		if(InputMapScroll)
		{
			EnhancedInputComponent->BindAction(InputMapScroll, ETriggerEvent::Triggered, this, &ThisClass::MapScroll);
		}
		if(InputWeaponSkillFirst)
		{
			EnhancedInputComponent->BindAction(InputWeaponSkillFirst, ETriggerEvent::Triggered, this, &ThisClass::WeaponSkillFirst);
		}
		if(InputWeaponSkillSecond)
		{
			EnhancedInputComponent->BindAction(InputWeaponSkillSecond, ETriggerEvent::Triggered, this, &ThisClass::WeaponSkillSecond);
		}
		if(InputSpellFirst)
		{
			EnhancedInputComponent->BindAction(InputSpellFirst, ETriggerEvent::Triggered, this, &ThisClass::SpellFirst);
		}
		if(InputSpellSecond)
		{
			EnhancedInputComponent->BindAction(InputSpellSecond, ETriggerEvent::Triggered, this, &ThisClass::SpellSecond);
		}
		if(InputSpellUlt)
		{
			EnhancedInputComponent->BindAction(InputSpellUlt, ETriggerEvent::Triggered, this, &ThisClass::SpellUlt);
		}
	}
}

void APolyWarPlayerCharacter::SetPlayerTeam()
{
	APolyWarGameStateBase* GameState = Cast<APolyWarGameStateBase>(UGameplayStatics::GetGameState(GetWorld()));
	if(!GameState) return;

	if(HasAuthority())
	{
		if(!GameState->IsTeamExistPlayer(ETeamType::ET_BlueTeam))
		{
			SetTeamType(ETeamType::ET_BlueTeam);
		}
		else if(!GameState->IsTeamExistPlayer(ETeamType::ET_RedTeam))
		{
			SetTeamType(ETeamType::ET_RedTeam);
		}
	}
}

void APolyWarPlayerCharacter::SetPlayerDeath()
{
	Super::SetPlayerDeath();

	if(HasAuthority())
	{
		APolyWarPlayerCharacter* SpectatorPawn = GetWorld()->SpawnActor<APolyWarPlayerCharacter>(SpectatorClass, GetActorTransform());
		if(SpectatorPawn)
		{
			SpectatorPawn->SpectatorSettings(this);
			GetController()->Possess(SpectatorPawn);
		}
	}
}

void APolyWarPlayerCharacter::LookUpRight(const FInputActionValue& Value)
{
	if(bIsOpenUI) return;

	const FVector2D MoveValue = Value.Get<FVector2D>();

	AddControllerYawInput(MoveValue.X);
	AddControllerPitchInput(MoveValue.Y);
}

void APolyWarPlayerCharacter::MoveForwardRight(const FInputActionValue& Value)
{
	if(!GetController()) return;
	if(bIsOpenUI || GetIsAttacking()) return;

	const FVector2D MoveValue = Value.Get<FVector2D>();
	const FRotator YawRotation(0.0f, GetController()->GetControlRotation().Yaw, 0.0f);

	if(MoveValue.X != 0.0f)
	{
		const FVector Direction(FRotationMatrix(YawRotation).GetUnitAxis((EAxis::X)));
		AddMovementInput(Direction, MoveValue.X);
	}
	if(MoveValue.Y != 0.0f)
	{
		const FVector Direction(FRotationMatrix(YawRotation).GetUnitAxis((EAxis::Y)));
		AddMovementInput(Direction, MoveValue.Y);
	}
}

void APolyWarPlayerCharacter::Jump()
{
	if(bIsOpenUI || GetIsAttacking()) return;
	Super::Jump();
}

void APolyWarPlayerCharacter::LeftMousePressedAndReleased(const FInputActionValue& Value)
{
	if(GetIsAttacking()) return;
	WeaponAttack();
}

void APolyWarPlayerCharacter::Run(const FInputActionValue& Value)
{
	bool IsTriggering = FMath::IsNearlyEqual(Value.Get<float>(), 1.0f);
	if(!HasAuthority())
	{
		ServerRun(IsTriggering);
	}
	if(IsTriggering)
	{
		GetCharacterMovement()->MaxWalkSpeed = CurrentRunSpeed;
		bIsRunning = true;
	}
	else
	{
		GetCharacterMovement()->MaxWalkSpeed = CurrentWalkSpeed;
		bIsRunning = false;
	}
}

void APolyWarPlayerCharacter::ServerRun_Implementation(bool InIsRun)
{
	if(InIsRun)
	{
		GetCharacterMovement()->MaxWalkSpeed = CurrentRunSpeed;
		bIsRunning = true;
	}
	else
	{
		GetCharacterMovement()->MaxWalkSpeed = CurrentWalkSpeed;
		bIsRunning = false;
	}
}

void APolyWarPlayerCharacter::SpectatorSettings(APolyWarPlayerCharacter* BeforeCharacter)
{
	if(AIPerceptionSourceComponent)
	{
		AIPerceptionSourceComponent->UnregisterFromSense(UAISense_Sight::StaticClass());
		AIPerceptionSourceComponent->UnregisterFromPerceptionSystem();
	}

	SetTeamType(BeforeCharacter->GetTeamType());
	DefaultWalkSpeed = BeforeCharacter->GetDefaultWalkSpeed();
	DefaultRunSpeed = BeforeCharacter->GetDefaultRunSpeed();

	if(GetMesh())
	{
		GetMesh()->SetOnlyOwnerSee(true);
		GetMesh()->SetCollisionResponseToAllChannels(ECR_Ignore);
	}
	if(GetCapsuleComponent())
	{
		GetCapsuleComponent()->SetGenerateOverlapEvents(false);
		GetCapsuleComponent()->SetCollisionResponseToAllChannels(ECR_Ignore);
		GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Block);
		GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_WorldDynamic, ECR_Block);
	}

}

void APolyWarPlayerCharacter::Map(const FInputActionValue& Value)
{
	PolyWarPlayerController = PolyWarPlayerController == nullptr ? Cast<APolyWarPlayerController>(GetController()) : PolyWarPlayerController;
	if(!PolyWarPlayerController) return;

	if(MapCamera && !MapCamera->TextureTarget)
	{
		if(HasAuthority() && IsLocallyControlled())
		{
			MapCamera->TextureTarget = CanvasRenderTarget1;
		}
		else if (!HasAuthority() && IsLocallyControlled())
		{
			MapCamera->TextureTarget = CanvasRenderTarget2;
			ServerSetMapCamera();
		}
	}

	PolyWarPlayerController->MapToggle();
}

void APolyWarPlayerCharacter::ServerSetMapCamera_Implementation()
{
	MapCamera->TextureTarget = CanvasRenderTarget2;
}

void APolyWarPlayerCharacter::MapLeftClick(const FInputActionValue& Value)
{
	if(!bIsOpenUI || !MapSpringArm || !IsLocallyControlled()) return;
	bHoldLeftMouseClick = FMath::IsNearlyEqual(Value.Get<float>(), 1.0f);
}

void APolyWarPlayerCharacter::MapRightClick(const FInputActionValue& Value)
{
	if(!bIsOpenUI || !MapSpringArm || !IsLocallyControlled()) return;
	bHoldRightMouseClick = FMath::IsNearlyEqual(Value.Get<float>(), 1.0f);
}

void APolyWarPlayerCharacter::MapMove(const FInputActionValue& Value)
{
	if(!bIsOpenUI || !MapSpringArm) return;

	// Map move
	if(bHoldRightMouseClick)
	{
		const FVector2D MoveValue = Value.Get<FVector2D>();
		const FVector ForwardDirection = MapSpringArm->GetForwardVector().GetSafeNormal2D();
		const FVector RightDirection = MapSpringArm->GetRightVector().GetSafeNormal2D();

		FVector MapLocation = MapSpringArm->GetComponentLocation();
		FVector Destination = MapLocation + MoveValue.Y * ForwardDirection * MapMoveSensitive;
		Destination += -MoveValue.X * RightDirection * MapMoveSensitive;

		MapSpringArm->SetWorldLocation(Destination);
	}

	// Map rotate
	if(bHoldLeftMouseClick)
	{
		const FVector2D MoveValue = Value.Get<FVector2D>();
		const FRotator NewRotator(
			MapSpringArm->GetComponentRotation().Pitch, // + (MoveValue.Y * MapRotateSensitive),
			MapSpringArm->GetComponentRotation().Yaw + (MoveValue.X * MapRotateSensitive),
			MapSpringArm->GetComponentRotation().Roll);

		MapSpringArm->SetWorldRotation(NewRotator);
	}

}

void APolyWarPlayerCharacter::MapScroll(const FInputActionValue& Value)
{
	if(!bIsOpenUI || !MapSpringArm) return;
	const bool bScrollUp = FMath::IsNearlyEqual(Value.Get<float>(), 1.0f);

	if(bScrollUp)
	{
		if(MapSpringArm->TargetArmLength <= MapHeightMinLimit)
		{
			return;
		}
		MapSpringArm->TargetArmLength -= MapScrollSensitive;
	}
	else
	{
		if(MapSpringArm->TargetArmLength >= MapHeightMaxLimit)
		{
			return;
		}
		MapSpringArm->TargetArmLength += MapScrollSensitive;
	}
}

void APolyWarPlayerCharacter::ResetMapCamera()
{
	if(!MapSpringArm) return;
	MapSpringArm->SetRelativeLocation(FVector(0.0f, 0.0f, 0.0f));
	MapSpringArm->TargetArmLength = MapDefaultHeight;
	MapSpringArm->SetWorldRotation(FRotator(-75.0f, 0.0f, 0.0f));
}

FTransform APolyWarPlayerCharacter::GetMainCameraTransform()
{
	if(!MainCamera) return FTransform();
	return MainCamera->GetComponentTransform();
}

FVector APolyWarPlayerCharacter::GetMapCameraPos()
{
	if(!MapCamera) return FVector();
	return MapCamera->GetComponentLocation();
}

FVector APolyWarPlayerCharacter::GetMapCameraForward()
{
	if(!MapCamera) return FVector();
	return MapCamera->GetForwardVector().GetSafeNormal();
}

FVector APolyWarPlayerCharacter::GetMapCameraRight()
{
	if(!MapCamera) return FVector();
	return MapCamera->GetRightVector().GetSafeNormal();
}

FVector APolyWarPlayerCharacter::GetMapCameraUp()
{
	if(!MapCamera) return FVector();
	return MapCamera->GetUpVector().GetSafeNormal();
}

EWeaponSkill APolyWarPlayerCharacter::GetWeaponSkillFirst()
{
	if(!GetEquippedWeapon()) return EWeaponSkill::EWS_MAX;

	return GetEquippedWeapon()->GetWeaponSkillFirst();
}

EWeaponSkill APolyWarPlayerCharacter::GetWeaponSkillSecond()
{
	if(!GetEquippedWeapon()) return EWeaponSkill::EWS_MAX;

	return GetEquippedWeapon()->GetWeaponSkillSecond();
}

float APolyWarPlayerCharacter::GetWeaponSkillCoolDown(EWeaponSkill WeaponSkill)
{
	if(!GetEquippedWeapon()) return 0.0f;

	return GetEquippedWeapon()->GetWeaponSkillCoolDown(WeaponSkill);
}

float APolyWarPlayerCharacter::GetWeaponSkillRemainCoolDown(EWeaponSkill WeaponSkill)
{
	if(!GetEquippedWeapon()) return 0.0f;

	return GetEquippedWeapon()->GetWeaponSkillRemainCoolDown(WeaponSkill);
}

TSubclassOf<ASpell> APolyWarPlayerCharacter::GetSpellFirst()
{
	if(!SpellComponent) return nullptr;

	return SpellComponent->GetSpellFirstClass();
}

TSubclassOf<ASpell> APolyWarPlayerCharacter::GetSpellSecond()
{
	if(!SpellComponent) return nullptr;

	return SpellComponent->GetSpellSecondClass();
}

TSubclassOf<ASpell> APolyWarPlayerCharacter::GetSpellUlt()
{
	if(!SpellComponent) return nullptr;

	return SpellComponent->GetSpellUltClass();
}

float APolyWarPlayerCharacter::GetSpellCoolDown(TSubclassOf<ASpell> Spell)
{
	if(!SpellComponent) return 0.0f;

	return SpellComponent->GetSpellCoolDown(Spell);
}

float APolyWarPlayerCharacter::GetSpellRemainCoolDown(TSubclassOf<ASpell> Spell)
{
	if(!SpellComponent) return 0.0f;

	return SpellComponent->GetSpellRemainCoolDown(Spell);
}

void APolyWarPlayerCharacter::WeaponSkillFirst(const FInputActionValue& Value)
{
	if(!GetEquippedWeapon()) return;
	WeaponSkillAttack(GetEquippedWeapon()->GetWeaponSkillFirst());
}

void APolyWarPlayerCharacter::WeaponSkillSecond(const FInputActionValue& Value)
{
	if(!GetEquippedWeapon()) return;
	WeaponSkillAttack(GetEquippedWeapon()->GetWeaponSkillSecond());
}

void APolyWarPlayerCharacter::SpellFirst(const FInputActionValue& Value)
{
	if(!CombatComponent || !CombatComponent->GetSpellComponent() || !CombatComponent->GetSpellComponent()->GetSpellFirstClass()) return;

	SpellAttack(CombatComponent->GetSpellComponent()->GetSpellFirstClass());
}

void APolyWarPlayerCharacter::SpellSecond(const FInputActionValue& Value)
{
	if(!CombatComponent || !CombatComponent->GetSpellComponent() || !CombatComponent->GetSpellComponent()->GetSpellSecondClass()) return;

	SpellAttack(CombatComponent->GetSpellComponent()->GetSpellSecondClass());
}

void APolyWarPlayerCharacter::SpellUlt(const FInputActionValue& Value)
{
	if(!CombatComponent || !CombatComponent->GetSpellComponent() || !CombatComponent->GetSpellComponent()->GetSpellUltClass()) return;

	SpellAttack(CombatComponent->GetSpellComponent()->GetSpellUltClass());
}
