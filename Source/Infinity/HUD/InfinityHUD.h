// Made by Bruce Crum

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "InfinityHUD.generated.h"

class AInfinityPlayerState;
class UChatBox;
class UUserWidget;

/**
 * 
 */
UCLASS()
class INFINITY_API AInfinityHUD : public AHUD
{
	GENERATED_BODY()

public:
	
	AInfinityHUD();

	// Called when all components have been initialized.
	virtual void PostInitializeComponents() override;

	// Called when we recieved a message;
	virtual void OnChatMessageReceived(const FText& Message, AInfinityPlayerState* SenderPlayerState);

	// Begins chat input
	void StartChatInput();

protected:

	// Creates the game hud element.
	void CreateGameHUD();

	// Creates the chat box
	void CreateChatBoxWidget();

	// Pointer to the chatbox
	UPROPERTY(BlueprintReadOnly, Category = "HUD")
	UChatBox* ChatBoxWidget;

	// The class to use for the creation of the chatbox.
	UPROPERTY(BlueprintReadOnly, Category = "HUD")
	TSubclassOf<UChatBox> ChatBoxWidgetClass;

	// Pointer to the GameHUD Widget
	UPROPERTY(BlueprintReadOnly, Category = "HUD")
	UUserWidget* GameHUD;

	// The class to use for the creation of the GameHUD Widget
	UPROPERTY(BlueprintReadOnly, Category = "HUD")
	TSubclassOf<UUserWidget> DefaultGameHUDClass;

	// Setups the HUD based on the default class.
	virtual void SetupHUD();

	
};
