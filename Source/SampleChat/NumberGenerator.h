// Fill out your copyright notice in the Description page of Project Settings.
#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "NumberGenerator.generated.h"

UCLASS()
class SAMPLECHAT_API UNumberGenerator : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:

    UFUNCTION(BlueprintCallable, Category = "NumberGame")
    static TArray<int32> GenerateNumber();    // 중복x 3자리 숫자 생성 (1~9)
};
