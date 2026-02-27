#include "Interaction/Abilities/InteractionGameplayAbility_Interact.h"

#include "Abilities/GameplayAbilityTargetTypes.h"
#include "AbilitySystemComponent.h"
#include "Interaction/IInteractableTarget.h"
#include "Interaction/IInteractionInstigator.h"
#include "Interaction/InteractionGameplayTags.h"
#include "Interaction/InteractionQuery.h"
#include "Interaction/InteractionStatics.h"
#include "Interaction/Tasks/AbilityTask_GrantNearbyInteraction.h"
#include "Interaction/Tasks/AbilityTask_WaitForInteractableTargets_SingleLineTrace.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(InteractionGameplayAbility_Interact)

UInteractionGameplayAbility_Interact::UInteractionGameplayAbility_Interact(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalPredicted;
	InteractionTraceProfile.Name = TEXT("BlockAll");
}

void UInteractionGameplayAbility_Interact::ActivateAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	UAbilitySystemComponent* AbilitySystem = GetAbilitySystemComponentFromActorInfo();
	if (AbilitySystem && AbilitySystem->GetOwnerRole() == ROLE_Authority)
	{
		UAbilityTask_GrantNearbyInteraction* Task =
			UAbilityTask_GrantNearbyInteraction::GrantAbilitiesForNearbyInteractors(this, InteractionScanRange, InteractionScanRate, InteractionScanChannel);
		Task->ReadyForActivation();
	}

	if (ActorInfo && ActorInfo->PlayerController.IsValid() && ActorInfo->AvatarActor.IsValid())
	{
		FInteractionQuery InteractionQuery;
		InteractionQuery.RequestingAvatar = ActorInfo->AvatarActor.Get();
		InteractionQuery.RequestingController = ActorInfo->PlayerController.Get();

		FGameplayAbilityTargetingLocationInfo StartLocation;
		StartLocation.LocationType = EGameplayAbilityTargetingLocationType::ActorTransform;
		StartLocation.SourceActor = ActorInfo->AvatarActor.Get();

		UAbilityTask_WaitForInteractableTargets_SingleLineTrace* WaitTask =
			UAbilityTask_WaitForInteractableTargets_SingleLineTrace::WaitForInteractableTargets_SingleLineTrace(
				this,
				InteractionQuery,
				InteractionTraceProfile,
				StartLocation,
				InteractionScanRange,
				InteractionScanRate,
				bShowInteractionTraceDebug);
		WaitTask->InteractableObjectsChanged.AddDynamic(this, &ThisClass::UpdateInteractions);
		WaitTask->ReadyForActivation();
	}
}

void UInteractionGameplayAbility_Interact::UpdateInteractions(const TArray<FInteractionOption>& InteractiveOptions)
{
	CurrentOptions = InteractiveOptions;
	OnInteractionOptionsUpdated.Broadcast(CurrentOptions);
}

FInteractionOption UInteractionGameplayAbility_Interact::PickInteractionOption(const TArray<FInteractionOption>& InOptions) const
{
	if (InOptions.Num() == 0)
	{
		return FInteractionOption();
	}

	if (InOptions.Num() == 1)
	{
		return InOptions[0];
	}

	AActor* AvatarActor = GetAvatarActorFromActorInfo();
	IInteractionInstigator* Instigator = AvatarActor ? Cast<IInteractionInstigator>(AvatarActor) : nullptr;
	if (Instigator)
	{
		FInteractionQuery InteractionQuery;
		InteractionQuery.RequestingAvatar = AvatarActor;
		if (const FGameplayAbilityActorInfo* CurrentInfo = GetCurrentActorInfo())
		{
			InteractionQuery.RequestingController = CurrentInfo->PlayerController.Get();
		}
		return Instigator->ChooseBestInteractionOption(InteractionQuery, InOptions);
	}

	return InOptions[0];
}

bool UInteractionGameplayAbility_Interact::TriggerInteraction()
{
	if (CurrentOptions.Num() == 0)
	{
		return false;
	}

	UAbilitySystemComponent* AbilitySystem = GetAbilitySystemComponentFromActorInfo();
	if (!AbilitySystem)
	{
		return false;
	}

	const FInteractionOption InteractionOption = PickInteractionOption(CurrentOptions);
	if (!InteractionOption.TargetAbilitySystem || !InteractionOption.TargetInteractionAbilityHandle.IsValid() || !InteractionOption.InteractableTarget)
	{
		return false;
	}

	AActor* InstigatorActor = GetAvatarActorFromActorInfo();
	AActor* InteractableTargetActor = UInteractionStatics::GetActorFromInteractableTarget(InteractionOption.InteractableTarget);

	if (!InstigatorActor || !InteractableTargetActor)
	{
		return false;
	}

	FGameplayEventData Payload;
	Payload.EventTag = TAG_ABILITY_INTERACTION_ACTIVATE;
	Payload.Instigator = InstigatorActor;
	Payload.Target = InteractableTargetActor;

	InteractionOption.InteractableTarget->CustomizeInteractionEventData(TAG_ABILITY_INTERACTION_ACTIVATE, Payload);

	AActor* TargetActor = const_cast<AActor*>(ToRawPtr(Payload.Target));
	FGameplayAbilityActorInfo TargetActorInfo;
	TargetActorInfo.InitFromActor(InteractableTargetActor, TargetActor, InteractionOption.TargetAbilitySystem);

	return InteractionOption.TargetAbilitySystem->TriggerAbilityFromGameplayEvent(
		InteractionOption.TargetInteractionAbilityHandle,
		&TargetActorInfo,
		TAG_ABILITY_INTERACTION_ACTIVATE,
		&Payload,
		*InteractionOption.TargetAbilitySystem);
}
