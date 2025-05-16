// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "AbilitySystem/Abilities/LyraGameplayAbility.h"

#include "LyraGameplayAbility_Hook.generated.h"

/**
 * ULyraGameplayAbility_Hook
 *
 *	Gameplay ability used for Hook into something. The hook will move player towards the point till it reaches or cancels ability.
 * 
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
	
	// Cancel abilty
	virtual void CancelAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateCancelAbility) override;

	// Performs Hit trace for hook
	void PerformHookTrace(ACharacter* Character, FHitResult& OutHitResult);

	// Callback when hook task is completed
	UFUNCTION()
	void OnMoveCompleted();

	// Callback when input press 
	UFUNCTION()
	void OnInputPressed(float TimePassed);

protected:
	
	// Hook max distance that will be able to reach in cm
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Lyra|Hook")
	float HookMaxDistance = 1500.f;
	
	// Hook launch speed
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Lyra|Hook")
	float HookMaxSpeed = 1000.f;
	
	// Hook launch speed
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Lyra|Hook")
	float HookToleranceStop = 100.f;

	// Hook Force Strength 
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Lyra|Hook")
	float HookStrength = 1000.f;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Lyra|Hook")
	float HookDuration = 1.f;

	// if this will be the only force to move the character mark as false, if true takes into account other forces. 
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Lyra|Hook")
	bool bAdditiveForce = true;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Lyra|Hook")
	bool bEnableGravityDuringHook = false;

	// Force curve 
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Lyra|Hook")
	TObjectPtr<UCurveFloat> StrengthOverTime;

};
