#include "NumberPlayerController.h"
#include "NumberGameMode.h"
#include "Blueprint/UserWidget.h"
#include "Components/EditableTextBox.h"

ANumberPlayerController::ANumberPlayerController()
{
    PrimaryActorTick.bCanEverTick = true;
    ChatWindow = nullptr;
    TimerTextBox = nullptr;
    static ConstructorHelpers::FClassFinder<UUserWidget> ChatWindowClassFinder(TEXT("/Game/Widgets/Widget_ChatWindow"));
    if (ChatWindowClassFinder.Succeeded())
    {
        ChatWindowClass = ChatWindowClassFinder.Class;
    }
}

void ANumberPlayerController::BeginPlay()
{
    Super::BeginPlay();
    if (IsLocalController() && ChatWindowClass && !ChatWindow)
    {
        ChatWindow = CreateWidget<UUserWidget>(this, ChatWindowClass);
        if (ChatWindow)
        {
            ChatWindow->AddToViewport();
            TimerTextBox = Cast<UEditableTextBox>(ChatWindow->GetWidgetFromName(TEXT("EditableTextBox_0")));
        }
        ClientReceiveMessage("|||||| Baseball Game |||||||");
    }
}

void ANumberPlayerController::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    /*ANumberGameMode* GameMode = Cast<ANumberGameMode>(GetWorld()->GetAuthGameMode());
    if (GameMode && TimerTextBox)
    {
        float CurrentTime = GameMode->GetTurnTimer();
        TimerTextBox->SetText(FText::FromString(FString::Printf(TEXT("%d"), FMath::RoundToInt(CurrentTime))));
    }*/

}
void ANumberPlayerController::ClientReceiveMessage_Implementation(const FString& Msg)
{
    OnMessageReceivedDelegate.Broadcast(Msg);
}

void ANumberPlayerController::SendMessageToServer_Implementation(const FString& Msg)
{
    if (ANumberGameMode* GameMode = Cast<ANumberGameMode>(GetWorld()->GetAuthGameMode()))
    {
        GameMode->GetMessageFromClient(Msg, PlayerState ? PlayerState->GetPlayerName() : TEXT("UnknownPlayer"), this);
    }
    /*
    ANumberGameMode* GameMode = Cast<ANumberGameMode>(GetWorld()->GetAuthGameMode());
    if (GameMode && PlayerState)
    {
        GameMode->GetMessageFromClient(Msg, PlayerState->GetPlayerName(),this);
    }
    else if (GameMode)
    {
        GameMode->GetMessageFromClient(Msg, TEXT("UnknownPlayer"), this);
    }
    */
}
bool ANumberPlayerController::SendMessageToServer_Validate(const FString& Msg)
{
    return true;
}

void ANumberPlayerController::UpdateTimerUI_Implementation(float NewTime)
{
    if (TimerTextBox)
    {
        TimerTextBox->SetText(FText::FromString(FString::Printf(TEXT("%d"), FMath::RoundToInt(NewTime))));
    }
}