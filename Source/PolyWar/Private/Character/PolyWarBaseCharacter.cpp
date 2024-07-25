// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/PolyWarBaseCharacter.h"

#include "Components/CapsuleComponent.h"
#include "Components/SphereComponent.h"
#include "Engine/SkeletalMeshSocket.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameState/PolyWarGameStateBase.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMaterialLibrary.h"
#include "Kismet/KismetRenderingLibrary.h"
#include "Net/UnrealNetwork.h"
#include "Perception/AIPerceptionStimuliSourceComponent.h"
#include "Perception/AISense_Sight.h"
#include "PolyWar/PolyWar.h"
#include "PolyWarComponent/CombatComponent.h"
#include "PolyWarComponent/StateComponent.h"
#include "PolyWarComponent/SpellComponent.h"
#include "Weapon/Weapon.h"

APolyWarBaseCharacter::APolyWarBaseCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	SetReplicates(true);
	AActor::SetReplicateMovement(true);
	NetUpdateFrequency = 66.0f;
	MinNetUpdateFrequency = 33.0f;

	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
	GetMesh()->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);

	GetCharacterMovement()->MaxWalkSpeed = DefaultWalkSpeed;
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 720.0f, 0.0f);

	CombatComponent = CreateDefaultSubobject<UCombatComponent>("CombatComponent");
	CombatComponent->SetIsReplicated(true);

	StateComponent = CreateDefaultSubobject<UStateComponent>("StateComponent");
	StateComponent->SetIsReplicated(true);

	SpellComponent = CreateDefaultSubobject<USpellComponent>("SpellComponent");
	SpellComponent->SetIsReplicated(true);

	AIPerceptionSourceComponent = CreateDefaultSubobject<UAIPerceptionStimuliSourceComponent>("AIPerceptionSourceComponent");
	AIPerceptionSourceComponent->RegisterForSense(UAISense_Sight::StaticClass());
	AIPerceptionSourceComponent->RegisterWithPerceptionSystem();

	VisibleSphere = CreateDefaultSubobject<USphereComponent>("VisibleSphere");
	VisibleSphere->SetupAttachment(GetRootComponent());
	VisibleSphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	VisibleSphere->SetCollisionResponseToAllChannels(ECR_Ignore);
	VisibleSphere->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);

}

void APolyWarBaseCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	if(CombatComponent)
	{
		CombatComponent->SetOwnerCharacter(this);
	}
	if(SpellComponent)
	{
		SpellComponent->SetOwnerCharacter(this);
	}
	if(CombatComponent && SpellComponent)
	{
		CombatComponent->SetSpellComponent(SpellComponent);
	}
	if(StateComponent)
	{
		StateComponent->SetOwnerCharacter(this);
	}
}

void APolyWarBaseCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(APolyWarBaseCharacter, bIsRunning);
	DOREPLIFETIME(APolyWarBaseCharacter, TeamType);
}

void APolyWarBaseCharacter::BeginPlay()
{
	Super::BeginPlay();

	if(VisibleSphere)
	{
		VisibleSphere->SetComponentTickInterval(0.1f);
		VisibleSphere->OnComponentBeginOverlap.AddDynamic(this, &ThisClass::VisibleSphereBeginOverlap);
		VisibleSphere->OnComponentEndOverlap.AddDynamic(this, &ThisClass::VisibleSphereEndOverlap);
	}

	SpawnWeapon(true);

	if(HasAuthority())
	{
		OnTakeAnyDamage.AddDynamic(this, &ThisClass::ReceiveDamage);
	}

	if(StateComponent)
	{
		StateComponent->UpdateHUDHealth();
	}

}

void APolyWarBaseCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	UpdateFog();

	if(LocalPlayerTeam != ETeamType::ET_NoTeam && LocalPlayerTeam != GetTeamType())
	{
		if(DetectedNum > 0)
		{
			GetMesh()->SetVisibility(true, true);
		}
		else
		{
			GetMesh()->SetVisibility(false, true);
		}
	}

}

