// Fill out your copyright notice in the Description page of Project Settings.


#include "LyraGameplayAbility_Hook.h"

#include "GameFramework/Character.h"
#include "DrawDebugHelpers.h"
#include "Engine/World.h"
#include "AbilitySystemGlobals.h"

#include "Abilities/Tasks/AbilityTask_WaitInputPress.h"
#include "Abilities/Tasks/AbilityTask_ApplyRootMotionConstantForce.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "AbilitySystemComponent.h"

ULyraGameplayAbility_Hook::ULyraGameplayAbility_Hook(const FObjectInitializer& ObjectInitializer)
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::ServerInitiated;
}

void ULyraGameplayAbility_Hook::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	UAbilitySystemComponent* AbilitySystem = GetAbilitySystemComponentFromActorInfo();
	if (AbilitySystem)
	{
		auto ROLE = AbilitySystem->GetOwnerRole();
		GEngine->AddOnScreenDebugMessage(INDEX_NONE, 15.f, FColor::Orange, FString::Printf(TEXT("Activando Gancho siendo: [%s]"), *UEnum::GetDisplayValueAsText(ROLE).ToString()));
	}

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

	// Perform hook targeting to launch character only in Server 
	if (AbilitySystem && AbilitySystem->GetOwnerRole() == ROLE_Authority)
	{
		FHitResult HookHit;
		PerformHookTrace(Character, HookHit);

		// Not valid hook point 
		if (HookHit.bBlockingHit == false)
		{
			// Add Log
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
	}

	// Input Press Task to cancel again only Locally Controlled 
	if(IsLocallyControlled())
	{
		UAbilityTask_WaitInputPress* WaitInputPressTask = UAbilityTask_WaitInputPress::WaitInputPress(this);
		WaitInputPressTask->OnPress.AddDynamic(this, &ThisClass::OnInputPressed);
		WaitInputPressTask->ReadyForActivation();
	}	

}

void ULyraGameplayAbility_Hook::CancelAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateCancelAbility)
{
	Super::CancelAbility(Handle, ActorInfo, ActivationInfo, bReplicateCancelAbility);

	UAbilitySystemComponent* AbilitySystem = GetAbilitySystemComponentFromActorInfo();
	if (AbilitySystem)
	{
		auto ROLE = AbilitySystem->GetOwnerRole();
		GEngine->AddOnScreenDebugMessage(INDEX_NONE, 15.f, FColor::Cyan, FString::Printf(TEXT("Cancelando la ability del gancho [%s]"), *UEnum::GetDisplayValueAsText(ROLE).ToString()));
	}
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
		UAbilitySystemComponent* AbilitySystem = GetAbilitySystemComponentFromActorInfo();
		if (AbilitySystem)
		{
			auto ROLE = AbilitySystem->GetOwnerRole();
			GEngine->AddOnScreenDebugMessage(INDEX_NONE, 15.f, FColor::Green, FString::Printf(TEXT("Cancelando la ability del gancho volver a apretar input: [%s]"), *UEnum::GetDisplayValueAsText(ROLE).ToString()));
		}
		//CancelAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true);
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
	
	}
}
