#pragma once

#include "Abilities/GameplayAbility.h"
#include "Engine/CollisionProfile.h"
#include "Interaction/InteractionOption.h"

#include "InteractionGameplayAbility_Interact.generated.h"

struct FGameplayAbilityActorInfo;
struct FGameplayEventData;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FInteractionOptionsUpdatedEvent, const TArray<FInteractionOption>&, InteractionOptions);

UCLASS(Abstract)
class INTERACTIONSYSTEM_API UInteractionGameplayAbility_Interact : public UGameplayAbility
{
	GENERATED_BODY()

public:
	UInteractionGameplayAbility_Interact(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	virtual void ActivateAbility(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		const FGameplayEventData* TriggerEventData) override;

	UFUNCTION(BlueprintCallable)
	void UpdateInteractions(const TArray<FInteractionOption>& InteractiveOptions);

	UFUNCTION(BlueprintCallable)
	bool TriggerInteraction();

	UPROPERTY(BlueprintAssignable)
	FInteractionOptionsUpdatedEvent OnInteractionOptionsUpdated;

protected:
	virtual FInteractionOption PickInteractionOption(const TArray<FInteractionOption>& InOptions) const;

	UPROPERTY(BlueprintReadWrite)
	TArray<FInteractionOption> CurrentOptions;

	UPROPERTY(EditDefaultsOnly)
	float InteractionScanRate = 0.1f;

	UPROPERTY(EditDefaultsOnly)
	float InteractionScanRange = 500.0f;

	UPROPERTY(EditDefaultsOnly)
	TEnumAsByte<ECollisionChannel> InteractionScanChannel = ECC_Visibility;

	UPROPERTY(EditDefaultsOnly)
	FCollisionProfileName InteractionTraceProfile;

	UPROPERTY(EditDefaultsOnly)
	bool bShowInteractionTraceDebug = false;
};
