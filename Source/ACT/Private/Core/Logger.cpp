#include "Logger.h"
#include "Runtime/Engine/Classes/Kismet/KismetSystemLibrary.h"

void ULogger::Print(const FString& InString, FLinearColor textColor, float duration, bool bPrintToScreen, bool bPrintToLog) {
	UWorld* world = GEngine->GameViewport->GetWorld();
	if (!world) {
		world = GEngine->GetWorldContexts()[0].World();
	}

	UKismetSystemLibrary::PrintString(world, InString, true, true, textColor, duration);
}
