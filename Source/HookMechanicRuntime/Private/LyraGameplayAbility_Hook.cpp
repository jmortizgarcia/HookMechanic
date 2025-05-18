// Fill out your copyright notice in the Description page of Project Settings.


#include "LyraGameplayAbility_Hook.h"

#include "GameFramework/Character.h"
#include "DrawDebugHelpers.h"
#include "Engine/World.h"
#include "AbilitySystemGlobals.h"
#include "Abilities/Tasks/AbilityTask_WaitInputPress.h"
#include "Abilities/Tasks/AbilityTask_ApplyRootMotionConstantForce.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Logging/LogMacros.h"

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
		UE_LOG(LogAbilitySystemComponent, Warning, TEXT("Not posible to commit Hook Ability!"));
		EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
		return;
	}
	// No valid character
	ACharacter* Character = Cast<ACharacter>(ActorInfo->AvatarActor);
	if (Character == nullptr)
	{
		UE_LOG(LogAbilitySystemComponent, Warning, TEXT("Not valid character activating Hook Ability!"));
		EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
		return;
	}

	// Perform hook targeting to launch character only in Server 
	if (ActorInfo->IsNetAuthority())
	{
		FHitResult HookHit;
		PerformHookTrace(Character, HookHit);

		// Not valid hook point 
		if (HookHit.bBlockingHit == false)
		{
			UE_LOG(LogAbilitySystemComponent, Verbose, TEXT("Not valid hook point to reach!"));
			EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
			return;
		}

		// Calculations 
		FVector Direction = (HookHit.Location - Character->GetActorLocation()).GetSafeNormal();
		float Distance = FVector::Dist(HookHit.Location, Character->GetActorLocation());
		float Duration = Distance / HookMaxSpeed;

		// Task to launch character
		UAbilityTask_ApplyRootMotionConstantForce* ApplyForceTask = UAbilityTask_ApplyRootMotionConstantForce::ApplyRootMotionConstantForce(this, FName("HookForce"), Direction, HookMaxSpeed, Duration, false, nullptr, ERootMotionFinishVelocityMode::MaintainLastRootMotionVelocity, FVector::ZeroVector, 0.f, false);
		ApplyForceTask->OnFinish.AddDynamic(this, &ThisClass::OnMoveCompleted);
		ApplyForceTask->ReadyForActivation();

		// Client
		Client_PerformHookMovement(HookHit.Location);
	}

	// Input Press Task to cancel again only Locally Controlled 
	if(IsLocallyControlled())
	{
		UAbilityTask_WaitInputPress* WaitInputPressTask = UAbilityTask_WaitInputPress::WaitInputPress(this);
		WaitInputPressTask->OnPress.AddDynamic(this, &ThisClass::OnInputPressed);
		WaitInputPressTask->ReadyForActivation();
	}	

}

void ULyraGameplayAbility_Hook::PerformHookTrace(ACharacter* Character, FHitResult& OutHitResult)
{
	APlayerController* PC = Cast<APlayerController>(Character->GetController());
	if (PC == nullptr)
	{
		UE_LOG(LogAbilitySystemComponent, Warning, TEXT("Not valid character Performing hook trace!"));
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
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
	}
}

void ULyraGameplayAbility_Hook::Client_PerformHookMovement_Implementation(const FVector& Location)
{
	ACharacter* Character = Cast<ACharacter>(CurrentActorInfo->AvatarActor);
	if (Character == nullptr)
	{
		UE_LOG(LogAbilitySystemComponent, Warning, TEXT("Not valid client character Performing hook trace!"));
		return;
	}

	// Draw Hook in client
	FHitResult HitResult;
	PerformHookTrace(Character, HitResult);
	
	// Calculations 
	FVector Direction = (Location - Character->GetActorLocation()).GetSafeNormal();
	float Distance = FVector::Dist(Location, Character->GetActorLocation());
	float Duration = Distance / HookMaxSpeed;

	// Task to launch character
	UAbilityTask_ApplyRootMotionConstantForce* ApplyForceTask = UAbilityTask_ApplyRootMotionConstantForce::ApplyRootMotionConstantForce(this, FName("HookForce"), Direction, HookMaxSpeed, Duration, false, nullptr, ERootMotionFinishVelocityMode::MaintainLastRootMotionVelocity, FVector::ZeroVector, 0.f, false);
	ApplyForceTask->OnFinish.AddDynamic(this, &ThisClass::OnMoveCompleted);
	ApplyForceTask->ReadyForActivation();
}
