// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Deco/PolyWarCanAttackDeco.h"

#include "BehaviorTree/BlackboardComponent.h"
#include "Character/PolyWarAICharacter.h"
#include "Character/PolyWarBaseCharacter.h"
#include "Controller/PolyWarAIController.h"
#include "Weapon/Weapon.h"

UPolyWarCanAttackDeco::UPolyWarCanAttackDeco()
{
	NodeName = "Is Can Attack";
}

bool UPolyWarCanAttackDeco::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const
{
	Super::CalculateRawConditionValue(OwnerComp, NodeMemory);

	//~ Begin Default Value is Valid
	const AAIController* AIController = OwnerComp.GetAIOwner();
	if(!AIController) return false;

	const APolyWarAICharacter* OwnerCharacter = Cast<APolyWarAICharacter>(AIController->GetPawn());
	if(!OwnerCharacter) return false;

	UBlackboardComponent* Blackboard = OwnerComp.GetBlackboardComponent();
	if(!Blackboard) return false;

	const AActor* ClosestEnemy = Cast<AActor>(Blackboard->GetValueAsObject(ClosestEnemyKey.SelectedKeyName));
	if(!ClosestEnemy) return false;
	//~ End Default Value is Valid

	FVector ToEnemyVector = ClosestEnemy->GetActorLocation() - OwnerCharacter->GetActorLocation();
	float BetweenSize = (OwnerCharacter->GetActorForwardVector().Rotation() - ToEnemyVector.Rotation()).Yaw;
	float HalfAttackAngle = OwnerCharacter->GetWeaponAttackAngle();
	bool bInDegree = false;

	if(BetweenSize < HalfAttackAngle && BetweenSize > -HalfAttackAngle) bInDegree = true;
	if(BetweenSize > 360 - HalfAttackAngle || BetweenSize < -360.0f + HalfAttackAngle) bInDegree = true;

	bool IsSuccess =
		OwnerCharacter->GetLastOrder() != EOrderType::EOD_Move &&
		OwnerCharacter->GetWeaponAttackRange() > 0.0f && OwnerCharacter->GetWeaponAttackAngle() > 0.0f &&
		OwnerCharacter->GetWeaponAttackRange() > FVector::Distance(OwnerCharacter->GetActorLocation(), ClosestEnemy->GetActorLocation()) &&
		bInDegree;
	if(!IsSuccess) return false;

	Blackboard->SetValueAsFloat("AttackDelay", OwnerCharacter->GetEquippedWeapon()->GetAttackDelay());
	return true;
}
