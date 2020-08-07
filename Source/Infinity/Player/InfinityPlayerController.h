// Made by Bruce Crum

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "Infinity/Factions/TeamInterface.h"
#include "InfinityPlayerController.generated.h"

class AInfinityPlayerState;
class UUserWidget;
class USoundBase;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnRoundWonDelegate, AInfinityPlayerState*, WinningPlayerState, uint8, WinningTeam);

/**
 * 
 */
UCLASS()
class INFINITY_API AInfinityPlayerController : public APlayerController, public ITeamInterface
{
	GENERATED_BODY()
	
public:

	AInfinityPlayerController();

	void ConstructWidgets();

	virtual void SetupInputComponent() override;

	virtual void ClientTeamMessage_Implementation(APlayerState* SenderPlayerStateBase, const FString& S, FName Type, float MsgLifeTime) override;

	// Called when we recieve a chat message
	virtual void OnChatMessageReceived(const FText& Message, AInfinityPlayerState* SenderPlayerState = nullptr);

	// Server RPC to send chat messages.
	UFUNCTION(Server, Reliable, WithValidation)
	void ServerSendChatMessage(const FText& Message);
	void ServerSendChatMessage_Implementation(const FText& Message);
	bool ServerSendChatMessage_Validate(const FText& Message);

	// Called when the chatbox has popped up and text input is allowed
	void OnChatInputStarted();

	// Called when the chatbox has closed.
	void OnChatInputEnded();

	UFUNCTION(Client, Reliable)
	void ClientPlaySound2D(USoundBase* SoundToPlay);
	void ClientPlaySound2D_Implementation(USoundBase* SoundToPlay);

	/* Called when a round has been won from the gamemode.
	 * If single based, there will be a winning player state,
	 * otherwise, a winning team. 
	 */
	void OnRoundWon(AInfinityPlayerState* WinningPlayerState, uint8 WinningTeam);

	// Client version of OnRoundWon()
	UFUNCTION(Client, Reliable)
	void ClientOnRoundWon(AInfinityPlayerState* WinningPlayerState, uint8 WinningTeam);
	void ClientOnRoundWon_Implementation(AInfinityPlayerState* WinningPlayerState, uint8 WinningTeam);

	// Delegate for when a round is won.
	UPROPERTY(BlueprintAssignable)
	FOnRoundWonDelegate OnRoundWonDelegate;

	// Respawns the player, if applicable.
	void RespawnPlayer();

	// Queues a respawn with a delay.
	void QueueRespawnDelay(float Delay);

	// Joins the specified team
	void JoinTeam(uint8 NewTeam);

	virtual uint8 GetTeamId() const override;

protected:

	// Called when we want to start a chat
	void StartChat();

	// Is this player currently inputing text?
	bool bIsChatting;

	// Called when we want to open the in game menu
	void ToggleInGameMenu();

	// Shows/Hide in game menu based on the argument passed.
	UFUNCTION(BlueprintCallable, Category = "Player Controller")
	void SetShowInGameMenu(const bool NewIsInGameMenu);

	// Is this player looking at the in game menu?
	bool bIsInGameMenu;

	// Pointer to the InGameMenu Widget
	UPROPERTY(BlueprintReadOnly, Category = "HUD")
	UUserWidget* InGameMenuWidget;

	// The class to use for the creation of the InGameMenu Widget
	UPROPERTY(BlueprintReadOnly, Category = "HUD")
	TSubclassOf<UUserWidget> InGameMenuWidgetClass;

	// Called when we want to show the scoreboard
	void ShowScoreboard();

	// Called when we want to hide the scoreboard
	void HideScoreboard();

	// Is this character looking at the scoreboard?
	bool bIsLookingAtScoreboard;

	// Pointer to the Scoreboard Widget
	UPROPERTY(BlueprintReadOnly, Category = "HUD")
	UUserWidget* ScoreboardWidget;

	// The class to use for the creation of the InGameMenu Widget
	UPROPERTY(BlueprintReadOnly, Category = "HUD")
	TSubclassOf<UUserWidget> ScoreboardWidgetClass;

	// Updates the players input mode
	void UpdateInputMode();

	UPROPERTY()
	FTimerHandle DelayRespawnTimerHandle;

	// Called when we want to eject to spectator.
	void OnQueueRespawnDelayFinished();
};
