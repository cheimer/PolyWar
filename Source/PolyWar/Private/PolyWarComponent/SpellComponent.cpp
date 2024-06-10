// Fill out your copyright notice in the Description page of Project Settings.


#include "PolyWarComponent/SpellComponent.h"

#include "Character/PolyWarBaseCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"
#include "PolyWarComponent/CombatComponent.h"
#include "Spell/Spell.h"
#include "Spell/ThrowSpell.h"

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

	CurrentSpell = GetWorld()->SpawnActorDeferred<ASpell>(Spell, OwnerCharacter->GetTransform(),
		OwnerCharacter, OwnerCharacter, ESpawnActorCollisionHandlingMethod::AlwaysSpawn);

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

	if(!OwnerCharacter->IsLocallyControlled())
	{
		// SpellStart->SpawnActor<ASpell> Worked Server, Client. So Need to remove Client's Spell.
		if(!OwnerCharacter->HasAuthority())
		{
			CurrentSpell->Destroy();
		}
		return;
	}

	FVector SpawnLocation;
	if(!CurrentSpell->GetSpawnLocation(SpawnLocation)) return;

	FTransform SpawnTransform = OwnerCharacter->GetActorTransform();
	SpawnTransform.SetLocation(SpawnLocation);

	if(OwnerCharacter->HasAuthority())
	{
		CurrentSpell->FinishSpawning(SpawnTransform);

		if(Cast<AThrowSpell>(CurrentSpell))
		{
			Cast<AThrowSpell>(CurrentSpell)->SetSpawnDefault(GetThrowSpellDirection(SpawnLocation));
		}
		else
		{
			CurrentSpell->SetSpawnDefault();
		}
	}
	else
	{
		if(Cast<AThrowSpell>(CurrentSpell))
		{
			ServerThrowSpellFinishSpawning(SpawnTransform, GetThrowSpellDirection(SpawnLocation));
		}
		else
		{
			ServerSpellFinishSpawning(SpawnTransform);
		}

		CurrentSpell->Destroy();
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

void USpellComponent::ServerSpellFinishSpawning_Implementation(const FTransform& SpawnTransform)
{
	CurrentSpell->FinishSpawning(SpawnTransform);
	CurrentSpell->SetSpawnDefault();
}

void USpellComponent::ServerThrowSpellFinishSpawning_Implementation(const FTransform& SpawnTransform, const FVector& Direction)
{
	CurrentSpell->FinishSpawning(SpawnTransform);
	if(Cast<AThrowSpell>(CurrentSpell))
	{
		Cast<AThrowSpell>(CurrentSpell)->SetSpawnDefault(Direction);
	}
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

FVector USpellComponent::GetThrowSpellDirection(const FVector& SpellLocation)
{
	if(!OwnerCharacter) return FVector::Zero();

	FVector CenterWorldPosition;
	FVector CenterWorldDirection;
	bool bScreenToWorld = OwnerCharacter->GetViewportCenter(CenterWorldPosition, CenterWorldDirection);
	if(!bScreenToWorld) return FVector::Zero();

	FVector Destination = CenterWorldPosition + CenterWorldDirection * OwnerCharacter->AdjustThrowPosVal;
	return (Destination - SpellLocation).GetSafeNormal();
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
