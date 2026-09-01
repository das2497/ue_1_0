// Copyright Epic Games, Inc. All Rights Reserved.

#include "tp_1_0Character.h"
#include "tp_1_0HealthComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "Kismet/GameplayStatics.h"

Atp_1_0Character::Atp_1_0Character()
{
	PrimaryActorTick.bCanEverTick = true;

	StandingHalfHeight = 96.f;
	CrouchHalfHeight = 60.f;
	ProneHalfHeight = 28.f;
	StandingEyeHeight = 64.f;
	CrouchEyeHeight = 40.f;
	ProneEyeHeight = 16.f;

	WalkSpeed = 220.f;
	SprintSpeed = 520.f;
	CrouchSpeed = 140.f;
	ProneSpeed = 70.f;

	BaseTurnRate = 45.f;
	BaseLookUpRate = 45.f;
	Stance = EOperativeStance::Standing;
	bWantsSprint = false;
	bDead = false;

	GetCapsuleComponent()->InitCapsuleSize(42.f, StandingHalfHeight);

	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = true;
	bUseControllerRotationRoll = false;

	GetCharacterMovement()->bOrientRotationToMovement = false;
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 540.0f, 0.0f);
	GetCharacterMovement()->JumpZVelocity = 420.f;
	GetCharacterMovement()->AirControl = 0.2f;
	GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
	GetCharacterMovement()->NavAgentProps.bCanCrouch = true;
	GetCharacterMovement()->SetCrouchedHalfHeight(CrouchHalfHeight);

	BaseEyeHeight = StandingEyeHeight;
	CrouchedEyeHeight = CrouchEyeHeight;

	FirstPersonCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
	FirstPersonCamera->SetupAttachment(GetCapsuleComponent());
	FirstPersonCamera->SetRelativeLocation(FVector(0.f, 0.f, StandingEyeHeight));
	FirstPersonCamera->bUsePawnControlRotation = true;

	HealthComponent = CreateDefaultSubobject<Utp_1_0HealthComponent>(TEXT("HealthComponent"));
}

void Atp_1_0Character::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	UpdateCameraHeight(DeltaTime);
	ApplyStanceCapsuleAndSpeed();
}

void Atp_1_0Character::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	check(PlayerInputComponent);

	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &Atp_1_0Character::OnJumpPressed);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

	PlayerInputComponent->BindAction("Crouch", IE_Pressed, this, &Atp_1_0Character::OnCrouchPressed);
	PlayerInputComponent->BindAction("Prone", IE_Pressed, this, &Atp_1_0Character::OnPronePressed);
	PlayerInputComponent->BindAction("Sprint", IE_Pressed, this, &Atp_1_0Character::OnSprintPressed);
	PlayerInputComponent->BindAction("Sprint", IE_Released, this, &Atp_1_0Character::OnSprintReleased);

	PlayerInputComponent->BindAxis("MoveForward", this, &Atp_1_0Character::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &Atp_1_0Character::MoveRight);
	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("TurnRate", this, &Atp_1_0Character::TurnAtRate);
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("LookUpRate", this, &Atp_1_0Character::LookUpAtRate);
}

void Atp_1_0Character::BeginPlay()
{
	Super::BeginPlay();

	if (HealthComponent)
	{
		HealthComponent->OnHealthDepleted.AddDynamic(this, &Atp_1_0Character::HandleDeath);
	}
}

void Atp_1_0Character::OnStartCrouch(float HalfHeightAdjust, float ScaledHalfHeightAdjust)
{
	Super::OnStartCrouch(HalfHeightAdjust, ScaledHalfHeightAdjust);
	if (Stance != EOperativeStance::Prone)
	{
		Stance = EOperativeStance::Crouching;
	}
}

void Atp_1_0Character::OnEndCrouch(float HalfHeightAdjust, float ScaledHalfHeightAdjust)
{
	Super::OnEndCrouch(HalfHeightAdjust, ScaledHalfHeightAdjust);
	if (Stance == EOperativeStance::Crouching)
	{
		Stance = EOperativeStance::Standing;
	}
}

void Atp_1_0Character::OnCrouchPressed()
{
	if (bDead)
	{
		return;
	}

	if (Stance == EOperativeStance::Prone)
	{
		SetStance(EOperativeStance::Crouching);
		return;
	}

	if (Stance == EOperativeStance::Crouching)
	{
		SetStance(EOperativeStance::Standing);
	}
	else
	{
		SetStance(EOperativeStance::Crouching);
	}
}

void Atp_1_0Character::OnPronePressed()
{
	if (bDead)
	{
		return;
	}

	if (Stance == EOperativeStance::Prone)
	{
		SetStance(EOperativeStance::Crouching);
	}
	else
	{
		SetStance(EOperativeStance::Prone);
	}
}

void Atp_1_0Character::OnSprintPressed()
{
	bWantsSprint = true;
}

void Atp_1_0Character::OnSprintReleased()
{
	bWantsSprint = false;
}

void Atp_1_0Character::OnJumpPressed()
{
	if (bDead || Stance == EOperativeStance::Prone)
	{
		return;
	}

	if (Stance == EOperativeStance::Crouching)
	{
		SetStance(EOperativeStance::Standing);
		return;
	}

	Jump();
}