void APolyWarBaseCharacter::UpdateFog()
{
	if(IsDead()) return;

	FVector StartVec = GetActorLocation() + FVector(0.0f, 0.0f, 500.0f);
	FVector EndVec = GetActorLocation() + FVector(0.0f, 0.0f, -500.0f);

	FHitResult HitResult;
	FCollisionQueryParams CollisionQueryParams = FCollisionQueryParams::DefaultQueryParam;
	CollisionQueryParams.bTraceComplex = true;
	CollisionQueryParams.bReturnFaceIndex  = true;
	GetWorld()->LineTraceSingleByChannel(HitResult, StartVec, EndVec, ECC_Fog, CollisionQueryParams);

	FVector2D UVSpace;
	if(!UGameplayStatics::FindCollisionUV(HitResult, 0, UVSpace))
	{
		UE_LOG(LogTemp, Warning, TEXT("Cannot Find UV"));
		return;
	}
	FLinearColor UVColor;
	UVColor.R = UVSpace.X; UVColor.G = UVSpace.Y, UVColor.B = 0.0f, UVColor.A = 1.0f;

	if(FogOfWarMaterial && FogOfWarRender)
	{
		FogOfWarMaterial->SetVectorParameterValue(FName("Location"), UVColor);
		UKismetRenderingLibrary::DrawMaterialToRenderTarget(this, FogOfWarRender, FogOfWarMaterial);
	}
	if(FogOfWarRevealMaterial && FogOfWarRevealRender)
	{
		FogOfWarRevealMaterial->SetVectorParameterValue(FName("Location"), UVColor);
		UKismetRenderingLibrary::DrawMaterialToRenderTarget(this, FogOfWarRevealRender, FogOfWarRevealMaterial);
	}

	if(FogOfWarRevealRender)
	{
		UKismetRenderingLibrary::ClearRenderTarget2D(this, FogOfWarRevealRender);
	}
}

// Use WorldHideStart
void APolyWarBaseCharacter::CreateFogOfWar( UMaterialInterface* InFogOfWarInterface, UTextureRenderTarget2D* InFogOfWarRender,
	UMaterialInterface* InFogOfWarRevealInterface, UTextureRenderTarget2D* InFogOfWarRevealRender)
{
	if(InFogOfWarInterface)
	{
		FogOfWarMaterial = UKismetMaterialLibrary::CreateDynamicMaterialInstance(this, InFogOfWarInterface);
		if(FogOfWarMaterial)
		{
			FogOfWarMaterial->SetScalarParameterValue(FName("Size"), FogRevealSize * FogSizeRate);
		}
	}
	if(InFogOfWarRender)
	{
		FogOfWarRender = InFogOfWarRender;
	}
	if(InFogOfWarRevealInterface)
	{
		FogOfWarRevealMaterial = UKismetMaterialLibrary::CreateDynamicMaterialInstance(this, InFogOfWarRevealInterface);
		if(FogOfWarRevealMaterial)
		{
			FogOfWarRevealMaterial->SetScalarParameterValue(FName("Size"), FogRevealSize * FogSizeRate);
		}
	}
	if(InFogOfWarRevealRender)
	{
		FogOfWarRevealRender = InFogOfWarRevealRender;
	}
	if(VisibleSphere)
	{
		VisibleSphere->SetSphereRadius(FogRevealSize * 10000.0f * FogSizeRate);
	}
}

// Use WorldRevealStart
void APolyWarBaseCharacter::CreateFogOfWar(UMaterialInterface* InFogOfWarRevealInterface, UTextureRenderTarget2D* InFogOfWarRevealRender)
{
	if(InFogOfWarRevealInterface)
	{
		FogOfWarRevealMaterial = UKismetMaterialLibrary::CreateDynamicMaterialInstance(this, InFogOfWarRevealInterface);
		if(FogOfWarRevealMaterial)
		{
			FogOfWarRevealMaterial->SetScalarParameterValue(FName("Size"), FogRevealSize * FogSizeRate);
		}
	}
	if(InFogOfWarRevealRender)
	{
		FogOfWarRevealRender = InFogOfWarRevealRender;
	}
}

void APolyWarBaseCharacter::ReceiveDamage(AActor* DamagedActor, float Damage,
                                          const UDamageType* DamageType, AController* InstigatedBy, AActor* DamageCauser)
{
	if(!StateComponent) return;

	StateComponent->ReceiveDamage(Damage, DamageCauser);
}

bool APolyWarBaseCharacter::IsDead()
{
	if(!StateComponent) return false;

	return StateComponent->IsDead();
}

