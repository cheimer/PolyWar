// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/MapButton.h"

void UMapButton::PostInitProperties()
{
	Super::PostInitProperties();

	if(!OnPressed.IsBound())
	{
		OnPressed.AddDynamic(this, &ThisClass::MapButtonClicked);
	}
}

void UMapButton::MapButtonClicked()
{
	if(bIsUnitButton)
	{
		OnUnitButtonClicked.Broadcast(UnitNum);
	}
	else if (bIsOrderButton)
	{
		OnOrderButtonClicked.Broadcast(OrderType);
	}

}
