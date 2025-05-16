// Fill out your copyright notice in the Description page of Project Settings.


#include "LyraGameplayAbility_Hook.h"

#include "GameFramework/Character.h"
#include "DrawDebugHelpers.h"
#include "Engine/World.h"
#include "AbilitySystemGlobals.h"

#include "Abilities/Tasks/AbilityTask_WaitInputPress.h"
#include "Abilities/Tasks/AbilityTask_ApplyRootMotionMoveToForce.h"

ULyraGameplayAbility_Hook::ULyraGameplayAbility_Hook(const FObjectInitializer& ObjectInitializer)
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::ServerInitiated;
}

void ULyraGameplayAbility_Hook::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	// Commit Ability 
	if(!CommitAbility(Handle, ActorInfo, ActivationInfo))
	{
		// Add Log
		EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
		return;
	}
	// No valid character
	ACharacter* Character = Cast<ACharacter>(ActorInfo->AvatarActor);
	if (Character == nullptr)
	{
		// Add Log
		EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
		return;
	}

	// Perform hook targeting to launch character
	FHitResult HookHit;
	PerformHookTrace(Character, HookHit);
	
	// Not valid hook point 
	if (HookHit.bBlockingHit== false)
	{
		// Add Log
		EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
		return;
	}

	// Calculations to mantain momentum
	//FVector Direction = (HookHit.Location - Character->GetActorLocation()).GetSafeNormal();
	float Distance = FVector::Dist(HookHit.Location, Character->GetActorLocation());
	float Duration = Distance / HookMaxSpeed;
	//FVector VelocityOnFinish = Direction * HookMaxSpeed;

	// Hook task 
	UAbilityTask_ApplyRootMotionMoveToForce* ApplyForceTask = UAbilityTask_ApplyRootMotionMoveToForce::ApplyRootMotionMoveToForce(this, FName(), HookHit.Location, Duration, true, EMovementMode::MOVE_Flying, true, PathOffsetCurve, ERootMotionFinishVelocityMode::MaintainLastRootMotionVelocity, FVector::ZeroVector, 0.f);
	// Bind task callbacks when reaching point
	ApplyForceTask->OnTimedOut.AddDynamic(this, &ThisClass::OnMoveCompleted);
	ApplyForceTask->OnTimedOutAndDestinationReached.AddDynamic(this, &ThisClass::OnMoveCompleted);
	ApplyForceTask->ReadyForActivation();
	
	// Input Press Task to cancel again
	UAbilityTask_WaitInputPress* WaitInputPressTask = UAbilityTask_WaitInputPress::WaitInputPress(this);
	WaitInputPressTask->OnPress.AddDynamic(this, &ThisClass::OnInputPressed);
	WaitInputPressTask->ReadyForActivation();
}

void ULyraGameplayAbility_Hook::CancelAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateCancelAbility)
{
	Super::CancelAbility(Handle, ActorInfo, ActivationInfo, bReplicateCancelAbility);
}

void ULyraGameplayAbility_Hook::PerformHookTrace(ACharacter* Character, FHitResult& OutHitResult)
{
	APlayerController* PC = Cast<APlayerController>(Character->GetController());
	if (PC == nullptr)
	{
		return;
	}

	FVector CameraLocation;
	FRotator CameraRotation;
	PC->GetPlayerViewPoint(CameraLocation, CameraRotation);

	FVector TraceStart = CameraLocation;
	FVector Direction = CameraRotation.Vector();
	FVector TraceEnd = TraceStart + (Direction * HookMaxDistance);

	// Ignore the character performing the trace
	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(Character); 

	GetWorld()->LineTraceSingleByChannel(OutHitResult, TraceStart, TraceEnd, ECC_Visibility, QueryParams);

	// DEBUG 
	bool bValidHit = OutHitResult.bBlockingHit;
	DrawDebugLine(GetWorld(), TraceStart, bValidHit ? OutHitResult.Location : TraceEnd, bValidHit ? FColor::Green : FColor::Yellow, false, 2.0f);
	if (bValidHit)
	{
		DrawDebugSphere(GetWorld(), OutHitResult.Location, 10.0f, 12, FColor::Red, false, 2.0f);
	}
}

void ULyraGameplayAbility_Hook::OnMoveCompleted()
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}

void ULyraGameplayAbility_Hook::OnInputPressed(float TimePassed)
{
	if (IsActive())
	{
		CancelAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true);
	}
}
