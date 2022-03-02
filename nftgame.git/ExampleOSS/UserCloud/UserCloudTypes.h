// Copyright June Rhodes. MIT Licensed.

#pragma once

#include "UserCloudTypes.generated.h"

USTRUCT(BlueprintType)
struct EXAMPLEOSS_API FCloudFileDataCPP
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadOnly, Category = "User Cloud")
    FString Hash;

    UPROPERTY(BlueprintReadOnly, Category = "User Cloud")
    FName HashType;

    UPROPERTY(BlueprintReadOnly, Category = "User Cloud")
    FString DLName;

    UPROPERTY(BlueprintReadWrite, Category = "User Cloud")
    FString FileName;

    UPROPERTY(BlueprintReadOnly, Category = "User Cloud")
    int32 FileSize;

    UPROPERTY(BlueprintReadOnly, Category = "User Cloud")
    FString URL;

    UPROPERTY(BlueprintReadOnly, Category = "User Cloud")
    int64 ChunkID;

    static FCloudFileDataCPP FromNative(const struct FCloudFileHeader &FileHeader);
};