// Fill out your copyright notice in the Description page of Project Settings.


#include "Spell/ThrowSpell.h"

#include "Character/PolyWarBaseCharacter.h"
#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystemComponent.h"

AThrowSpell::AThrowSpell()
{
	SpellProjectile = CreateDefaultSubobject<UProjectileMovementComponent>("WeaponProjectile");
	SpellProjectile->SetAutoActivate(false);
	SpellProjectile->ProjectileGravityScale = 0.0f;

	SpellProjectile->InitialSpeed = SpellThrowSpeed;
	SpellProjectile->MaxSpeed = SpellThrowSpeed;
}

bool AThrowSpell::GetSpawnLocation(FVector& SpawnLocation)
{
	if(!GetOwner()) return false;

	SpawnLocation = GetOwner()->GetActorLocation() + SpellSpawnLocation;
	return true;
}

void AThrowSpell::ApplyEffectOnce(APolyWarBaseCharacter* EffectedActor)
{
	Super::ApplyEffectOnce(EffectedActor);

	if(!SpellIsPassing)
	{
		SpellCollision->SetSimulatePhysics(false);
		SpellCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		SpellCollision->SetCollisionResponseToAllChannels(ECR_Ignore);

		if(SpellHitParticle)
		{
			CurrentParticle->Deactivate();
			UParticleSystemComponent* HitParticleComponent = UGameplayStatics::SpawnEmitterAttached(SpellHitParticle, SpellCollision);
			HitParticleComponent->OnSystemFinished.AddDynamic(this, &ThisClass::FinishParticle);
			CurrentParticle = HitParticleComponent;
		}
		else
		{
			DestroySpell();
		}
	}
}

void AThrowSpell::SetSpawnDefault()
{
	Super::SetSpawnDefault();

	if(!GetOwner()) return;

	APolyWarBaseCharacter* OwnerCharacter = Cast<APolyWarBaseCharacter>(GetOwner());
	if(!OwnerCharacter) return;

	FVector CenterWorldPosition;
	FVector CenterWorldDirection;
	bool bScreenToWorld = OwnerCharacter->GetViewportCenter(CenterWorldPosition, CenterWorldDirection);
	if(!bScreenToWorld) return;

	FVector EndPosition = CenterWorldPosition + CenterWorldDirection * OwnerCharacter->AdjustThrowPosVal;
	FVector Direction = (EndPosition - GetActorLocation()).GetSafeNormal();

	SpellProjectile->Activate();
	SpellProjectile->Velocity = Direction * SpellThrowSpeed;
}
