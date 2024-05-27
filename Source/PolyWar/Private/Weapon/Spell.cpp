// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon/Spell.h"

#include "Components/SphereComponent.h"

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
	
}

void ASpell::OnSpellBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
}
