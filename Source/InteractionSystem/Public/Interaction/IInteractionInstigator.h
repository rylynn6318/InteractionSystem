#pragma once

#include "CoreMinimal.h"
#include "Interaction/InteractionOption.h"

#include "IInteractionInstigator.generated.h"

struct FInteractionQuery;

UINTERFACE(MinimalAPI)
class UInteractionInstigator : public UInterface
{
	GENERATED_BODY()
};

class IInteractionInstigator
{
	GENERATED_BODY()

public:
	virtual FInteractionOption ChooseBestInteractionOption(const FInteractionQuery& InteractQuery, const TArray<FInteractionOption>& InteractOptions) = 0;
};
