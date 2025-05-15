// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/Tasks/AbilityTask.h"
#include "AbilityTask_HookMove.generated.h"


DECLARE_DYNAMIC_MULTICAST_DELEGATE(FHookMoveFinish);
/**
 *	Move along hook line task.
 */

UCLASS()
class HOOKMECHANICRUNTIME_API UAbilityTask_HookMove : public UAbilityTask
{
	GENERATED_BODY()

public:
	UAbilityTask_HookMove(const FObjectInitializer& ObjectInitializer);
	// Creates Task
	static UAbilityTask_HookMove* HookMove(UGameplayAbility* OwningAbility, FName TaskInstanceName, ACharacter* Character, FVector HookLocation, float Speed, float StopDistance);

	// Task interface
	virtual void Activate() override;
	/** Tick function for this task, if bTickingTask == true */
	virtual void TickTask(float DeltaTime) override;

public:
	FHookMoveFinish OnHookFinish;

protected:
	// Hook Location
	FVector HookLocation;
	// Movement Speed
	float Speed;
	// Character performing movement
	TObjectPtr<ACharacter> CharacterPtr;
	// Distance squared to stop task 
	float StopDistanceSqr;
};
