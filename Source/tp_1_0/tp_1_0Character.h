// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "tp_1_0Character.generated.h"

class UCameraComponent;
class Utp_1_0HealthComponent;

UENUM(BlueprintType)
enum class EOperativeStance : uint8
{
	Standing,
	Crouching,
	Prone
};

UCLASS(config = Game)
class TP_1_0_API Atp_1_0Character : public ACharacter
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	UCameraComponent* FirstPersonCamera;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Health, meta = (AllowPrivateAccess = "true"))
	Utp_1_0HealthComponent* HealthComponent;

public:
	Atp_1_0Character();

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable, Category = "Stance")
	EOperativeStance GetStance() const { return Stance; }

	UFUNCTION(BlueprintCallable, Category = "Health")
	Utp_1_0HealthComponent* GetHealthComponent() const { return HealthComponent; }

protected:
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	virtual void OnStartCrouch(float HalfHeightAdjust, float ScaledHalfHeightAdjust) override;
	virtual void OnEndCrouch(float HalfHeightAdjust, float ScaledHalfHeightAdjust) override;

	void MoveForward(float Value);
	void MoveRight(float Value);
	void TurnAtRate(float Rate);
	void LookUpAtRate(float Rate);

	void OnCrouchPressed();
	void OnPronePressed();
	void OnSprintPressed();
	void OnSprintReleased();
	void OnJumpPressed();

	UFUNCTION()
	void HandleDeath();

	UFUNCTION()
	void RestartLevelStub();

	void SetStance(EOperativeStance NewStance);
	void RestoreCapsuleFromProne();
	void ApplyStanceCapsuleAndSpeed();
	void UpdateCameraHeight(float DeltaTime);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
	float BaseTurnRate;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
	float BaseLookUpRate;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement")
	float WalkSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement")
	float SprintSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement")
	float CrouchSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement")
	float ProneSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Stance")
	float StandingHalfHeight;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Stance")
	float CrouchHalfHeight;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Stance")
	float ProneHalfHeight;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Stance")
	float StandingEyeHeight;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Stance")
	float CrouchEyeHeight;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Stance")
	float ProneEyeHeight;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Stance")
	EOperativeStance Stance;

	bool bWantsSprint;
	bool bDead;
};