void Atp_1_0Character::RestoreCapsuleFromProne()
{
	UCapsuleComponent* Capsule = GetCapsuleComponent();
	if (!Capsule)
	{
		return;
	}

	const float CurrentHalf = Capsule->GetUnscaledCapsuleHalfHeight();
	if (CurrentHalf + 0.5f < StandingHalfHeight)
	{
		const float Delta = StandingHalfHeight - CurrentHalf;
		Capsule->SetCapsuleHalfHeight(StandingHalfHeight);
		AddActorWorldOffset(FVector(0.f, 0.f, Delta));
	}
}

void Atp_1_0Character::SetStance(EOperativeStance NewStance)
{
	if (bDead || NewStance == Stance)
	{
		return;
	}

	const EOperativeStance OldStance = Stance;

	if (OldStance == EOperativeStance::Prone)
	{
		RestoreCapsuleFromProne();
	}

	if (OldStance == EOperativeStance::Crouching && NewStance != EOperativeStance::Crouching)
	{
		UnCrouch();
	}

	Stance = NewStance;

	if (NewStance == EOperativeStance::Crouching)
	{
		Crouch();
	}

	ApplyStanceCapsuleAndSpeed();
}

void Atp_1_0Character::ApplyStanceCapsuleAndSpeed()
{
	UCapsuleComponent* Capsule = GetCapsuleComponent();
	UCharacterMovementComponent* Move = GetCharacterMovement();
	if (!Capsule || !Move)
	{
		return;
	}

	float TargetHalf = StandingHalfHeight;
	float Speed = WalkSpeed;

	switch (Stance)
	{
	case EOperativeStance::Crouching:
		TargetHalf = CrouchHalfHeight;
		Speed = CrouchSpeed;
		break;
	case EOperativeStance::Prone:
		TargetHalf = ProneHalfHeight;
		Speed = ProneSpeed;
		break;
	default:
		TargetHalf = StandingHalfHeight;
		Speed = (bWantsSprint && !bDead) ? SprintSpeed : WalkSpeed;
		break;
	}

	if (Stance == EOperativeStance::Prone)
	{
		const float CurrentHalf = Capsule->GetUnscaledCapsuleHalfHeight();
		if (!FMath::IsNearlyEqual(CurrentHalf, TargetHalf, 0.5f))
		{
			const float Delta = CurrentHalf - TargetHalf;
			Capsule->SetCapsuleHalfHeight(TargetHalf);
			AddActorWorldOffset(FVector(0.f, 0.f, -Delta));
		}
		Move->MaxWalkSpeed = Speed;
		Move->MaxWalkSpeedCrouched = CrouchSpeed;
	}
	else
	{
		Move->MaxWalkSpeed = Speed;
		Move->MaxWalkSpeedCrouched = CrouchSpeed;
	}
}

void Atp_1_0Character::UpdateCameraHeight(float DeltaTime)
{
	if (!FirstPersonCamera)
	{
		return;
	}

	float TargetEye = StandingEyeHeight;
	switch (Stance)
	{
	case EOperativeStance::Crouching:
		TargetEye = CrouchEyeHeight;
		break;
	case EOperativeStance::Prone:
		TargetEye = ProneEyeHeight;
		break;
	default:
		break;
	}

	const FVector Current = FirstPersonCamera->GetRelativeLocation();
	const float NewZ = FMath::FInterpTo(Current.Z, TargetEye, DeltaTime, 12.f);
	FirstPersonCamera->SetRelativeLocation(FVector(Current.X, Current.Y, NewZ));
}

void Atp_1_0Character::HandleDeath()
{
	bDead = true;
	bWantsSprint = false;
	DisableInput(Cast<APlayerController>(GetController()));
	GetCharacterMovement()->DisableMovement();

	if (UWorld* World = GetWorld())
	{
		FTimerHandle RestartHandle;
		World->GetTimerManager().SetTimer(RestartHandle, this, &Atp_1_0Character::RestartLevelStub, 2.0f, false);
	}
}

void Atp_1_0Character::RestartLevelStub()
{
	const FString MapName = UGameplayStatics::GetCurrentLevelName(this, true);
	UGameplayStatics::OpenLevel(this, FName(*MapName));
}

void Atp_1_0Character::MoveForward(float Value)
{
	if (bDead || Controller == nullptr || Value == 0.0f)
	{
		return;
	}

	const FRotator Rotation = Controller->GetControlRotation();
	const FRotator YawRotation(0, Rotation.Yaw, 0);
	const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	AddMovementInput(Direction, Value);
}

void Atp_1_0Character::MoveRight(float Value)
{
	if (bDead || Controller == nullptr || Value == 0.0f)
	{
		return;
	}

	const FRotator Rotation = Controller->GetControlRotation();
	const FRotator YawRotation(0, Rotation.Yaw, 0);
	const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
	AddMovementInput(Direction, Value);
}

void Atp_1_0Character::TurnAtRate(float Rate)
{
	AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void Atp_1_0Character::LookUpAtRate(float Rate)
{
	AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}
