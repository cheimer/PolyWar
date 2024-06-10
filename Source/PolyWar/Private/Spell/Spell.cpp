// Fill out your copyright notice in the Description page of Project Settings.


#include "Spell/Spell.h"

#include "ParticleHelper.h"
#include "Character/PolyWarBaseCharacter.h"
#include "Components/SphereComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystem.h"
#include "Particles/ParticleSystemComponent.h"
#include "PolyWarComponent/StateComponent.h"
#include "NiagaraComponent.h"

ASpell::ASpell()
{
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;
	AActor::SetReplicateMovement(true);

	SpellCollision = CreateDefaultSubobject<USphereComponent>("SpellCollision");
	SetRootComponent(SpellCollision);
	SpellCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	SpellCollision->SetCollisionResponseToAllChannels(ECR_Ignore);
	SpellCollision->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);

}

void ASpell::BeginPlay()
{
	Super::BeginPlay();

	if(SpellCollision)
	{
		SpellCollision->OnComponentBeginOverlap.AddDynamic(this, &ThisClass::OnSpellBeginOverlap);
	}

	if(SpellParticle)
	{
		CurrentParticle = UGameplayStatics::SpawnEmitterAttached(SpellParticle, SpellCollision);
		CurrentParticle->OnSystemFinished.AddDynamic(this, &ThisClass::FinishParticle);
	}

	FTimerHandle DestroyTimer;
	GetWorldTimerManager().SetTimer(DestroyTimer, this, &ThisClass::DestroySpell, SpellDuration);
	
}

void ASpell::OnSpellBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if(!GetOwner()) return;
	if(GetOwner() == OtherActor && !bEffectOwner) return;

	APolyWarBaseCharacter* OwnerCharacter = Cast<APolyWarBaseCharacter>(GetOwner());
	APolyWarBaseCharacter* EffectedActor = Cast<APolyWarBaseCharacter>(OtherActor);

	if(OwnerCharacter && EffectedActor)
	{
		if(bEffectEnemy && OwnerCharacter->GetTeamType() != EffectedActor->GetTeamType())
		{
			ApplyEffectOnce(EffectedActor);
		}
		if(bEffectTeam && OwnerCharacter->GetTeamType() == EffectedActor->GetTeamType())
		{
			ApplyEffectOnce(EffectedActor);
		}
	}

}

void ASpell::ApplyEffectOnce(APolyWarBaseCharacter* EffectedActor)
{
	if(!GetOwner()) return;
	APolyWarBaseCharacter* OwnerCharacter = Cast<APolyWarBaseCharacter>(GetOwner());
	if(!OwnerCharacter) return;

	if(HitActors.Contains(EffectedActor))
	{
		return;
	}
	HitActors.Emplace(EffectedActor);

	if(SpellDamage > 0.0f && GetOwner() && GetOwner()->GetInstigatorController())
	{
		UGameplayStatics::ApplyDamage(EffectedActor, SpellDamage * OwnerCharacter->GetSpellPowerRate(),
			GetOwner()->GetInstigatorController(), GetOwner(), UDamageType::StaticClass());
	}

	if(SpellType != ESpellType::EST_None)
	{
		ApplySpellType(EffectedActor, GetOwner());
	}
}

void ASpell::ApplySpellType(AActor* EffectedActor, AActor* DamageCauser)
{
	if(!GetOwner()) return;

	UStateComponent* EffectedState = EffectedActor->GetComponentByClass<UStateComponent>();
	if(!EffectedState) return;

	switch(SpellType)
	{
	case ESpellType::EST_None:
	case ESpellType::ET_MAX:
		break;
	case ESpellType::EST_Fire:
		EffectedState->DOTDamaged(5.0f, 3.0f);
		break;
	case ESpellType::EST_Ice:
		EffectedState->Slowed(0.5f, 5.0f);
		break;
	case ESpellType::EST_SpeedBuf:
		EffectedState->SpeedBuf(2.0f, 5.0f);
		break;
	case ESpellType::EST_PowerBuf:
		EffectedState->PowerBuf(1.5f, 10.0f);
		break;
	case ESpellType::EST_SpellBuf:
		EffectedState->SpellBuf(1.5f, 10.0f);
		break;
	}

}

void ASpell::DestroySpell()
{
	SpellCollision->SetSimulatePhysics(false);
	SpellCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	SpellCollision->SetCollisionResponseToAllChannels(ECR_Ignore);

	if(CurrentParticle)
	{
		if(CurrentParticle->bWasCompleted)
		{
			Destroy();
		}
		else
		{
			CurrentParticle->Deactivate();
		}
	}

}

void ASpell::FinishParticle(UParticleSystemComponent* PSystem)
{
	if(CurrentParticle == PSystem)
	{
		Destroy();
	}
}

bool ASpell::GetSpawnLocation(FVector& Location)
{
	return false;
}

bool ASpell::LineTraceSpellRange(FHitResult& HitResult)
{
	if(!GetOwner()) return false;

	APolyWarBaseCharacter* OwnerCharacter = Cast<APolyWarBaseCharacter>(GetOwner());
	if(!OwnerCharacter) return false;

	FVector CenterWorldPosition;
	FVector CenterWorldDirection;
	bool bScreenToWorld = OwnerCharacter->GetViewportCenter(CenterWorldPosition, CenterWorldDirection);
	if(!bScreenToWorld) return false;

	FVector EndPosition = CenterWorldPosition + CenterWorldDirection * SpellRange;
	GetWorld()->LineTraceSingleByChannel(HitResult, CenterWorldPosition, EndPosition, ECC_Visibility);

	return true;
}
