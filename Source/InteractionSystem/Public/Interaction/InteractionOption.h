#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "GameplayTagContainer.h"

#include "InteractionOption.generated.h"

class IInteractableTarget;
class UUserWidget;

USTRUCT(BlueprintType)
struct FInteractionOption
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadWrite)
	TScriptInterface<IInteractableTarget> InteractableTarget;

	// Semantic action identifier (e.g. Action.Interaction.Pickup, Action.Interaction.Use).
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FGameplayTag ActionTag;

	// Gameplay event tag used when triggering the selected interaction option.
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FGameplayTag ActivationEventTag;

	// Higher value means higher priority when selecting among multiple options.
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Priority = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText Text;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText SubText;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSubclassOf<UGameplayAbility> InteractionAbilityToGrant;

	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<UAbilitySystemComponent> TargetAbilitySystem = nullptr;

	UPROPERTY(BlueprintReadOnly)
	FGameplayAbilitySpecHandle TargetInteractionAbilityHandle;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSoftClassPtr<UUserWidget> InteractionWidgetClass;

public:
	FORCEINLINE bool operator==(const FInteractionOption& Other) const
	{
		return InteractableTarget == Other.InteractableTarget &&
			ActionTag == Other.ActionTag &&
			ActivationEventTag == Other.ActivationEventTag &&
			Priority == Other.Priority &&
			InteractionAbilityToGrant == Other.InteractionAbilityToGrant &&
			TargetAbilitySystem == Other.TargetAbilitySystem &&
			TargetInteractionAbilityHandle == Other.TargetInteractionAbilityHandle &&
			InteractionWidgetClass == Other.InteractionWidgetClass &&
			Text.IdenticalTo(Other.Text) &&
			SubText.IdenticalTo(Other.SubText);
	}

	FORCEINLINE bool operator!=(const FInteractionOption& Other) const
	{
		return !operator==(Other);
	}

	FORCEINLINE bool operator<(const FInteractionOption& Other) const
	{
		const UObject* ThisTargetObject = InteractableTarget.GetObject();
		const UObject* OtherTargetObject = Other.InteractableTarget.GetObject();
		if (ThisTargetObject != OtherTargetObject)
		{
			return ThisTargetObject < OtherTargetObject;
		}

		if (Priority != Other.Priority)
		{
			return Priority > Other.Priority;
		}

		if (ActionTag != Other.ActionTag)
		{
			return ActionTag.GetTagName().LexicalLess(Other.ActionTag.GetTagName());
		}

		if (ActivationEventTag != Other.ActivationEventTag)
		{
			return ActivationEventTag.GetTagName().LexicalLess(Other.ActivationEventTag.GetTagName());
		}

		if (InteractionAbilityToGrant != Other.InteractionAbilityToGrant)
		{
			const FString ThisAbilityPath = InteractionAbilityToGrant ? InteractionAbilityToGrant->GetPathName() : FString();
			const FString OtherAbilityPath = Other.InteractionAbilityToGrant ? Other.InteractionAbilityToGrant->GetPathName() : FString();
			return ThisAbilityPath < OtherAbilityPath;
		}

		if (TargetAbilitySystem != Other.TargetAbilitySystem)
		{
			return TargetAbilitySystem < Other.TargetAbilitySystem;
		}

		if (TargetInteractionAbilityHandle != Other.TargetInteractionAbilityHandle)
		{
			const int32 HandleCompare = FMemory::Memcmp(&TargetInteractionAbilityHandle, &Other.TargetInteractionAbilityHandle, sizeof(FGameplayAbilitySpecHandle));
			return HandleCompare < 0;
		}

		if (InteractionWidgetClass != Other.InteractionWidgetClass)
		{
			return InteractionWidgetClass.ToSoftObjectPath().ToString() < Other.InteractionWidgetClass.ToSoftObjectPath().ToString();
		}

		if (!Text.IdenticalTo(Other.Text))
		{
			return Text.ToString() < Other.Text.ToString();
		}

		if (!SubText.IdenticalTo(Other.SubText))
		{
			return SubText.ToString() < Other.SubText.ToString();
		}

		return false;
	}
};
