#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "Interaction/InteractionOption.h"

#include "IInteractableTarget.generated.h"

struct FInteractionQuery;

class FInteractionOptionBuilder
{
public:
	FInteractionOptionBuilder(TScriptInterface<IInteractableTarget> InScope, TArray<FInteractionOption>& InOptions)
		: Scope(InScope)
		, Options(InOptions)
	{
	}

	void AddInteractionOption(const FInteractionOption& Option)
	{
		FInteractionOption& OptionEntry = Options.Add_GetRef(Option);
		OptionEntry.InteractableTarget = Scope;
	}

private:
	TScriptInterface<IInteractableTarget> Scope;
	TArray<FInteractionOption>& Options;
};

UINTERFACE(MinimalAPI)
class UInteractableTarget : public UInterface
{
	GENERATED_BODY()
};

class IInteractableTarget
{
	GENERATED_BODY()

public:
	// Populate FInteractionOption fields when adding entries:
	// - ActionTag: semantic action identifier for UI/logic
	// - ActivationEventTag: gameplay event tag used when the option is triggered
	// - Priority: higher value wins before instigator-side tie-breaking
	virtual void GatherInteractionOptions(const FInteractionQuery& InteractQuery, FInteractionOptionBuilder& OptionBuilder) = 0;
	virtual void CustomizeInteractionEventData(const FGameplayTag& InteractionEventTag, FGameplayEventData& InOutEventData) { }
};
