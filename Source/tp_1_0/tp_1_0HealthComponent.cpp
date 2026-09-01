// Copyright Epic Games, Inc. All Rights Reserved.

#include "tp_1_0HealthComponent.h"

Utp_1_0HealthComponent::Utp_1_0HealthComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

	MaxHealth = 100.f;
	RegenDelay = 4.f;
	RegenRate = 12.f;
	Health = MaxHealth;
	bDead = false;
	LastDamageTime = -1000.f;
}

void Utp_1_0HealthComponent::BeginPlay()
{
	Super::BeginPlay();

	Health = MaxHealth;
	bDead = false;

	if (AActor* Owner = GetOwner())
	{
		Owner->OnTakeAnyDamage.AddDynamic(this, &Utp_1_0HealthComponent::HandleTakeAnyDamage);
	}
}

void Utp_1_0HealthComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (bDead || Health >= MaxHealth || RegenRate <= 0.f)
	{
		return;
	}

	const UWorld* World = GetWorld();
	if (!World)
	{
		return;
	}

	if (World->GetTimeSeconds() - LastDamageTime >= RegenDelay)
	{
		Health = FMath::Min(MaxHealth, Health + RegenRate * DeltaTime);
	}
}

void Utp_1_0HealthComponent::HandleTakeAnyDamage(AActor* DamagedActor, float Damage, const UDamageType* DamageType, AController* InstigatedBy, AActor* DamageCauser)
{
	if (bDead || Damage <= 0.f)
	{
		return;
	}

	Health = FMath::Max(0.f, Health - Damage);
	LastDamageTime = GetWorld() ? GetWorld()->GetTimeSeconds() : 0.f;

	if (Health <= 0.f)
	{
		bDead = true;
		OnHealthDepleted.Broadcast();
	}
}
