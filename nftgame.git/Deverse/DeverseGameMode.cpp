// Copyright Cosugames, LLC 2021

#include "DeverseGameMode.h"
#include "DeverseCharacter.h"
#include "UObject/ConstructorHelpers.h"

ADeverseGameMode::ADeverseGameMode()
{
	// set default pawn class to our Blueprinted character
	/*static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPersonCPP/Blueprints/ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}*/
	//return DefaultPawnClass;
}
