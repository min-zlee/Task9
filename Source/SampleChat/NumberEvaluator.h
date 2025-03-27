// Fill out your copyright notice in the Description page of Project Settings.
#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "NumberEvaluator.generated.h"

UCLASS()
class SAMPLECHAT_API UNumberEvaluator : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:

    UFUNCTION(BlueprintCallable, Category = "NumberGame")
    static void EvaluateGuess(const TArray<int32>& Target, const TArray<int32>& Guess, int32& OutStrikes, int32& OutBalls);
};
