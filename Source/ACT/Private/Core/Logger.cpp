#include "Logger.h"
#include "Runtime/Engine/Classes/Kismet/KismetSystemLibrary.h"

void ULogger::Print(const FString& InString, FLinearColor textColor, float duration, bool bPrintToScreen, bool bPrintToLog) {
	UKismetSystemLibrary::PrintString(GEngine->GetWorldContexts()[0].World(), InString, true, true, textColor, duration);
}
