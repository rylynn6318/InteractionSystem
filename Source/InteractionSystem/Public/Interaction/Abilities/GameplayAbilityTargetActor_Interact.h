#pragma once

#include "Abilities/GameplayAbilityTargetActor_Trace.h"

#include "GameplayAbilityTargetActor_Interact.generated.h"

UCLASS(Blueprintable)
class INTERACTIONSYSTEM_API AGameplayAbilityTargetActor_Interact : public AGameplayAbilityTargetActor_Trace
{
	GENERATED_BODY()

public:
	// Legacy/manual targeting path kept intentionally for projects that drive interaction via target actors.
	AGameplayAbilityTargetActor_Interact(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
	virtual FHitResult PerformTrace(AActor* InSourceActor) override;
};
