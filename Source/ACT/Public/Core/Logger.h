#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Logger.generated.h"

UCLASS()
class ACT_API ULogger : public UObject {
	GENERATED_BODY()

public:
	static void Print(const FString& InString, FLinearColor textColor = FColor::Magenta, float duration = 10, bool bPrintToScreen = true, bool bPrintToLog = true);
};
