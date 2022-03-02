// Copyright June Rhodes. MIT Licensed.

#pragma once

#include "TitleFileTypes.generated.h"

UCLASS(BlueprintType, Transient, Meta = (DontUseGenericSpawnObject, IsBlueprintBase = "false"))
class EXAMPLEOSS_API UCPPFileData : public UObject
{
    GENERATED_BODY()

private:
    TArray<uint8> Data;

public:
    static UCPPFileData *FromNative(const TArray<uint8> &InData);
    TArray<uint8> ToNative() const
    {
        return this->Data;
    };
    const TArray<uint8> &GetData() const
    {
        return this->Data;
    };
};