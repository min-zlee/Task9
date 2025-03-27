#include "NumberGameMode.h"
#include "NumberGenerator.h"
#include "NumberEvaluator.h"
#include "Net/UnrealNetwork.h"

ANumberGameMode::ANumberGameMode()
{
    PlayerControllerClass = ANumberPlayerController::StaticClass();

    HostAttempts = 3;
    GuestAttempts = 3;
    HostWins = 0;
    GuestWins = 0;
    bIsHostTurn = true;
    TurnTimer = 15.0f;
    HostsID = "";
    GuestsID = "";
}

void ANumberGameMode::BeginPlay()
{
    Super::BeginPlay();
    TargetNumber = UNumberGenerator::GenerateNumber();
    UE_LOG(LogTemp, Warning, TEXT("TargetNumber: %d, %d, %d"), TargetNumber[0], TargetNumber[1], TargetNumber[2]);
}

void ANumberGameMode::GetMessageFromClient(const FString& Msg, const FString& UserID, ANumberPlayerController* SenderPC)
{
    if (HostsID.IsEmpty())
    {
        HostsID = UserID;
        HostsNik = Msg;
        GoBroadCast("Host: [ " + Msg + " ] has joined the game.");
        return;
    }
    else if (GuestsID.IsEmpty() && UserID != HostsID)
    {
        GuestsID = UserID;
        GuestsNik = Msg;

        GoBroadCast("Guest: [ " + Msg + " ] has joined the game.");
        GoBroadCast("Start Game! [ " + HostsNik + " ] Turn. Enter your number!\n");
        StartTurnTimer();
        return;
    }

    // 게임이 시작되지 않았으면 입력 무시
    if (HostsID.IsEmpty() || GuestsID.IsEmpty()) return;

    // 현재 턴 확인
    bool bIsHost = (UserID == HostsID);
    bool bIsGuest = (UserID == GuestsID);
    if (!bIsHost && !bIsGuest)
    {
        if (SenderPC)
        {
            SenderPC->ClientReceiveMessage("You are not part of the game.");
        }
        return;
    }

    if ((bIsHost && !bIsHostTurn) || (bIsGuest && bIsHostTurn))
    {
        if (SenderPC)
        {
            SenderPC->ClientReceiveMessage("XXXX  Not your Turn!  XXXX");
        }
        return;
    }

    if (Msg.StartsWith("/"))
    {
        FString NumberStr = Msg.Mid(1);
        TArray<int32> Guess;
        FString ErrorMessage;

        if (!ValidateGuessInput(NumberStr, Guess, ErrorMessage))
        {
            GoBroadCast(" \n" + ErrorMessage);
            CheckGameState(0, true, bIsHost);
            return;
        }

        int32 Strikes, Balls;
        UNumberEvaluator::EvaluateGuess(TargetNumber, Guess, Strikes, Balls);

        FString SenderNick = (bIsHost) ? HostsNik : GuestsNik;
        FString Result = (Strikes + Balls) ? FString::Printf(TEXT("[ %d ] Strike [ %d ] Ball"), Strikes, Balls) : "OUT!";
        FString CombinedMessage = FString::Printf(TEXT("\n+---                               ---+\n%s: [ %s ]\n%s: %s"), *SenderNick, *NumberStr, *SenderNick, *Result);
        GoBroadCast(CombinedMessage);

        
        if (bIsHost)
        {
            HostAttempts--;
            GoBroadCast("* Host Attempts Left: " + FString::Printf(TEXT("%d"), HostAttempts));
        }
        else
        {
            GuestAttempts--;
            GoBroadCast("* Guest Attempts Left: " + FString::Printf(TEXT("%d"), GuestAttempts));
        }
        GoBroadCast("+---                               ---+\n");
        // 0스트라이크, 0볼인 경우 기회만 소모 (즉시 승리 X)
        CheckGameState(Strikes, false, bIsHost);
    }
    else
    {
        GoBroadCast(UserID + ": " + Msg);
    }
}

bool ANumberGameMode::ValidateGuessInput(const FString& NumberStr, TArray<int32>& OutGuess, FString& OutErrorMessage)
{
    if (NumberStr.Len() != 3)
    {
        OutErrorMessage = "!!!!   Enter a three-digit number    !!!!";
        return false;
    }

    TArray<int32> Guess;
    TSet<int32> UniqueElements;
    for (int32 i = 0; i < 3; i++)
    {
        FString Char = NumberStr.Mid(i, 1);
        if (!Char.IsNumeric())
        {
            OutErrorMessage = "!!!!        Numbers only        !!!!";
            return false;
        }
        int32 Digit = FCString::Atoi(*Char);
        if (Digit < 1 || Digit > 9)
        {
            OutErrorMessage = "!!! Enter a number from 1 to 9  !!!";
            return false;
        }
        if (UniqueElements.Contains(Digit))
        {
            OutErrorMessage = "!!! There are duplicate numbers !!!";
            return false;
        }
        UniqueElements.Add(Digit);
        Guess.Add(Digit);
    }
    OutGuess = Guess;
    return true;
}

void ANumberGameMode::GoBroadCast(const FString& Msg)
{
    for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
    {
        if (ANumberPlayerController* PlayerController = Cast<ANumberPlayerController>(*It))
        {
            PlayerController->ClientReceiveMessage(Msg);
        }
    }
}

