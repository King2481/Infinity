// Made by Bruce Crum

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "InfinityPlayerController.generated.h"

class AInfinityPlayerState;
class UUserWidget;
class USoundBase;

/**
 * 
 */
UCLASS()
class INFINITY_API AInfinityPlayerController : public APlayerController
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
};
