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
	SpellProjectile->SetIsReplicated(true);
	SpellProjectile->SetAutoActivate(false);

	SpellProjectile->ProjectileGravityScale = 0.0f;
	SpellProjectile->InitialSpeed = SpellThrowSpeed;
	SpellProjectile->MaxSpeed = SpellThrowSpeed;
}

void AThrowSpell::OnSpellBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	Super::OnSpellBeginOverlap(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);

	if(OtherComp)
	{
		ECollisionChannel CollisionChannel = OtherComp->GetCollisionObjectType();
		if(CollisionChannel == ECC_WorldStatic)
		{
			SpellDestroyHit();
		}
	}
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
		SpellDestroyHit();
	}
}

void AThrowSpell::SpellDestroyHit()
{
	SpellCollision->SetSimulatePhysics(false);
	SpellCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	SpellCollision->SetCollisionResponseToAllChannels(ECR_Ignore);

	if(SpellHitParticle)
	{
		CurrentParticle->DestroyComponent();
		UParticleSystemComponent* HitParticleComponent =
			UGameplayStatics::SpawnEmitterAtLocation(this, SpellHitParticle, GetActorLocation());
		HitParticleComponent->OnSystemFinished.AddDynamic(this, &ThisClass::FinishParticle);
		CurrentParticle = HitParticleComponent;
	}
	else
	{
		DestroySpell();
	}
}

void AThrowSpell::SetSpawnDefault()
{
	UE_LOG(LogTemp, Warning, TEXT("ThrowSpell: Need to use SetSpawnDefault(FVector)"));
}

void AThrowSpell::SetSpawnDefault(FVector Direction)
{
	Super::SetSpawnDefault();

	if(GetOwner() && GetOwner()->HasAuthority())
	{
		MulticastSetProjectile(Direction);
	}
}

void AThrowSpell::MulticastSetProjectile_Implementation(FVector_NetQuantize Direction)
{
	SpellCollision->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Overlap);

	SpellProjectile->Activate();
	SpellProjectile->Velocity = Direction * SpellThrowSpeed;
}
