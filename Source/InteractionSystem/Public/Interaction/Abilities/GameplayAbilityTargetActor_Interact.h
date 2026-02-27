#pragma once

#include "Abilities/GameplayAbilityTargetActor_Trace.h"

#include "GameplayAbilityTargetActor_Interact.generated.h"

UCLASS(Blueprintable)
class INTERACTIONSYSTEM_API AGameplayAbilityTargetActor_Interact : public AGameplayAbilityTargetActor_Trace
{
	GENERATED_BODY()

public:
	AGameplayAbilityTargetActor_Interact(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
	virtual FHitResult PerformTrace(AActor* InSourceActor) override;
};
