// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "NumberPlayerController.h"
#include "GameFramework/GameModeBase.h"
#include "NumberGameMode.generated.h"

UCLASS()
class SAMPLECHAT_API ANumberGameMode : public AGameModeBase
{
    GENERATED_BODY()

public:
    ANumberGameMode();

    virtual void BeginPlay() override;

    UFUNCTION(BlueprintCallable, Category = "NumberGame")
    void GetMessageFromClient(const FString& Msg, const FString& UserID, ANumberPlayerController* SenderPC);
    UFUNCTION(BlueprintCallable, Category = "NumberGame")
    void GoBroadCast(const FString& Msg);

    UFUNCTION()
    void OnRep_TurnTimer();
    UFUNCTION(BlueprintCallable, Category = "Game")
    float GetTurnTimer() const;

protected:
    UPROPERTY(Replicated)
    TArray<int32> TargetNumber;
    UPROPERTY(Replicated)
    int32 HostAttempts;
    UPROPERTY(Replicated)
    int32 GuestAttempts;
    UPROPERTY(Replicated)
    int32 HostWins;
    UPROPERTY(Replicated)
    int32 GuestWins;
    UPROPERTY(Replicated)
    bool bIsHostTurn;
    UPROPERTY(Replicated, BlueprintReadOnly)
    float TurnTimer;
    UPROPERTY(Replicated)
    FString HostsID;
    UPROPERTY(Replicated)
    FString GuestsID;
    UPROPERTY(Replicated)
    FString HostsNik;
    UPROPERTY(Replicated)
    FString GuestsNik; 

    void CheckGameState(int32 Strikes, bool bIsOut, bool bIsHost);
    void ResetGame();
    void CheckCurrentscore();
    void HandleWin(bool bIsHost);
    bool ValidateGuessInput(const FString& NumberStr, TArray<int32>& OutGuess, FString& OutErrorMessage);


    FTimerHandle TurnTimerHandle;
    // 타이머 틱
    void TurnTimerTick();
    void StartTurnTimer();


    // 네트워크 복제를 위한 함수
    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
};
