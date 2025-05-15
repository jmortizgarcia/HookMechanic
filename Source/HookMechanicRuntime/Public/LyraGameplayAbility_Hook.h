// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

//#include "CoreMinimal.h"

#include "AbilitySystem/Abilities/LyraGameplayAbility.h"

#include "LyraGameplayAbility_Hook.generated.h"

/**
 * ULyraGameplayAbility_Hook
 *
 *	Gameplay ability used for Hook into a wall.
 */
UCLASS(Abstract)
class HOOKMECHANICRUNTIME_API ULyraGameplayAbility_Hook : public ULyraGameplayAbility
{
	GENERATED_BODY()

public:
	ULyraGameplayAbility_Hook(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:

	// Activates ability 
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

	// Checks ability to be casted
	virtual bool CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags, const FGameplayTagContainer* TargetTags, FGameplayTagContainer* OptionalRelevantTags) const override;

	// Ends Ability
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;
};