void APolyWarBaseCharacter::SetPlayerDeath()
{
	GetCharacterMovement()->StopMovementImmediately();
	GetCharacterMovement()->DisableMovement();
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	if(GetEquippedWeapon())
	{
		GetEquippedWeapon()->SetCollisionEnabled(false);
	}

	PlayDeathAnimMontage(true);

	FTimerHandle DeathTimer;
	GetWorldTimerManager().SetTimer(DeathTimer, this, &ThisClass::DeathTimerFinished, 3.0f);

	OnCharacterDeathDelegate.Broadcast(this);
}

void APolyWarBaseCharacter::DeathTimerFinished()
{
	GetWorldTimerManager().ClearAllTimersForObject(this);

	if(GetEquippedWeapon())
	{
		GetEquippedWeapon()->Destroy();
	}
	Destroy();
}

void APolyWarBaseCharacter::UpdateWalkSpeed()
{
	if(!bIsRunning)
	{
		GetCharacterMovement()->MaxWalkSpeed = CurrentWalkSpeed;
	}
	else
	{
		GetCharacterMovement()->MaxWalkSpeed = CurrentRunSpeed;
	}
}

void APolyWarBaseCharacter::VisibleSphereBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	APolyWarBaseCharacter* OtherCharacter = Cast<APolyWarBaseCharacter>(OtherActor);
	if(!OtherCharacter) return;

	if(LocalPlayerTeam == ETeamType::ET_NoTeam)
	{
		APolyWarGameStateBase* PolyWarGameState = GetWorld()->GetGameState<APolyWarGameStateBase>();
		if(!PolyWarGameState) return;

		LocalPlayerTeam = PolyWarGameState->GetLocalPlayerTeam();

		if(LocalPlayerTeam == ETeamType::ET_NoTeam)
		{
			return;
		}
	}

	if(LocalPlayerTeam == GetTeamType() && OtherCharacter->GetTeamType() != GetTeamType())
	{
		OtherCharacter->DetectedNumAdd();
	}
}

void APolyWarBaseCharacter::VisibleSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	APolyWarBaseCharacter* OtherCharacter = Cast<APolyWarBaseCharacter>(OtherActor);
	if(!OtherCharacter) return;

	if(OtherCharacter->IsDead())
	{
		return;
	}

	if(LocalPlayerTeam == ETeamType::ET_NoTeam)
	{
		APolyWarGameStateBase* PolyWarGameState = GetWorld()->GetGameState<APolyWarGameStateBase>();
		if(!PolyWarGameState) return;

		LocalPlayerTeam = PolyWarGameState->GetLocalPlayerTeam();

		if(LocalPlayerTeam == ETeamType::ET_NoTeam) return;
	}

	if(LocalPlayerTeam == GetTeamType() && OtherCharacter->GetTeamType() != GetTeamType())
	{
		OtherCharacter->DetectedNumMinus();
	}
}

AWeapon* APolyWarBaseCharacter::SpawnWeapon(bool IsAttach)
{
	if(!EquippedWeaponClass || !GetMesh() || !GetWorld() || !HasAuthority()) return nullptr;

	AWeapon* Weapon = GetWorld()->SpawnActor<AWeapon>(EquippedWeaponClass);
	Weapon->SetOwner(this);

	const USkeletalMeshSocket* HandSocket = GetMesh()->GetSocketByName(RightHandSocket);
	if(HandSocket)
	{
		if(IsAttach)
		{
			HandSocket->AttachActor(Weapon, GetMesh());
		}
		else
		{
			Weapon->SetActorLocation(HandSocket->GetSocketLocation(GetMesh()));
		}
	}
	if(Weapon && CombatComponent && !CombatComponent->GetEquippedWeapon())
	{
		CombatComponent->SetEquippedWeapon(Weapon);
	}

	return Weapon;
}

void APolyWarBaseCharacter::ThrowWeapon()
{
	if(!CombatComponent || !IsLocallyControlled()) return;

	FVector CenterWorldPosition;
	FVector CenterWorldDirection;
	bool bScreenToWorld = GetViewportCenter(CenterWorldPosition, CenterWorldDirection);
	if(!bScreenToWorld) return;

	if(HasAuthority())
	{
		AWeapon* SpawnedWeapon = SpawnWeapon(false);
		if(!SpawnedWeapon) return;

		FVector Destination = CenterWorldPosition + CenterWorldDirection * AdjustThrowPosVal;
		FVector WeaponDirection = (Destination - SpawnedWeapon->GetActorLocation()).GetSafeNormal();

		CombatComponent->ThrowWeapon(SpawnedWeapon, WeaponDirection);
	}
	else if (!HasAuthority())
	{
		ServerThrowWeapon(CenterWorldPosition, CenterWorldDirection);
	}

}

