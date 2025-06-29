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
class UUserWidget;

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

	// Callback when hook task is completed
	UFUNCTION()
	void OnMoveCompleted();

	// Callback when input press 
	UFUNCTION()
	void OnInputPressed(float TimePassed);

	// Client function to perform root motion 
	UFUNCTION(Client, Reliable)
	void Client_PerformHookMovement(const FVector& Location);

protected:
	// Hook max distance that will be able to reach in cm
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Lyra|Hook")
	float HookMaxDistance = 1500.f;
	
	// Hook launch speed
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Lyra|Hook")
	float HookMaxSpeed = 1000.f;
	
	// Hook Ability Icon 
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Lyra|Hook|UI")
	TSubclassOf<UUserWidget> AbilityWidgetClass;

	// Hook Ability Icon Extension Gameplay Tag 
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Lyra|Hook|UI")
	FGameplayTag AbilityWidgetExtensionTag;
};
