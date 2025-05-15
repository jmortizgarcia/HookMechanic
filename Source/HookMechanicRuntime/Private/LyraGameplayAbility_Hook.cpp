// Fill out your copyright notice in the Description page of Project Settings.


#include "LyraGameplayAbility_Hook.h"

#include "GameFramework/Character.h"
#include "DrawDebugHelpers.h"
#include "Engine/World.h"
#include "AbilitySystemGlobals.h"

ULyraGameplayAbility_Hook::ULyraGameplayAbility_Hook(const FObjectInitializer& ObjectInitializer)
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::ServerInitiated;
}

void ULyraGameplayAbility_Hook::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
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

	// Launch character to hook point
	if (HookHit.bBlockingHit)
	{
		FVector HookTargetLocation = HookHit.Location;
		FVector CharacterLocation = Character->GetActorLocation();
		// Launch Character 
		FVector Direction = (HookTargetLocation - CharacterLocation).GetSafeNormal();
		FVector LaunchVelocityVector = Direction * HookLaunchSpeed;
		Character->LaunchCharacter(LaunchVelocityVector, true, true);
	}

	// Avoid having the ability always active
	EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
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
