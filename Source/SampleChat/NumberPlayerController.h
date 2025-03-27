#pragma once
#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/PlayerState.h"
#include "NumberPlayerController.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnMessageReceived, const FString&, Msg);
class UUserWidget;

UCLASS()
class SAMPLECHAT_API ANumberPlayerController : public APlayerController
{
    GENERATED_BODY()

public:
    ANumberPlayerController();

    UPROPERTY(BlueprintAssignable, Category = "NumberGame")
    FOnMessageReceived OnMessageReceivedDelegate;

    UFUNCTION(Client, Reliable)
    void ClientReceiveMessage(const FString& Msg);

    UFUNCTION(Client, Reliable)
    void UpdateTimerUI(float NewTime);

    UFUNCTION(Server, Reliable, WithValidation, BlueprintCallable, Category = "NumberGame")
    void SendMessageToServer(const FString& Msg);

protected:
    virtual void BeginPlay() override;
    virtual void Tick(float DeltaTime) override;


    UPROPERTY(EditAnywhere, Category = "NumberGame")
    TSubclassOf<UUserWidget> ChatWindowClass;

    UPROPERTY()
    UUserWidget* ChatWindow;
    UPROPERTY()
    class UEditableTextBox* TimerTextBox;

    UFUNCTION(BlueprintImplementableEvent, Category = "NumberGame")
    void OnMessageReceived(const FString& Msg);

};
