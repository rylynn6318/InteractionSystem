#include "Interaction/Tasks/AbilityTask_GrantNearbyInteraction.h"

#include "AbilitySystemComponent.h"
#include "Engine/OverlapResult.h"
#include "Engine/World.h"
#include "GameFramework/Controller.h"
#include "Interaction/IInteractableTarget.h"
#include "Interaction/InteractionOption.h"
#include "Interaction/InteractionQuery.h"
#include "Interaction/InteractionStatics.h"
#include "TimerManager.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(AbilityTask_GrantNearbyInteraction)

UAbilityTask_GrantNearbyInteraction::UAbilityTask_GrantNearbyInteraction(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

UAbilityTask_GrantNearbyInteraction* UAbilityTask_GrantNearbyInteraction::GrantAbilitiesForNearbyInteractors(
	UGameplayAbility* OwningAbility,
	float InInteractionScanRange,
	float InInteractionScanRate,
	TEnumAsByte<ECollisionChannel> InInteractionScanChannel)
{
	UAbilityTask_GrantNearbyInteraction* MyObj = NewAbilityTask<UAbilityTask_GrantNearbyInteraction>(OwningAbility);
	MyObj->InteractionScanRange = InInteractionScanRange;
	MyObj->InteractionScanRate = InInteractionScanRate;
	MyObj->InteractionScanChannel = InInteractionScanChannel;
	return MyObj;
}

void UAbilityTask_GrantNearbyInteraction::Activate()
{
	SetWaitingOnAvatar();

	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().SetTimer(QueryTimerHandle, this, &ThisClass::QueryInteractables, InteractionScanRate, true);
	}
}

void UAbilityTask_GrantNearbyInteraction::OnDestroy(bool AbilityEnded)
{
	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(QueryTimerHandle);
	}

	if (AbilitySystemComponent.IsValid())
	{
		for (const TPair<FObjectKey, FGameplayAbilitySpecHandle>& Pair : InteractionAbilityCache)
		{
			if (Pair.Value.IsValid())
			{
				AbilitySystemComponent->ClearAbility(Pair.Value);
			}
		}
	}

	InteractionAbilityCache.Reset();
	Super::OnDestroy(AbilityEnded);
}

void UAbilityTask_GrantNearbyInteraction::QueryInteractables()
{
	UWorld* World = GetWorld();
	AActor* ActorOwner = GetAvatarActor();
	if (!World || !ActorOwner || !AbilitySystemComponent.IsValid())
	{
		return;
	}

	FCollisionQueryParams Params(SCENE_QUERY_STAT(UAbilityTask_GrantNearbyInteraction), false);
	TArray<FOverlapResult> OverlapResults;
	World->OverlapMultiByChannel(
		OUT OverlapResults,
		ActorOwner->GetActorLocation(),
		FQuat::Identity,
		InteractionScanChannel,
		FCollisionShape::MakeSphere(InteractionScanRange),
		Params);

	if (OverlapResults.Num() == 0)
	{
		return;
	}

	TArray<TScriptInterface<IInteractableTarget>> InteractableTargets;
	UInteractionStatics::AppendInteractableTargetsFromOverlapResults(OverlapResults, OUT InteractableTargets);

	FInteractionQuery InteractionQuery;
	InteractionQuery.RequestingAvatar = ActorOwner;
	InteractionQuery.RequestingController = Cast<AController>(ActorOwner->GetInstigatorController());
	if (!InteractionQuery.RequestingController.IsValid())
	{
		InteractionQuery.RequestingController = Cast<AController>(ActorOwner->GetOwner());
	}

	TArray<FInteractionOption> Options;
	for (TScriptInterface<IInteractableTarget>& InteractiveTarget : InteractableTargets)
	{
		FInteractionOptionBuilder InteractionBuilder(InteractiveTarget, Options);
		InteractiveTarget->GatherInteractionOptions(InteractionQuery, InteractionBuilder);
	}

	for (FInteractionOption& Option : Options)
	{
		if (!Option.InteractionAbilityToGrant)
		{
			continue;
		}

		const FObjectKey ObjectKey(Option.InteractionAbilityToGrant);
		if (InteractionAbilityCache.Find(ObjectKey))
		{
			continue;
		}

		FGameplayAbilitySpec Spec(Option.InteractionAbilityToGrant, 1, INDEX_NONE, this);
		FGameplayAbilitySpecHandle Handle = AbilitySystemComponent->GiveAbility(Spec);
		InteractionAbilityCache.Add(ObjectKey, Handle);
	}
}
