#pragma once

#include "Abilities/GameplayAbilityTargetTypes.h"
#include "Interaction/InteractionQuery.h"
#include "Interaction/Tasks/AbilityTask_WaitForInteractableTargets.h"

#include "AbilityTask_WaitForInteractableTargets_SingleLineTrace.generated.h"

struct FCollisionProfileName;

class UGameplayAbility;

UCLASS()
class INTERACTIONSYSTEM_API UAbilityTask_WaitForInteractableTargets_SingleLineTrace : public UAbilityTask_WaitForInteractableTargets
{
	GENERATED_UCLASS_BODY()

public:
	virtual void Activate() override;

	UFUNCTION(BlueprintCallable, Category = "Ability|Tasks", meta = (HidePin = "OwningAbility", DefaultToSelf = "OwningAbility", BlueprintInternalUseOnly = "TRUE"))
	static UAbilityTask_WaitForInteractableTargets_SingleLineTrace* WaitForInteractableTargets_SingleLineTrace(
		UGameplayAbility* OwningAbility,
		FInteractionQuery InInteractionQuery,
		FCollisionProfileName InTraceProfile,
		FGameplayAbilityTargetingLocationInfo InStartLocation,
		float InInteractionScanRange = 100.0f,
		float InInteractionScanRate = 0.1f,
		bool bInShowDebug = false);

private:
	virtual void OnDestroy(bool AbilityEnded) override;
	void PerformTrace();

	UPROPERTY()
	FInteractionQuery InteractionQuery;

	UPROPERTY()
	FGameplayAbilityTargetingLocationInfo StartLocation;

	float InteractionScanRange = 100.0f;
	float InteractionScanRate = 0.1f;
	bool bShowDebug = false;

	FTimerHandle TimerHandle;
};