void ANumberGameMode::CheckGameState(int32 Strikes, bool bIsOut, bool bIsHost)
{
    if (bIsOut)
    {
        if (bIsHost)
        {
            GuestWins++;
            GoBroadCast("!!!!      Host made a mistake       !!!!\n**  [ " + GuestsNik + " ] Wins this round  **");
        }
        else
        {
            HostWins++;
            GoBroadCast("!!!!      Guest made a mistake       !!!!\n**  [ " + HostsNik + " ] Wins this round  **");
        }
        CheckCurrentscore();
        ResetGame();
        return;
    }

    if (Strikes == 3)
    {
        HandleWin(bIsHost);
        return;
    }

    bool bHostOutOfAttempts = (HostAttempts <= 0);
    bool bGuestOutOfAttempts = (GuestAttempts <= 0);

    if (bHostOutOfAttempts && !bGuestOutOfAttempts)
    {
        GoBroadCast("Host has used all attempts. Guest gets the remaining turns.");
        bIsHostTurn = false;
        TurnTimer = 15.0f; 
        GoBroadCast(GuestsNik + " Turn.");
    }
    else if (bGuestOutOfAttempts && !bHostOutOfAttempts)
    {
        GoBroadCast("Guest has used all attempts. Host gets the remaining turns.");
        bIsHostTurn = true; 
        TurnTimer = 15.0f;
        GoBroadCast(HostsNik + " Turn.");
    }
    else if (bHostOutOfAttempts && bGuestOutOfAttempts)
    {
        GoBroadCast("It's a draw! Both players have used all attempts.");
        CheckCurrentscore();
        ResetGame();
    }
    else
    {
        bIsHostTurn = !bIsHostTurn;
        TurnTimer = 15.0f;
        GoBroadCast(bIsHostTurn ? HostsNik + " Turn." : GuestsNik + " Turn.");
    }
}

void ANumberGameMode::HandleWin(bool bIsHost)
{
    if (bIsHost)
    {
        HostWins++;
        GoBroadCast("**  [ " + HostsNik + " ] Wins this round  **");
    }
    else
    {
        GuestWins++;
        GoBroadCast("**  [ " + GuestsNik + " ] Wins this round  **");
    }
    CheckCurrentscore();
    ResetGame();
}

void ANumberGameMode::ResetGame()
{
    TargetNumber = UNumberGenerator::GenerateNumber();
    HostAttempts = 3;
    GuestAttempts = 3;
    TurnTimer = 15.0f;
    GoBroadCast("\n|||||| Baseball Game |||||||\n"+ (bIsHostTurn ? HostsNik + " Turn." : GuestsNik + " Turn."));
    UE_LOG(LogTemp, Warning, TEXT("TargetNumber: %d, %d, %d"), TargetNumber[0], TargetNumber[1], TargetNumber[2]); //test
}

void ANumberGameMode::CheckCurrentscore()
{
    GoBroadCast("\n#----                                                  ----#\n Current score - Host: "
        + FString::Printf(TEXT("%d"), HostWins) + " | Guest: " + FString::Printf(TEXT("%d"), GuestWins)
        +"\n#----                                                  ----#\n");

}

void ANumberGameMode::TurnTimerTick()
{
    TurnTimer -= 1.0f;
    if (FMath::IsNearlyEqual(TurnTimer, FMath::RoundToFloat(TurnTimer), 0.1f))
    {
        OnRep_TurnTimer();
    }
    if (TurnTimer <= 0.0f)
    {
        if (bIsHostTurn)
        {
            HostAttempts--;
            GoBroadCast("The host's time is up! You missed your chance.");
            CheckGameState(0, false, true);
        }
        else
        {
            GuestAttempts--;
            GoBroadCast("The Guest's time is up! You missed your chance.");
            CheckGameState(0, false, false);
        }

    }
}

void ANumberGameMode::StartTurnTimer()
{
    GetWorldTimerManager().ClearTimer(TurnTimerHandle);
    GetWorldTimerManager().SetTimer(TurnTimerHandle, this, &ANumberGameMode::TurnTimerTick, 1.0f, true);
}

void ANumberGameMode::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);
    DOREPLIFETIME(ANumberGameMode, HostAttempts);
    DOREPLIFETIME(ANumberGameMode, GuestAttempts);
    DOREPLIFETIME(ANumberGameMode, HostWins);
    DOREPLIFETIME(ANumberGameMode, GuestWins);
    DOREPLIFETIME(ANumberGameMode, bIsHostTurn);
    DOREPLIFETIME(ANumberGameMode, TurnTimer);
    DOREPLIFETIME(ANumberGameMode, HostsID);
    DOREPLIFETIME(ANumberGameMode, GuestsID);
    DOREPLIFETIME(ANumberGameMode, TargetNumber);
    DOREPLIFETIME(ANumberGameMode, HostsNik);
    DOREPLIFETIME(ANumberGameMode, GuestsNik);
}

float ANumberGameMode::GetTurnTimer() const
{
    return TurnTimer;
}

void ANumberGameMode::OnRep_TurnTimer()
{
    for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
    {
        ANumberPlayerController* PC = Cast<ANumberPlayerController>(*It);
        if (PC)
        {
            PC->UpdateTimerUI(TurnTimer);
        }
    }
}
