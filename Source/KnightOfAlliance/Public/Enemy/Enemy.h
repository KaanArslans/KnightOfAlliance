// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Characters/BaseCharacter.h"
#include "Characters/CharacterTypes.h"
#include "Enemy.generated.h"

class UHealthBarComponent;
class UPawnSensingComponent;

UCLASS()
class KNIGHTOFALLIANCE_API AEnemy : public ABaseCharacter
{
	GENERATED_BODY()
public:
	AEnemy();
	virtual void Tick(float DeltaTime) override;
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;
	virtual void Destroyed() override;

	virtual void GetHit_Implementation(const FVector& ImpactPoint, AActor* Hitter) override;

protected:
	virtual void BeginPlay() override;
	//virtual void Die_Implementation() override;
	
	void Die();
	void SoulSpawn();
	virtual void Attack() override;
	virtual bool CanAttack() override;
	virtual void HandleDamage(float DamageAmount) override;
	virtual void AttackEnd() override;

	UPROPERTY(BlueprintReadOnly)
	EEnemyState EnemyState = EEnemyState::EES_Patrolling;
private:
	//ai
	void CheckPatrolTarget();
	void CheckCombatTarget();
	void PatrolTimerFinished();
	void HideHealthBar();
	void ShowHealthBar();
	void LoseInterest();
	void StartPatrolling();
	void ChaseTarget();
	bool IsOutsideCombatRadius();
	bool IsOutsideAttackRadius();
	bool IsInsideAttackRadius();
	bool IsChasing();
	bool IsAttacking();
	bool IsDead();
	bool IsEngaged();
	void ClearPatrolTimer();
	void StartAttackTimer();
	void ClearAttackTimer();
	bool InTargetRange(AActor* Target, double Radius);
	void MoveToTarget(AActor* Target);
	AActor* ChoosePatrolTarget();
	void SpawnDefaultWeapon();
	void InitializeEnemy();

	UFUNCTION()
		void PawnSeen(APawn* SeenPawn);

	UPROPERTY(VisibleAnywhere)
		UHealthBarComponent* HealthBarWidget;

	//enemy navigation
	UPROPERTY(VisibleAnywhere)
		UPawnSensingComponent* PawnSensing;

	UPROPERTY(EditAnywhere, Category = Combat)
		TSubclassOf<class AWeapon> WeaponClass;

	UPROPERTY(EditAnywhere, Category = Combat)
		double CombatRadius = 500.f;

	UPROPERTY(EditAnywhere, Category = Combat)
		double AttackRadius = 150.f;

	UPROPERTY(EditAnywhere, Category = Combat)
		double AcceptanceRadius = 50.f;

	UPROPERTY()
		class AAIController* EnemyController;

		//current
	UPROPERTY(EditInstanceOnly, Category = "AI Navigation")
		AActor* PatrolTarget;

	UPROPERTY(EditInstanceOnly, Category = "AI Navigation")
		TArray<AActor*> PatrolTargets;

	UPROPERTY(EditAnywhere)
		double PatrolRadius = 200.f;

	FTimerHandle PatrolTimer;
	FTimerHandle AttackTimer;

	UPROPERTY(EditAnywhere, Category = "Combat")
	float AttackMin = 0.5f;
	UPROPERTY(EditAnywhere, Category = "Combat")
	float AttackMax = 1.f;

	UPROPERTY(EditAnywhere, Category = "Combat")
	float PatrollingSpeed = 125.f;
	UPROPERTY(EditAnywhere, Category = "Combat")
	float ChasingSpeed = 300.f;

	UPROPERTY(EditAnywhere, Category = "Combat")
		float DeathLifeSpan = 8.f;

	UPROPERTY(EditAnywhere, Category = "Combat")
	TSubclassOf<class ASouls> SoulsClass;
};
