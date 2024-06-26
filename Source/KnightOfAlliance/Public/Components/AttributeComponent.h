// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "AttributeComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent),Blueprintable,BlueprintType )
class KNIGHTOFALLIANCE_API UAttributeComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UAttributeComponent();
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Actor Attributes")

	float Health;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Actor Attributes")
	float MaxHealth;
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	void RegenStamina(float DeltaTime);
protected:
	// Called when the game starts
	virtual void BeginPlay() override;
private:
	

	UPROPERTY(EditAnywhere, Category = "Actor Attributes")
	float Stamina;

	UPROPERTY(EditAnywhere, Category = "Actor Attributes")
	float MaxStamina;

	UPROPERTY(EditAnywhere, Category = "Actor Attributes")
		int32 Gold;

	UPROPERTY(EditAnywhere, Category = "Actor Attributes")
		int32 Souls;

	UPROPERTY(EditAnywhere, Category = "Actor Attributes")
		float RollCost = 14.f;

	UPROPERTY(EditAnywhere, Category = "Actor Attributes")
		float StaminaRegenRate = 8.f;
public:
	void ReceiveDamage(float Damage);
	void ConsumeStamina(float StaminaCost);
	float GetHealthPercent();
	float GetStaminaPercent();
	bool IsAlive();
	void AddSouls(int32 NumberOfSouls);
	void AddGold(int32 NumberOfGold);
	FORCEINLINE int32 GetGold() const { return Gold; }
	FORCEINLINE int32 GetSouls() const { return Souls; }
	FORCEINLINE float GetRollCost() const { return RollCost; }
	FORCEINLINE float GetStamina() const { return Stamina; }
};
