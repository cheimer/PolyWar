// Fill out your copyright notice in the Description page of Project Settings.


#include "PolyWarComponent/SpellComponent.h"

#include "Character/PolyWarBaseCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"
#include "PolyWarComponent/CombatComponent.h"
#include "Weapon/Spell.h"

USpellComponent::USpellComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

}

void USpellComponent::BeginPlay()
{
	Super::BeginPlay();

}

void USpellComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

}

void USpellComponent::SpellStart(TSubclassOf<ASpell> Spell)
{
	if(!OwnerCharacter || !IsValidSpell(Spell)) return;

	CurrentAnimMontage = GetSpellAnimMontage(Spell);

	CurrentSpell = GetWorld()->SpawnActorDeferred<ASpell>(Spell, OwnerCharacter->GetTransform(), OwnerCharacter, OwnerCharacter);

	if(!CurrentAnimMontage || CurrentAnimMontage->GetNumSections() < 3 || !CurrentSpell) return;

	if(CurrentSpell->GetSpellCastingTime() > 0.0f)
	{
		float DefaultSectionLength = GetSpellAnimMontage(Spell)->GetSectionLength(0);
		float CastingTime = CurrentSpell->GetSpellCastingTime() + DefaultSectionLength;
		FTimerHandle SpellTimer;
		OwnerCharacter->GetWorldTimerManager().SetTimer(SpellTimer, this, &USpellComponent::SpellCastingEnd, CastingTime);
	}
	else
	{
		float DefaultSectionLength = GetSpellAnimMontage(Spell)->GetSectionLength(0);
		float CastingSectionLength = GetSpellAnimMontage(Spell)->GetSectionLength(1);
		float CastingTime = DefaultSectionLength + CastingSectionLength;
		FTimerHandle SpellTimer;
		OwnerCharacter->GetWorldTimerManager().SetTimer(SpellTimer, this, &USpellComponent::SpellCastingEnd, CastingTime);
	}

}

void USpellComponent::SpellCastingEnd()
{
	if(!OwnerCharacter || !OwnerCharacter->GetMesh() || !OwnerCharacter->GetMesh()->GetAnimInstance()) return;
	UAnimInstance* AnimInstance = OwnerCharacter->GetMesh()->GetAnimInstance();
	if(!AnimInstance) return;

	AnimInstance->Montage_JumpToSection(FName("CastingEnd"));

	if(!CurrentAnimMontage || CurrentAnimMontage->GetNumSections() < 3) return;
	float CastingEndSectionLength = CurrentAnimMontage->GetSectionLength(2);
	FTimerHandle SpellTimer;
	OwnerCharacter->GetWorldTimerManager().SetTimer(SpellTimer, this, &USpellComponent::SpellEnd, CastingEndSectionLength * 0.9f);
}

void USpellComponent::SpellEffect()
{
	if(!GetWorld() || !OwnerCharacter || !CurrentSpell) return;

	// SpellStart->SpawnActor<ASpell> On Server, Client. So Need to remove Client's Spell.
	if(!OwnerCharacter->IsLocallyControlled())
	{
		if(!OwnerCharacter->HasAuthority())
		{
			CurrentSpell->Destroy();
		}
		return;
	}

	FTransform SpawnTransform;
	SetCurrentSpellTransform(SpawnTransform);

	if(OwnerCharacter->HasAuthority())
		CurrentSpell->FinishSpawning(SpawnTransform);

	if(!OwnerCharacter->HasAuthority())
	{
		CurrentSpell->Destroy();
		ServerFinishSpawning(SpawnTransform);
	}

}

void USpellComponent::SpellEnd()
{
	if(!OwnerCharacter) return;

	CurrentSpell = nullptr;
	CurrentAnimMontage = nullptr;

	UCombatComponent* CombatComponent = OwnerCharacter->GetComponentByClass<UCombatComponent>();
	if(!CombatComponent) return;

	CombatComponent->SpellCastEnd();
}

void USpellComponent::ServerFinishSpawning_Implementation(const FTransform& SpawnTransform)
{
	CurrentSpell->FinishSpawning(SpawnTransform);
}

bool USpellComponent::IsValidSpell(TSubclassOf<ASpell> Spell)
{
	if(Spell == SpellFirstClass && SpellFirstAnimMontage)
	{
		return true;
	}
	if(Spell == SpellSecondClass && SpellSecondAnimMontage)
	{
		return true;
	}
	if(Spell == SpellUltClass && SpellUltAnimMontage)
	{
		return true;
	}

	return false;
}

void USpellComponent::SetCurrentSpellTransform(FTransform& SpawnTransform)
{
	if(!OwnerCharacter || !CurrentSpell) return;

	FVector CenterWorldPosition;
	FVector CenterWorldDirection;
	bool bScreenToWorld = OwnerCharacter->GetViewportCenter(CenterWorldPosition, CenterWorldDirection);
	if(!bScreenToWorld) return;

	FHitResult HitResult;
	FVector EndPosition = CenterWorldPosition + CenterWorldDirection * CurrentSpell->GetSpellRange();
	GetWorld()->LineTraceSingleByChannel(HitResult, CenterWorldPosition, EndPosition, ECC_Visibility);

	FVector SpawnLocation;
	switch(CurrentSpell->GetSpellType())
	{
	case ESpellType::EST_MyAOE:
		SpawnLocation = OwnerCharacter->GetActorLocation();
		break;
	case ESpellType::EST_TargetAOE:
		if(HitResult.bBlockingHit)
		{
			SpawnLocation = HitResult.Location;
		}
		else
		{
			SpawnLocation = EndPosition;
		}
		break;
	case ESpellType::EST_TargetOverhead:
		if(HitResult.bBlockingHit)
		{
			SpawnLocation = HitResult.ImpactPoint;
			SpawnLocation.Z += 3000.0f; // TEMP
		}
		else
		{
			SpawnLocation = EndPosition;
			SpawnLocation.Z += 3000.0f; // TEMP
		}
		break;
	case ESpellType::EST_MAX:
		return;
	}

	SpawnTransform = OwnerCharacter->GetActorTransform();
	SpawnTransform.SetLocation(SpawnLocation);
}

/*
 * Get, Set Func
 */
void USpellComponent::SetOwnerCharacter(APolyWarBaseCharacter* InOwnerCharacter)
{
	if(!InOwnerCharacter) return;
	OwnerCharacter = InOwnerCharacter;
}

TSubclassOf<ASpell> USpellComponent::GetSpellFirstClass() const
{
	return SpellFirstClass;
}

TSubclassOf<ASpell> USpellComponent::GetSpellSecondClass() const
{
	return SpellSecondClass;
}

TSubclassOf<ASpell> USpellComponent::GetSpellUltClass() const
{
	return SpellUltClass;
}

UAnimMontage* USpellComponent::GetSpellAnimMontage(TSubclassOf<ASpell> Spell)
{
	if(Spell == SpellFirstClass)
	{
		return SpellFirstAnimMontage;
	}
	if(Spell == SpellSecondClass)
	{
		return SpellSecondAnimMontage;
	}
	if(Spell == SpellUltClass)
	{
		return SpellUltAnimMontage;
	}

	return nullptr;
}
