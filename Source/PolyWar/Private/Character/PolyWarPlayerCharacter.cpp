// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/PolyWarPlayerCharacter.h"

#include "EnhancedInputComponent.h"
#include "InputMappingContext.h"
#include "InputActionValue.h"
#include "EnhancedInputSubsystems.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"

APolyWarPlayerCharacter::APolyWarPlayerCharacter()
{
	SpringArm = CreateDefaultSubobject<USpringArmComponent>("SpringArm");
	SpringArm->SetupAttachment(GetMesh());
	SpringArm->TargetArmLength = 400.0f;
	SpringArm->bUsePawnControlRotation = true;
	SpringArm->SocketOffset.Y = 75.0f;
	SpringArm->SocketOffset.Z = 75.0f;

	Camera = CreateDefaultSubobject<UCameraComponent>("Camera");
	Camera->SetupAttachment(SpringArm);
	Camera->bUsePawnControlRotation = false;
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);

	bUseControllerRotationYaw = false;
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

void APolyWarPlayerCharacter::Run(const FInputActionValue& Value)
{
	bool IsTriggering = FMath::IsNearlyEqual(Value.Get<float>(), 1.0f);
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
