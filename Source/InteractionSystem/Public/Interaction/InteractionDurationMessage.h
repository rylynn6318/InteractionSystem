#pragma once

#include "CoreMinimal.h"
#include "Interaction/InteractionGameplayTags.h"

#include "InteractionDurationMessage.generated.h"

USTRUCT(BlueprintType)
struct FInteractionDurationMessage
{
	GENERATED_BODY()

public:
	// Optional utility payload kept intentionally for HUD/UX flows that visualize timed interactions.
	UPROPERTY(BlueprintReadWrite)
	TObjectPtr<AActor> Instigator = nullptr;

	UPROPERTY(BlueprintReadWrite)
	float Duration = 0.0f;
};
