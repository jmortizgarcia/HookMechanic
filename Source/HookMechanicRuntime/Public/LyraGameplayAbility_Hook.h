// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "AbilitySystem/Abilities/LyraGameplayAbility.h"

#include "LyraGameplayAbility_Hook.generated.h"

/**
 * ULyraGameplayAbility_Hook
 *
 *	Gameplay ability used for Hook into a wall.
 */
UCLASS()
class HOOKMECHANICRUNTIME_API ULyraGameplayAbility_Hook : public ULyraGameplayAbility
{
	GENERATED_BODY()

public:
	ULyraGameplayAbility_Hook(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:

	// Activates ability 
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

	// Performs Hit trace for hook
	void PerformHookTrace(ACharacter* Character, FHitResult& OutHitResult);

protected:
	
	// Hook max distance that will be able to reach in cm
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Lyra|Hook")
	float HookMaxDistance = 700.f;
	
	// Hook launch speed
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Lyra|Hook")
	float HookLaunchSpeed = 1000.f;
};
