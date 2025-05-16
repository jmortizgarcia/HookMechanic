#include "AbilityTask_HookMove.h"
// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilityTask_HookMove.h"
#include "GameFramework/Character.h"


UAbilityTask_HookMove::UAbilityTask_HookMove(const FObjectInitializer& ObjectInitializer)
{
	bTickingTask = true;
}

UAbilityTask_HookMove* UAbilityTask_HookMove::HookMove(UGameplayAbility* OwningAbility, FName TaskInstanceName, ACharacter* Character, FVector Location, float HookSpeed, float StopDistance)
{
	UAbilityTask_HookMove* Task = NewAbilityTask<UAbilityTask_HookMove>(OwningAbility, TaskInstanceName);
	
	// Define Task members
	Task->CharacterPtr = Character;
	Task->HookLocation = Location;
	Task->Speed = HookSpeed;
	Task->StopDistanceSqr = FMath::Square(StopDistance);
	return Task;
}

void UAbilityTask_HookMove::Activate()
{
	
}

void UAbilityTask_HookMove::TickTask(float DeltaTime)
{
	if (CharacterPtr == nullptr)
	{
		EndTask();
		return;
	}
	//ACharacter* Character = CharacterPtr.Get();
	FVector CurrentLocation = CharacterPtr->GetActorLocation();
	FVector Direction = (HookLocation - CurrentLocation).GetSafeNormal();

	FVector DeltaMove = Direction * Speed * DeltaTime;

	FVector NewLocation = CurrentLocation + DeltaMove;

	if (FVector::DistSquared(NewLocation, HookLocation) <= StopDistanceSqr)
	{
		CharacterPtr->SetActorLocation(HookLocation);
		OnHookFinish.Broadcast();
		EndTask();
		return;
	}

	CharacterPtr->SetActorLocation(NewLocation);
}

void UAbilityTask_HookMove::OnDestroy(bool AbilityIsEnding)
{
	Super::OnDestroy(AbilityIsEnding);
}
