// Made by Bruce Crum


#include "InfinityHUD.h"
#include "ChatBox.h"
#include "Blueprint/UserWidget.h"
#include "Infinity/Player/InfinityPlayerController.h"

AInfinityHUD::AInfinityHUD()
{
	ChatBoxWidget = nullptr;
	GameHUD = nullptr;

#if UE_SERVER
	ChatBoxWidgetClass = nullptr;
	DefaultGameHUDClass = nullptr;
#else
	// Chat
	static ConstructorHelpers::FClassFinder<UChatBox> ChatBoxWidgetFinder(TEXT("/Game/UI/Widgets/HUD/BP_ChatBox"));
	ChatBoxWidgetClass = ChatBoxWidgetFinder.Succeeded() ? ChatBoxWidgetFinder.Class : nullptr;

	// HUD
	static ConstructorHelpers::FClassFinder<UUserWidget> GameHUDWidgetClass(TEXT("/Game/UI/Widgets/HUD/BP_GameHUD"));
	DefaultGameHUDClass = GameHUDWidgetClass.Class;
#endif
}

void AInfinityHUD::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	SetupHUD();
}

void AInfinityHUD::SetupHUD()
{
#if !UE_SERVER
	CreateGameHUD();
	CreateChatBoxWidget();

	const auto PC = Cast<AInfinityPlayerController>(GetOwningPlayerController());
	if (PC)
	{
		// The player controller needs to construct its specific widgets.
		PC->ConstructWidgets();
	}
#endif
}

void AInfinityHUD::CreateGameHUD()
{
	if (!DefaultGameHUDClass)
	{
		UE_LOG(LogTemp, Error, TEXT("Error, attempted to create GameHUD widget, but GameHUD class is null"));
		return;
	}

	GameHUD = CreateWidget<UUserWidget>(GetOwningPlayerController(), DefaultGameHUDClass);
	if (GameHUD)
	{
		GameHUD->AddToViewport();
	}
}

void AInfinityHUD::CreateChatBoxWidget()
{
	if (!ChatBoxWidgetClass)
	{
		UE_LOG(LogTemp, Error, TEXT("Error, attempted to create chatbox widget, but chatbox class is null"));
		return;
	}

	ChatBoxWidget = CreateWidget<UChatBox>(GetOwningPlayerController(), ChatBoxWidgetClass);
	if (ChatBoxWidget)
	{
		ChatBoxWidget->AddToViewport();
	}
}

void AInfinityHUD::StartChatInput()
{
	if (ChatBoxWidget)
	{
		ChatBoxWidget->StartChatInput();
	}
}

void AInfinityHUD::OnChatMessageReceived(const FText& Message, AInfinityPlayerState* SenderPlayerState)
{
	if (ChatBoxWidget)
	{
		ChatBoxWidget->OnChatMessageRecieved(Message);
	}
}