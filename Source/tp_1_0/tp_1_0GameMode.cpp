// Copyright Epic Games, Inc. All Rights Reserved.

#include "tp_1_0GameMode.h"
#include "tp_1_0Character.h"
#include "UObject/ConstructorHelpers.h"

Atp_1_0GameMode::Atp_1_0GameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPersonCPP/Blueprints/ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}
