// Made by Bruce Crum


#include "InfinityGameInstance.h"
#include "Engine/GameEngine.h"
#include "GameFramework/GameModeBase.h"
#include "GameMapsSettings.h"

UInfinityGameInstance::UInfinityGameInstance()
{

}

AGameModeBase* UInfinityGameInstance::CreateGameModeForURL(FURL InURL, UWorld* InWorld)
{
	UGameEngine* const GameEngine = Cast<UGameEngine>(GEngine);
	AWorldSettings* Settings = InWorld->GetWorldSettings();

	bool bScenarioSelectedGameMode = false;

	FString GamemodeParam, ScenarioParam;
	FString	Error, Options;
	FString SelectedGamemode;

	for (int32 i = 0; i < InURL.Op.Num(); i++)
	{
		// Append option
		Options += TEXT("?");
		Options += InURL.Op[i];

		// Parse out gamemode params
		FParse::Value(*InURL.Op[i], TEXT("GAME="), GamemodeParam);
	}

	// Use the gamemode param by default
	SelectedGamemode = GamemodeParam;

	// Get the GameMode class. Start by using the default game type specified in the map's worldsettings.  It may be overridden by settings below.
	TSubclassOf<AGameModeBase> GameClass = Settings->DefaultGameMode;

	// If there is a GameMode parameter in the URL, allow it to override the default game type
	if (!SelectedGamemode.IsEmpty())
	{
		FString const GameClassName = UGameMapsSettings::GetGameModeForName(SelectedGamemode);

		// If the gamename was specified, we can use it to fully load the pergame PreLoadClass packages
		if (GameEngine)
		{
			GameEngine->LoadPackagesFully(InWorld, FULLYLOAD_Game_PreLoadClass, *GameClassName);
		}

		// Don't overwrite the map's world settings if we failed to load the value off the command line parameter
		TSubclassOf<AGameModeBase> GameModeParamClass = LoadClass<AGameModeBase>(nullptr, *GameClassName);
		if (GameModeParamClass)
		{
			GameClass = GameModeParamClass;
		}
		else
		{
			UE_LOG(LogLoad, Warning, TEXT("Failed to load game mode '%s' specified by URL options."), *GameClassName);
		}
	}

	// Fall back to game default
	if (!GameClass)
	{
		GameClass = LoadClass<AGameModeBase>(nullptr, *UGameMapsSettings::GetGlobalDefaultGameMode());
	}

	if (!GameClass)
	{
		// Fall back to raw GameMode
		GameClass = AGameModeBase::StaticClass();
	}
	else
	{
		// See if game instance wants to override it
		GameClass = OverrideGameModeClass(GameClass, FPaths::GetBaseFilename(InURL.Map), Options, *InURL.Portal);
	}

	// no matter how the game was specified, we can use it to load the PostLoadClass packages
	if (GameEngine)
	{
		GameEngine->LoadPackagesFully(InWorld, FULLYLOAD_Game_PostLoadClass, GameClass->GetPathName());
		GameEngine->LoadPackagesFully(InWorld, FULLYLOAD_Game_PostLoadClass, TEXT("LoadForAllGameModes"));
	}

	// Spawn the GameMode.
	UE_LOG(LogLoad, Log, TEXT("Game class is '%s'"), *GameClass->GetName());
	FActorSpawnParameters SpawnInfo;
	SpawnInfo.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	SpawnInfo.ObjectFlags |= RF_Transient;	// We never want to save game modes into a map

	return InWorld->SpawnActor<AGameModeBase>(GameClass, SpawnInfo);
}
