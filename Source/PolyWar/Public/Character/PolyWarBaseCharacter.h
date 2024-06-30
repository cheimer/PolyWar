// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "PolyWarTypes/TeamType.h"
#include "PolyWarTypes/UnitType.h"
#include "PolyWarTypes/WeaponSkill.h"
#include "PolyWarBaseCharacter.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCharacterDeathDelegate, class APolyWarBaseCharacter*, DeathCharacter);

UCLASS()
class POLYWAR_API APolyWarBaseCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	APolyWarBaseCharacter();
	virtual void PostInitializeComponents() override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void Tick(float DeltaTime) override;

	FOnCharacterDeathDelegate OnCharacterDeathDelegate;

	void CreateFogOfWar(UMaterialInterface* InFogOfWarInterface, UTextureRenderTarget2D* InFogOfWarRender,
		UMaterialInterface* InFogOfWarRevealInterface, UTextureRenderTarget2D* InFogOfWarRevealRender);

	void CreateFogOfWar(UMaterialInterface* InFogOfWarRevealInterface, UTextureRenderTarget2D* InFogOfWarRevealRender);

	virtual void SetPlayerDeath();
	void UpdateWalkSpeed();

	void PlayAttackAnimMontage(bool RandPlay = true, int32 Index = 0);
	void PlayWeaponSkillAnimMontage(EWeaponSkill WeaponSkill);
	void PlaySpellAnimMontage(UAnimMontage* SpellAnimMontage);
	void PlayDamagedAnimMontage(bool RandPlay = true, int32 Index = 0);
	void PlayDeathAnimMontage(bool RandPlay = true, int32 Index = 0);

	virtual void WeaponAttack();
	virtual void WeaponSkillAttack(EWeaponSkill WeaponSkill);
	virtual void SpellAttack(TSubclassOf<class ASpell> Spell);

	bool IsDead();

	float GetPowerRate();
	float GetSpellPowerRate();

	bool GetViewportCenter(FVector& CenterWorldPosition, FVector& CenterWorldDirection);

	FVector GetRightHandLocation();

	void DetectedNumAdd() {DetectedNum++;}
	void DetectedNumMinus() {DetectedNum = FMath::Clamp(DetectedNum - 1, 0, DetectedNum);}

	void SetCustomDepth(bool bEnable, ETeamType InTeamType = ETeamType::ET_NoTeam);

	// Lower Value -> Throw weapon close viewport
	// Higher Value -> Throw weapon close character
	float AdjustThrowPosVal = 5000.f;

protected:
	virtual void BeginPlay() override;

	bool bIsOpenUI = false;
	bool bIsGameEnd = false;

	//~ Begin Components

	UPROPERTY(VisibleAnywhere, Category = "Component")
	class UCombatComponent* CombatComponent;

	UPROPERTY(VisibleAnywhere, Category = "Component")
	class UStateComponent* StateComponent;

	UPROPERTY(VisibleAnywhere, Category = "Component")
	class USpellComponent* SpellComponent;

	UPROPERTY(VisibleAnywhere, Category = "Component")
	class UAIPerceptionStimuliSourceComponent* AIPerceptionSourceComponent;

	UPROPERTY(VisibleAnywhere, Category = "Component")
	class USphereComponent* VisibleSphere;
	//~ End Components

	//~ Begin Move

	UPROPERTY(EditAnywhere, Category = "Settable")
	float DefaultWalkSpeed = 600.0f;
	float CurrentWalkSpeed = DefaultWalkSpeed;

	UPROPERTY(EditAnywhere, Category = "Settable")
	float DefaultRunSpeed = 1200.0f;
	float CurrentRunSpeed = DefaultRunSpeed;

	UPROPERTY(Replicated)
	bool bIsRunning = false;
	//~ End Move

	//~ Begin Weapon

	UFUNCTION(BlueprintCallable)
	void WeaponAttackCheckStart();

	UFUNCTION(BlueprintCallable)
	void WeaponAttackCheckEnd();

	UPROPERTY(EditDefaultsOnly, Category = "Set Should")
	FName RightHandSocket = "RightHandSocket";
	//~ End Weapon

	//~ Begin Health

	UFUNCTION()
	void ReceiveDamage(AActor* DamagedActor, float Damage,
		const UDamageType* DamageType, AController* InstigatedBy, AActor* DamageCauser);

	UPROPERTY(EditAnywhere, Category = "Settable")
	TArray<UAnimMontage*> DamagedAnimMontages;

	UPROPERTY(EditAnywhere, Category = "Settable")
	TArray<UAnimMontage*> DeathAnimMontages;

	void DeathTimerFinished();
	//~ End Health

	//~ Begin Unit Default

	UPROPERTY(EditAnywhere, Category = "Set Should")
	float FogRevealSize = 0.3f;

	UPROPERTY(EditAnywhere, Category = "Set Should")
	EUnitType UnitType;

	UPROPERTY(EditAnywhere, Category = "Set Should", Replicated)
	ETeamType TeamType;
	//~ End Unit Default

	UFUNCTION()
	void VisibleSphereBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void VisibleSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

