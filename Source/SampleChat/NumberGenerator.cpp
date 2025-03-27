// Fill out your copyright notice in the Description page of Project Settings.
#include "NumberGenerator.h"
#include "Math/RandomStream.h"

TArray<int32> UNumberGenerator::GenerateNumber()
{
    TArray<int32> Digits = { 1, 2, 3, 4, 5, 6, 7, 8, 9 };
    TArray<int32> Number;
    Number.Reserve(3);

    FRandomStream RandomStream(FDateTime::Now().GetTicks());
    for (int32 i = 0; i < 3; i++)
    {
        int32 Index = RandomStream.RandRange(0, Digits.Num() - 1);
        Number.Add(Digits[Index]);
        Digits.RemoveAt(Index);
    }
    return Number;
}