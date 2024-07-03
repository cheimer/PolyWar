// Fill out your copyright notice in the Description page of Project Settings.


#include "Spell/FallingSpell.h"

#include "Components/SphereComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystemComponent.h"

void AFallingSpell::BeginPlay()
{
	Super::BeginPlay();

	SpellCollision->OnComponentHit.AddDynamic(this, &ThisClass::OnSpellBeginHit);
}

void AFallingSpell::OnSpellBeginHit(UPrimitiveComponent* HitComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	if(OtherComp)
	{
		ECollisionChannel CollisionChannel = OtherComp->GetCollisionObjectType();
		if(CollisionChannel == ECC_WorldStatic)
		{
			SpellHitGround();
		}
	}
}

void AFallingSpell::SpellHitGround()
{
	SpellCollision->SetSimulatePhysics(false);
	SpellCollision->SetNotifyRigidBodyCollision(false);
	SpellCollision->SetWorldScale3D(SpellCollision->GetComponentScale() * 2.0f);

	SpellCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	SpellCollision->SetCollisionResponseToAllChannels(ECR_Ignore);
	SpellCollision->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);

	FTimerHandle DestroyAfterGroundTimer;
	GetWorldTimerManager().SetTimer(DestroyAfterGroundTimer, this, &ThisClass::DestroySpell, SpellDurationAfterGround);

	if(SpellHitGroundParticle)
	{
		CurrentParticle->Deactivate();
		UParticleSystemComponent* HitParticleComponent = UGameplayStatics::SpawnEmitterAttached(SpellHitGroundParticle, SpellCollision);
		HitParticleComponent->OnSystemFinished.AddDynamic(this, &ThisClass::FinishParticle);
		CurrentParticle = HitParticleComponent;
	}
}

bool AFallingSpell::GetSpawnLocation(FVector& SpawnLocation)
{
	if(!GetOwner()) return false;

	FHitResult HitResult;
	if(!LineTraceSpellRange(HitResult)) return false;

	if(HitResult.bBlockingHit)
	{
		SpawnLocation = HitResult.ImpactPoint;
		SpawnLocation.Z += SpellSpawnHeight;
	}
	else
	{
		FHitResult HitResultToDown;
		FVector EndDownPosition = HitResult.TraceEnd + FVector::DownVector * SpellSpawnHeight;
		GetWorld()->LineTraceSingleByChannel(HitResultToDown, HitResult.TraceEnd, EndDownPosition, ECC_Visibility);
		if(HitResultToDown.bBlockingHit)
		{
			SpawnLocation = HitResultToDown.ImpactPoint;
			SpawnLocation.Z += SpellSpawnHeight;
		}
		// It's worked when ground exist under HitPoint. So if want to Use Empty Space, Add that case.
		else
		{
			SpawnLocation = HitResult.TraceEnd;
		}
	}

	return true;
}

void AFallingSpell::SetSpawnDefault()
{
	Super::SetSpawnDefault();

	if(GetOwner() && GetOwner()->HasAuthority())
	{
		MulticastSetCollision();
	}
}

void AFallingSpell::MulticastSetCollision_Implementation()
{
	SpellCollision->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	SpellCollision->SetCollisionResponseToAllChannels(ECR_Ignore);
	SpellCollision->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	SpellCollision->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Block);

	SpellCollision->SetSimulatePhysics(true);
	SpellCollision->SetNotifyRigidBodyCollision(true);

}
