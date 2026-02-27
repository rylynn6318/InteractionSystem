#pragma once

#include "Abilities/Tasks/AbilityTask.h"
#include "GameplayAbilitySpec.h"
#include "UObject/ObjectKey.h"

#include "AbilityTask_GrantNearbyInteraction.generated.h"

class UGameplayAbility;

UCLASS()
class INTERACTIONSYSTEM_API UAbilityTask_GrantNearbyInteraction : public UAbilityTask
{
	GENERATED_UCLASS_BODY()

public:
	virtual void Activate() override;

	UFUNCTION(BlueprintCallable, Category = "Ability|Tasks", meta = (HidePin = "OwningAbility", DefaultToSelf = "OwningAbility", BlueprintInternalUseOnly = "TRUE"))
	static UAbilityTask_GrantNearbyInteraction* GrantAbilitiesForNearbyInteractors(
		UGameplayAbility* OwningAbility,
		float InInteractionScanRange,
		float InInteractionScanRate,
		TEnumAsByte<ECollisionChannel> InInteractionScanChannel = ECC_Visibility);

private:
	virtual void OnDestroy(bool AbilityEnded) override;
	void QueryInteractables();

	float InteractionScanRange = 100.0f;
	float InteractionScanRate = 0.1f;
	TEnumAsByte<ECollisionChannel> InteractionScanChannel = ECC_Visibility;

	FTimerHandle QueryTimerHandle;
	TMap<FObjectKey, FGameplayAbilitySpecHandle> InteractionAbilityCache;
};
