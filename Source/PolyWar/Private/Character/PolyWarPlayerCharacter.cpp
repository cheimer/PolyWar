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
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"
#include "PolyWarComponent/SpellComponent.h"
#include "Weapon/Spell.h"
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
	MapSpringArm->TargetArmLength = 0.0f;
	MapSpringArm->bUsePawnControlRotation = false;
	MapSpringArm->SetWorldRotation(FRotator(-90.0f, 0.0f, 0.0f));
	MapSpringArm->bDoCollisionTest = false;
	MapSpringArm->SetIsReplicated(true);

	MapCamera = CreateDefaultSubobject<USceneCaptureComponent2D>("MapCamera");
	MapCamera->SetupAttachment(MapSpringArm);

	bUseControllerRotationYaw = false;
	GetCharacterMovement()->SetIsReplicated(true);
	GetCharacterMovement()->bOrientRotationToMovement = true;

	GetCharacterMovement()->GravityScale = 3.0f;
	GetCharacterMovement()->JumpZVelocity = 800.0f;
	GetCharacterMovement()->BrakingFriction = 20.0f;
	GetCharacterMovement()->AirControl = 0.5f;

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

	ResetMapCameraLocation();

}

void APolyWarPlayerCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(APolyWarPlayerCharacter, bFocusOnScreen);
}

void APolyWarPlayerCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if(bFocusOnScreen)
	{
		APlayerController* PlayerController = Cast<APlayerController>(GetInstigatorController());
		if(!PlayerController) return;

		FVector2D ViewportSize;
		if(GEngine && GEngine->GameViewport)
		{
			GEngine->GameViewport->GetViewportSize(ViewportSize);
		}

		FVector2D Center(ViewportSize.X / 2, ViewportSize.Y / 2);
		FVector CenterWorldPosition;
		FVector CenterWorldDirection;
		bool bScreenToWorld = UGameplayStatics::DeprojectScreenToWorld(
			PlayerController, Center, CenterWorldPosition, CenterWorldDirection);
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
		if(InputMapClick)
		{
			EnhancedInputComponent->BindAction(InputMapClick, ETriggerEvent::Triggered, this, &ThisClass::MapClick);
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

void APolyWarPlayerCharacter::LookUpRight(const FInputActionValue& Value)
{
	if(bIsOpenMap) return;

	const FVector2D MoveValue = Value.Get<FVector2D>();

	AddControllerYawInput(MoveValue.X);
	AddControllerPitchInput(MoveValue.Y);
}

void APolyWarPlayerCharacter::MoveForwardRight(const FInputActionValue& Value)
{
	if(!GetController()) return;
	if(bIsOpenMap || GetIsAttacking()) return;

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
	if(bIsOpenMap || GetIsAttacking()) return;
	Super::Jump();
}

void APolyWarPlayerCharacter::LeftMousePressedAndReleased(const FInputActionValue& Value)
{
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
		GetCharacterMovement()->MaxWalkSpeed = RunSpeed;
		bIsRunning = true;
	}
	else
	{
		GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
		bIsRunning = false;
	}
}

void APolyWarPlayerCharacter::ServerRun_Implementation(bool InIsRun)
{
	if(InIsRun)
	{
		GetCharacterMovement()->MaxWalkSpeed = RunSpeed;
		bIsRunning = true;
	}
	else
	{
		GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
		bIsRunning = false;
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

void APolyWarPlayerCharacter::MapClick(const FInputActionValue& Value)
{
	if(!bIsOpenMap || !MapSpringArm || !IsLocallyControlled()) return;
	bHoldMouseClick = FMath::IsNearlyEqual(Value.Get<float>(), 1.0f);
}

void APolyWarPlayerCharacter::MapMove(const FInputActionValue& Value)
{
	if(!bIsOpenMap || !MapSpringArm || !bHoldMouseClick) return;
	const FVector2D MoveValue = Value.Get<FVector2D>();
	const FVector NewLocation(
		MapSpringArm->GetComponentLocation().X + MoveValue.Y * MapMoveSensitive,
		MapSpringArm->GetComponentLocation().Y + (-MoveValue.X * MapMoveSensitive),
		MapSpringArm->GetComponentLocation().Z);

	MapSpringArm->SetWorldLocation(NewLocation);

}

void APolyWarPlayerCharacter::MapScroll(const FInputActionValue& Value)
{
	if(!bIsOpenMap || !MapSpringArm) return;
	const bool bScrollUp = FMath::IsNearlyEqual(Value.Get<float>(), 1.0f);

	if(bScrollUp)
	{
		if(MapSpringArm->GetRelativeLocation().Z <= MapHeightMinLimit)
		{
			return;
		}
		MapSpringArm->AddRelativeLocation(FVector(0.0f, 0.0f, -MapMoveSensitive));
	}
	else
	{
		if(MapSpringArm->GetRelativeLocation().Z >= MapHeightMaxLimit)
		{
			return;
		}
		MapSpringArm->AddRelativeLocation(FVector(0.0f, 0.0f, MapMoveSensitive));
	}
}

void APolyWarPlayerCharacter::ResetMapCameraLocation()
{
	if(!MapSpringArm) return;
	MapSpringArm->SetRelativeLocation(FVector(0.0f, 0.0f, MapDefaultHeight));
}

FVector APolyWarPlayerCharacter::GetMapCameraPos()
{
	if(!MapSpringArm) return FVector();
	return MapSpringArm->GetComponentLocation();
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
	if(!SpellComponent || !SpellComponent->GetSpellFirstClass()) return;

	SpellAttack(SpellComponent->GetSpellFirstClass());
}

void APolyWarPlayerCharacter::SpellSecond(const FInputActionValue& Value)
{
	if(!SpellComponent || !SpellComponent->GetSpellSecondClass()) return;

	SpellAttack(SpellComponent->GetSpellSecondClass());
}

void APolyWarPlayerCharacter::SpellUlt(const FInputActionValue& Value)
{
	if(!SpellComponent || !SpellComponent->GetSpellUltClass()) return;

	SpellAttack(SpellComponent->GetSpellUltClass());
}
