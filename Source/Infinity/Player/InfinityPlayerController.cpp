// Made by Bruce Crum


#include "InfinityPlayerController.h"
#include "InfinityPlayerState.h"
#include "Infinity/Characters/InfinityCharacter.h"
#include "Infinity/HUD/InfinityHUD.h"
#include "Blueprint/UserWidget.h"
#include "Kismet/GameplayStatics.h"
#include "Infinity/GameModes/InfinityGameState.h"
#include "Infinity/GameModes/InfinityGameModeBase.h"
#include "Infinity/Factions/TeamInfo.h"
#include "InfinitySaveGame.h"

#define SAVE_SLOT_NAME "Profile"

AInfinityPlayerController::AInfinityPlayerController()
{
	bIsChatting = false;
	bIsInGameMenu = false;
	bIsLookingAtScoreboard = false;
	PlayerSkin = nullptr;

	InGameMenuWidget = nullptr;
	ScoreboardWidget = nullptr;

#if UE_SERVER
	InGameMenuWidgetClass = nullptr;
	ScoreboardWidgetClass = nullptr;
#else
	// InGameMenu
	static ConstructorHelpers::FClassFinder<UUserWidget> InGameMenuWidgetFinder(TEXT("/Game/UI/Widgets/HUD/BP_InGameMenu"));
	InGameMenuWidgetClass = InGameMenuWidgetFinder.Succeeded() ? InGameMenuWidgetFinder.Class : nullptr;

	// Scoreboard
	static ConstructorHelpers::FClassFinder<UUserWidget> ScoreboardWidgetFinder(TEXT("/Game/UI/Widgets/HUD/Scoreboard/BP_Scoreboard"));
	ScoreboardWidgetClass = ScoreboardWidgetFinder.Class;
#endif
}