void APolyWarBaseCharacter::ServerThrowWeapon_Implementation(const FVector& Position, const FVector& Direction)
{
	if(!CombatComponent) return;

	AWeapon* SpawnedWeapon = SpawnWeapon(false);
	if(!SpawnedWeapon) return;

	FVector Destination = Position + Direction * AdjustThrowPosVal;
	FVector WeaponDirection = (Destination - SpawnedWeapon->GetActorLocation()).GetSafeNormal();

	CombatComponent->ThrowWeapon(SpawnedWeapon, WeaponDirection);
}

void APolyWarBaseCharacter::SpellEffect()
{
	if(!CombatComponent) return;

	CombatComponent->SpellEffect();
}

void APolyWarBaseCharacter::WeaponAttack()
{
	if(!CombatComponent || !CombatComponent->GetEquippedWeapon()) return;
	if(bIsOpenUI || IsDead() || bIsGameEnd) return;

	CombatComponent->BeginWeaponAttack();
}

void APolyWarBaseCharacter::WeaponSkillAttack(EWeaponSkill WeaponSkill)
{
	if(!CombatComponent || !CombatComponent->GetEquippedWeapon()) return;
	if(bIsOpenUI || IsDead() || bIsGameEnd) return;

	CombatComponent->BeginWeaponSkill(WeaponSkill);
}

void APolyWarBaseCharacter::SpellAttack(TSubclassOf<ASpell> Spell)
{
	if(!CombatComponent || !CombatComponent->GetSpellComponent()) return;
	if(bIsOpenUI || IsDead() || bIsGameEnd) return;

	CombatComponent->BeginSpell(Spell);
}

float APolyWarBaseCharacter::GetPowerRate()
{
	if(!StateComponent) return 1.0f;

	return StateComponent->GetPowerRate();
}

float APolyWarBaseCharacter::GetSpellPowerRate()
{
	if(!StateComponent) return 1.0f;

	return StateComponent->GetSpellPowerRate();
}

void APolyWarBaseCharacter::SetApplyDamageRate(float DamageRate)
{
	if(!StateComponent) return;

	StateComponent->SetApplyDamageRate(DamageRate);
}

void APolyWarBaseCharacter::PlayAttackAnimMontage(bool RandPlay, int32 Index)
{
	if(AttackAnimMontages.Num() <= 0) return;

	if(RandPlay)
	{
		const int32 RandNum = FMath::RandRange(0, AttackAnimMontages.Num() - 1);

		if(!AttackAnimMontages[RandNum]) return;
		PlayAnimMontage(AttackAnimMontages[RandNum]);
	}
	else
	{
		if(AttackAnimMontages.Num() <= Index || !AttackAnimMontages[Index]) return;
		PlayAnimMontage(AttackAnimMontages[Index]);
	}
}

void APolyWarBaseCharacter::PlayWeaponSkillAnimMontage(EWeaponSkill WeaponSkill)
{
	if(WeaponSkillAnimMontages.Contains(WeaponSkill))
	{
		PlayAnimMontage(WeaponSkillAnimMontages[WeaponSkill]);
	}
}

void APolyWarBaseCharacter::PlaySpellAnimMontage(UAnimMontage* SpellAnimMontage)
{
	PlayAnimMontage(SpellAnimMontage);
}

void APolyWarBaseCharacter::PlayDamagedAnimMontage(bool RandPlay, int32 Index)
{
	if(DamagedAnimMontages.Num() <= 0) return;

	if(RandPlay)
	{
		const int32 RandNum = FMath::RandRange(0, DamagedAnimMontages.Num() - 1);

		if(!DamagedAnimMontages[RandNum]) return;
		PlayAnimMontage(DamagedAnimMontages[RandNum]);
	}
	else
	{
		if(DamagedAnimMontages.Num() <= Index || !DamagedAnimMontages[Index]) return;
		PlayAnimMontage(DamagedAnimMontages[Index]);
	}
}

void APolyWarBaseCharacter::PlayDeathAnimMontage(bool RandPlay, int32 Index)
{
	if(DeathAnimMontages.Num() <= 0) return;

	if(RandPlay)
	{
		const int32 RandNum = FMath::RandRange(0, DeathAnimMontages.Num() - 1);

		if(!DeathAnimMontages[RandNum]) return;
		PlayAnimMontage(DeathAnimMontages[RandNum]);
	}
	else
	{
		if(DeathAnimMontages.Num() <= Index || !DeathAnimMontages[Index]) return;
		PlayAnimMontage(DeathAnimMontages[Index]);
	}

}

