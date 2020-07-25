// Made by Bruce Crum


#include "ChatBox.h"
#include "Infinity/Player/InfinityPlayerController.h"
#include "Components/EditableTextBox.h"

UChatBox::UChatBox(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	ChatBody = FText::GetEmpty();
	MaxChatLines = 9;
	bIsBodyDirty = false;
	bInChatMode = false;
	LastChatMessageSentTime = -1.f;
}

void UChatBox::StartChatInput()
{
	const auto OwningPlayer = Cast<AInfinityPlayerController>(GetOwningPlayer());
	if (!OwningPlayer)
	{
		return;
	}

	bInChatMode = true;
	OnChatInputStarted();

	// Grab focus
	if (Input)
	{
		OwningPlayer->OnChatInputStarted();
		Input->SetUserFocus(OwningPlayer);
	}
}

void UChatBox::EndChatInput()
{
	const auto OwningPlayer = Cast<AInfinityPlayerController>(GetOwningPlayer());
	if (!OwningPlayer)
	{
		return;
	}

	bInChatMode = false;
	OnChatInputEnded();

	OwningPlayer->OnChatInputEnded();
}

FText UChatBox::GetChatBody()
{
	if (bIsBodyDirty)
	{
		bIsBodyDirty = false;
		ChatBody = FText::GetEmpty();

		for (int i = 0; i < ChatMessages.Num(); i++)
		{
			auto& Entry = ChatMessages[i];

			static FFormatNamedArguments Arguments;
			Arguments.Add(TEXT("Previous"), ChatBody);
			Arguments.Add(TEXT("Message"), Entry.Message);
			Arguments.Add(TEXT("LineBreak"), FText::FromString(i > 0 ? "\n" : ""));

			ChatBody = FText::Format(NSLOCTEXT("HUD", "ChatFormat", "{Previous}{LineBreak}{Message}"), Arguments);
		}
	}

	return ChatBody;
}

void UChatBox::SendChatMessage(const FText& Message)
{
	const auto PlayerOwner = Cast<AInfinityPlayerController>(GetOwningPlayer());
	if (!PlayerOwner)
	{
		return;
	}

	// Don't send empty or whitespace only chat messages
	if (Message.IsEmptyOrWhitespace())
	{
		return;
	}

	LastChatMessageSentTime = GetWorld() ? GetWorld()->TimeSeconds : -1.f;

	PlayerOwner->ServerSendChatMessage(Message);
}

void UChatBox::OnChatMessageRecieved_Implementation(const FText& Message)
{
	auto& NewEntry = ChatMessages[ChatMessages.AddZeroed()];
	NewEntry.Message = Message;
	NewEntry.TimeCreated = GetWorld() ? GetWorld()->RealTimeSeconds : 0.0f;

	// Remove any overflow from the start of the array
	if (ChatMessages.Num() > MaxChatLines)
	{
		ChatMessages.RemoveAt(0, ChatMessages.Num() - MaxChatLines);
	}

	bIsBodyDirty = true;
}