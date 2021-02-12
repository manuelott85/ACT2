#include "ACTProjectile_Base.h"

#include "EngineGlobals.h"
#include "Runtime/Engine/Classes/Engine/Engine.h"

AACTProjectile_Base::AACTProjectile_Base() {
	PrimaryActorTick.bCanEverTick = false;
}