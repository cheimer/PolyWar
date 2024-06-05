// Fill out your copyright notice in the Description page of Project Settings.


#include "Spell/Spell.h"

#include "ParticleHelper.h"
#include "Character/PolyWarBaseCharacter.h"
#include "Components/SphereComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystem.h"
#include "Particles/ParticleSystemComponent.h"

ASpell::ASpell()
{
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;

	SpellCollision = CreateDefaultSubobject<USphereComponent>("SpellCollision");
	SetRootComponent(SpellCollision);
	SpellCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	SpellCollision->SetCollisionResponseToAllChannels(ECR_Ignore);
	SpellCollision->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);

}

void ASpell::BeginPlay()
{
	Super::BeginPlay();

	SpellCollision->OnComponentBeginOverlap.AddDynamic(this, &ThisClass::OnSpellBeginOverlap);

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
	if(HitActors.Contains(EffectedActor))
	{
		return;
	}
	HitActors.Emplace(EffectedActor);

	if(GetOwner() && GetOwner()->GetInstigatorController())
	{
		UGameplayStatics::ApplyDamage(EffectedActor, SpellDamage,
			GetOwner()->GetInstigatorController(), GetOwner(), UDamageType::StaticClass());
	}
}

void ASpell::DestroySpell()
{
	SpellCollision->SetSimulatePhysics(false);
	SpellCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	SpellCollision->SetCollisionResponseToAllChannels(ECR_Ignore);

	if(CurrentParticle->bWasCompleted)
	{
		Destroy();
	}
	else
	{
		CurrentParticle->Deactivate();
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
