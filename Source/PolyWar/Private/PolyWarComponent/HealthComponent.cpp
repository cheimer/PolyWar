// Fill out your copyright notice in the Description page of Project Settings.


#include "PolyWarComponent/HealthComponent.h"

UHealthComponent::UHealthComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

}

void UHealthComponent::BeginPlay()
{
	Super::BeginPlay();

}
