// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/PolyWarPlayerCharacter.h"

#include "EnhancedInputComponent.h"
#include "InputMappingContext.h"
#include "InputActionValue.h"
#include "EnhancedInputSubsystems.h"

void APolyWarPlayerCharacter::BeginPlay()
{
	Super::BeginPlay();

	// Client Input Setting
	/*
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
	*/
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
	}
}

void APolyWarPlayerCharacter::LookUpRight(const FInputActionValue& Value)
{
	const FVector2D MoveValue = Value.Get<FVector2D>();

	AddControllerYawInput(MoveValue.X);
	AddControllerPitchInput(MoveValue.Y);
}

void APolyWarPlayerCharacter::MoveForwardRight(const FInputActionValue& Value)
{
	if(!GetController()) return;

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
	Super::Jump();
}

void APolyWarPlayerCharacter::LeftMousePressedAndReleased(const FInputActionValue& Value)
{

}