void APolyWarBaseCharacter::WeaponAttackCheckStart()
{
	if(CombatComponent)
	{
		CombatComponent->WeaponAttackCheckStart();
	}
}

void APolyWarBaseCharacter::WeaponAttackCheckEnd()
{
	if(CombatComponent)
	{
		CombatComponent->WeaponAttackCheckEnd();
	}
}

void APolyWarBaseCharacter::SetCustomDepth(bool bEnable, ETeamType InTeamType)
{
	if(!GetMesh()) return;

	if(InTeamType == ETeamType::ET_BlueTeam)
	{
		GetMesh()->SetCustomDepthStencilValue(CUSTOM_DEPTH_BLUE);
	}
	else if(InTeamType == ETeamType::ET_RedTeam)
	{
		GetMesh()->SetCustomDepthStencilValue(CUSTOM_DEPTH_RED);
	}

	GetMesh()->MarkRenderStateDirty();
	GetMesh()->SetRenderCustomDepth(bEnable);
}

bool APolyWarBaseCharacter::GetViewportCenter(FVector& CenterWorldPosition, FVector& CenterWorldDirection)
{
	APlayerController* PlayerController = Cast<APlayerController>(GetInstigatorController());
	if(!PlayerController) return false;

	FVector2D ViewportSize;
	if(GEngine && GEngine->GameViewport)
	{
		GEngine->GameViewport->GetViewportSize(ViewportSize);
	}

	FVector2D Center(ViewportSize.X / 2, ViewportSize.Y / 2);
	return UGameplayStatics::DeprojectScreenToWorld(
		PlayerController, Center, CenterWorldPosition, CenterWorldDirection);
}

FVector APolyWarBaseCharacter::GetRightHandLocation()
{
	if(!GetMesh()) return FVector::ZeroVector;

	const USkeletalMeshSocket* HandSocket = GetMesh()->GetSocketByName(RightHandSocket);
	if(!HandSocket) return FVector::ZeroVector;

	return HandSocket->GetSocketLocation(GetMesh());
}


/*
 * Get, Set Func
 */
bool APolyWarBaseCharacter::GetIsAttacking() const
{
	if(!CombatComponent) return false;

	return CombatComponent->GetIsAttacking();
}

AWeapon* APolyWarBaseCharacter::GetEquippedWeapon() const
{
	if(!CombatComponent || !CombatComponent->GetEquippedWeapon()) return nullptr;

	return CombatComponent->GetEquippedWeapon();
}

// If not possible, -1 is returned
float APolyWarBaseCharacter::GetWeaponAttackRange() const
{
	if(!CombatComponent || !CombatComponent->GetEquippedWeapon()) return -1.0f;

	return CombatComponent->GetEquippedWeapon()->GetAttackRange();
}

// If not possible, -1 is returned
float APolyWarBaseCharacter::GetWeaponAttackAngle() const
{
	if(!CombatComponent || !CombatComponent->GetEquippedWeapon()) return -1.0f;

	return CombatComponent->GetEquippedWeapon()->GetAttackAngle();
}

// If not possible, -1 is returned
float APolyWarBaseCharacter::GetAttackAnimMontageLen(int32 MontageNum)
{
	if(AttackAnimMontages.Num() < MontageNum) return -1.0f;

	return AttackAnimMontages[MontageNum]->GetPlayLength();
}

// If not possible, -1 is returned
float APolyWarBaseCharacter::GetWeaponSkillAnimPlayLen(EWeaponSkill WeaponSkill) const
{
	if(!CombatComponent || !CombatComponent->GetEquippedWeapon()) return -1.0f;
	if(!WeaponSkillAnimMontages.Contains(WeaponSkill)) return -1.0f;

	return WeaponSkillAnimMontages[WeaponSkill]->GetPlayLength();
}

float APolyWarBaseCharacter::GetCurrentHealth() const
{
	if(!StateComponent) return 0.0f;

	return StateComponent->GetCurrentHealth();
}

float APolyWarBaseCharacter::GetMaxHealth() const
{
	if(!StateComponent) return 0.0f;

	return StateComponent->GetMaxHealth();
}