void AInfinityPlayerController::ConstructWidgets()
{
	if (InGameMenuWidgetClass)
	{
		InGameMenuWidget = CreateWidget<UUserWidget>(this, InGameMenuWidgetClass);
		if (InGameMenuWidget)
		{
			// We add to viewport then hide.
			InGameMenuWidget->AddToViewport(1);
			InGameMenuWidget->SetVisibility(ESlateVisibility::Collapsed);
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Error, attempted to create InGameMenu widget, but InGameMenu class is null"));
	}

	if (ScoreboardWidgetClass)
	{
		ScoreboardWidget = CreateWidget<UUserWidget>(this, ScoreboardWidgetClass);
		if (ScoreboardWidget)
		{
			// We add to viewport then hide.
			ScoreboardWidget->AddToViewport();
			ScoreboardWidget->SetVisibility(ESlateVisibility::Collapsed);
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Error, attempted to create Scoreboard widget, but Scoreboard class is null"));
	}
}

void AInfinityPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	InputComponent->BindAction("StartChat", IE_Pressed, this, &AInfinityPlayerController::StartChat);
	InputComponent->BindAction("InGameMenu", IE_Pressed, this, &AInfinityPlayerController::ToggleInGameMenu);

	InputComponent->BindAction("Scoreboard", IE_Pressed, this, &AInfinityPlayerController::ShowScoreboard);
	InputComponent->BindAction("Scoreboard", IE_Released, this, &AInfinityPlayerController::HideScoreboard);
}

void AInfinityPlayerController::StartChat()
{
	const auto TaskForceHUD = Cast<AInfinityHUD>(GetHUD());
	if (TaskForceHUD)
	{
		TaskForceHUD->StartChatInput();
	}
}

void AInfinityPlayerController::ServerSendChatMessage_Implementation(const FText& Message)
{
	for (FConstPlayerControllerIterator Iterator = GetWorld()->GetPlayerControllerIterator(); Iterator; ++Iterator)
	{
		const auto PC = Cast<AInfinityPlayerController>(*Iterator);
		if (!PC)
		{
			continue;
		}

		PC->ClientTeamMessage(PlayerState, Message.ToString(), false ? TEXT("Host") : TEXT("Client"));
	}
}

bool AInfinityPlayerController::ServerSendChatMessage_Validate(const FText& Message)
{
	return true;
}

void AInfinityPlayerController::ClientTeamMessage_Implementation(APlayerState* SenderPlayerStateBase, const FString& S, FName Type, float MsgLifeTime)
{
	Super::ClientTeamMessage_Implementation(SenderPlayerStateBase, S, Type, MsgLifeTime);

	const auto SenderPlayerState = Cast<AInfinityPlayerState>(SenderPlayerStateBase);;

	const bool bGamemodeSay = Type == FName(TEXT("Gamemode"));
	const bool bHostSay = Type == FName(TEXT("Host"));

	static FFormatNamedArguments Arguments;
	Arguments.Add(TEXT("Name"), FText::FromString(SenderPlayerState->GetPlayerName()));
	Arguments.Add(TEXT("Title"), FText::FromString(bHostSay ? TEXT("(Host)") : TEXT("")));
	Arguments.Add(TEXT("Message"), FText::FromString(S));

	OnChatMessageReceived(FText::Format(NSLOCTEXT("HUD", "ChatMessageFormat", "{Name} {Title}: {Message}"), Arguments), SenderPlayerState);
}

void AInfinityPlayerController::OnChatMessageReceived(const FText& Message, AInfinityPlayerState* SenderPlayerState /*= nullptr*/)
{
	const auto TaskForceHUD = Cast<AInfinityHUD>(GetHUD());
	if (TaskForceHUD)
	{
		TaskForceHUD->OnChatMessageReceived(Message, SenderPlayerState);
	}
}

void AInfinityPlayerController::OnChatInputStarted()
{
	bIsChatting = true;
	UpdateInputMode();
}

void AInfinityPlayerController::OnChatInputEnded()
{
	bIsChatting = false;
	UpdateInputMode();
}

void AInfinityPlayerController::ToggleInGameMenu()
{
	SetShowInGameMenu(!bIsInGameMenu);
}

void AInfinityPlayerController::SetShowInGameMenu(const bool NewIsInGameMenu)
{
	if (InGameMenuWidget)
	{
		bIsInGameMenu = NewIsInGameMenu;
		
		const ESlateVisibility Visibility = bIsInGameMenu ? ESlateVisibility::SelfHitTestInvisible : ESlateVisibility::Collapsed;
		InGameMenuWidget->SetVisibility(Visibility);

		UpdateInputMode();
	}
}

void AInfinityPlayerController::ShowScoreboard()
{
	if (ScoreboardWidget)
	{
		bIsLookingAtScoreboard = true;

		ScoreboardWidget->SetVisibility(ESlateVisibility::SelfHitTestInvisible);

		UpdateInputMode();
	}
}

void AInfinityPlayerController::HideScoreboard()
{
	if (ScoreboardWidget)
	{
		bIsLookingAtScoreboard = false;

		ScoreboardWidget->SetVisibility(ESlateVisibility::Collapsed);

		UpdateInputMode();
	}
}

void AInfinityPlayerController::UpdateInputMode()
{
	if (bIsChatting || bIsInGameMenu)
	{
		SetInputMode(FInputModeUIOnly());
		bShowMouseCursor = true;
		return;
	}

	if (bIsLookingAtScoreboard)
	{
		SetInputMode(FInputModeGameAndUI());
		bShowMouseCursor = true;
	}

	SetInputMode(FInputModeGameOnly());
	bShowMouseCursor = false;
}

void AInfinityPlayerController::ClientPlaySound2D_Implementation(USoundBase* SoundToPlay)
{
	if (SoundToPlay)
	{
		UGameplayStatics::PlaySound2D(this, SoundToPlay);
	}
}

void AInfinityPlayerController::RespawnPlayer()
{
	if (GetPawn())
	{
		return;
	}

	// TODO: Move this to a gamemode function?
	auto GM = GetWorld()->GetAuthGameMode<AInfinityGameModeBase>();
	if (GM)
	{
		AActor* PlayerStart = GM->ChoosePlayerStart(this);
		if (PlayerStart)
		{
			GM->RestartPlayerAtPlayerStart(this, PlayerStart);
		}
	}
}

void AInfinityPlayerController::QueueRespawnDelay(float Delay)
{
	GetWorldTimerManager().SetTimer(DelayRespawnTimerHandle, this, &AInfinityPlayerController::OnQueueRespawnDelayFinished, Delay);
}

void AInfinityPlayerController::OnQueueRespawnDelayFinished()
{
	GetWorldTimerManager().ClearTimer(DelayRespawnTimerHandle);
	RespawnPlayer();
}

void AInfinityPlayerController::OnRoundWon(AInfinityPlayerState* WinningPlayerState, uint8 WinningTeam)
{
	ClientOnRoundWon(WinningPlayerState, WinningTeam);
}

void AInfinityPlayerController::ClientOnRoundWon_Implementation(AInfinityPlayerState* WinningPlayerState, uint8 WinningTeam)
{
	SetIgnoreMoveInput(true);
	SetIgnoreLookInput(true);

	OnRoundWonDelegate.Broadcast(WinningPlayerState, WinningTeam);
}

uint8 AInfinityPlayerController::GetTeamId() const
{
	const auto PS = Cast<AInfinityPlayerState>(PlayerState);
	
	return PS ? PS->GetTeamId() : ITeamInterface::InvalidId;
}

void AInfinityPlayerController::JoinTeam(uint8 NewTeam)
{
	const auto World = GetWorld();
	if (!World)
	{
		return;
	}

	// We call the GameState here instead of GameMode as eventually, I may want the ability for players to swap teams.

	const auto GS = Cast<AInfinityGameState>(World->GetGameState());
	const auto PS = Cast<AInfinityPlayerState>(PlayerState);

	if (NewTeam != ITeamInterface::InvalidId)
	{
		if (GS && PS)
		{
			GS->AddPlayerForTeam(PS, NewTeam);

			const auto TeamInfo = GS->GetTeamFromId(NewTeam);
			if (TeamInfo)
			{
				const auto Skin = TeamInfo->SelectRandomSkinForPlayer();
				if (Skin)
				{
					PlayerSkin = Skin;
				}
			}
		}
	}
	else
	{
		// We joined an invalid team, assume this is a free for all mode and set the player skin.

		if (UGameplayStatics::DoesSaveGameExist(SAVE_SLOT_NAME, 0))
		{
			const auto SaveGame = Cast<UInfinitySaveGame>(UGameplayStatics::LoadGameFromSlot(SAVE_SLOT_NAME, 0));
			if (SaveGame)
			{
				PlayerSkin = SaveGame->SavedSkin.LoadSynchronous();
			}
		}
	}
}

void AInfinityPlayerController::SetPawn(APawn* InPawn)
{
	Super::SetPawn(InPawn);

	const auto NewCharacter = Cast<AInfinityCharacter>(InPawn);
	if (NewCharacter)
	{
		NewCharacter->SetTeamId(GetTeamId());
		NewCharacter->SetPlayerSkin(PlayerSkin);
	}
}