private:
	//~ Begin Weapon

	class AWeapon* SpawnWeapon(bool IsAttach);

	UFUNCTION(BlueprintCallable, meta = (AllowPrivateAccess = "true"))
	void ThrowWeapon();

	UFUNCTION(Server, Reliable)
	void ServerThrowWeapon(const FVector& Position, const FVector& Direction);

	UPROPERTY(EditAnywhere, Category = "Settable")
	TSubclassOf<AWeapon> EquippedWeaponClass;

	UPROPERTY(EditAnywhere, Category = "Settable")
	TArray<UAnimMontage*> AttackAnimMontages;

	UPROPERTY(EditAnywhere, Category = "Settable")
	TMap<EWeaponSkill, UAnimMontage*> WeaponSkillAnimMontages;
	//~ End Weapon

	UFUNCTION(BlueprintCallable, meta = (AllowPrivateAccess = "true"))
	void SpellEffect();

	//~ Begin FogOfWar

	UPROPERTY()
	UMaterialInstanceDynamic* FogOfWarMaterial;
	UPROPERTY()
	UMaterialInstanceDynamic* FogOfWarRevealMaterial;

	UPROPERTY()
	UTextureRenderTarget2D* FogOfWarRender;
	UPROPERTY()
	UTextureRenderTarget2D* FogOfWarRevealRender;

	void UpdateFog();
	//~ End FogOfWar

	ETeamType LocalPlayerTeam = ETeamType::ET_NoTeam;

	int32 DetectedNum = 0;

public:
	bool GetIsRunning() const {return bIsRunning;}
	bool GetIsAttacking() const;

	AWeapon* GetEquippedWeapon() const;
	float GetWeaponAttackRange() const;
	float GetWeaponAttackAngle() const;
	int32 GetAttackAnimMontagesLen() const {return AttackAnimMontages.Num();}
	float GetAttackAnimMontageLen (int32 MontageNum);
	float GetWeaponSkillAnimPlayLen(EWeaponSkill WeaponSkill) const;
	int32 GetDamagedAnimMontagesLen() const {return DamagedAnimMontages.Num();}
	int32 GetDeathAnimMontagesLen() const {return DeathAnimMontages.Num();}

	float GetCurrentHealth() const;
	float GetMaxHealth() const;
	float GetDefaultWalkSpeed() const {return DefaultWalkSpeed;}
	float GetDefaultRunSpeed() const {return DefaultRunSpeed;}
	void SetWalkSpeed(float InWalkSpeed) {CurrentWalkSpeed = InWalkSpeed;}
	void SetRunSpeed(float InRunSpeed) {CurrentRunSpeed = InRunSpeed;}

	bool GetIsOpenUI() const {return bIsOpenUI;}
	void SetIsOpenUI(bool SetIsOpenUI) {bIsOpenUI = SetIsOpenUI;}
	bool GetIsGameEnd() const {return bIsGameEnd;}
	void SetIsGameEnd(bool SetIsGameEnd) {bIsGameEnd = SetIsGameEnd;}
	ETeamType GetTeamType() const {return TeamType;}
	void SetTeamType(ETeamType InTeamType) {TeamType = InTeamType;}
	EUnitType GetUnitType() const {return UnitType;}
	void SetUnitType(EUnitType InUnitType) {UnitType = InUnitType;}

};
