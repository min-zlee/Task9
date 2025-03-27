// Fill out your copyright notice in the Description page of Project Settings.
#include "NumberEvaluator.h"

void UNumberEvaluator::EvaluateGuess(const TArray<int32>& Target, const TArray<int32>& Guess, int32& OutStrikes, int32& OutBalls)
{
    OutStrikes = 0;
    OutBalls = 0;

    TSet<int32> TargetSet(Target);
    for (int32 i = 0; i < 3; i++)
    {
        if (Guess[i] == Target[i])
        {
            OutStrikes++;
        }
        else if (TargetSet.Contains(Guess[i]))
        {
            OutBalls++;
        }
    }
}