// Fill out your copyright notice in the Description page of Project Settings.


#include "LyraGameplayAbility_Hook.h"
//#include "AbilitySystem/Abilities/LyraGameplayAbility.h"

#include "GameFramework/Character.h"
#include "DrawDebugHelpers.h"
#include "Engine/World.h"

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
		EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
		return;
	}

	ACharacter* Character = Cast<ACharacter>(ActorInfo->AvatarActor);
	if (Character == nullptr)
	{
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
		FVector LaunchVelocityVector = Direction * 500;  // HOOK LAUNCH VELOCITY CHANGE TO UPROPERTY
		Character->LaunchCharacter(LaunchVelocityVector, true, true);
	}

	EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
}

bool ULyraGameplayAbility_Hook::CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags, const FGameplayTagContainer* TargetTags, FGameplayTagContainer* OptionalRelevantTags) const
{
	bool bCanActivate = Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags);
	return bCanActivate;
}

void ULyraGameplayAbility_Hook::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void ULyraGameplayAbility_Hook::PerformHookTrace(ACharacter* Character, FHitResult& OutHitResult)
{
	FVector TraceStart = Character->GetActorLocation();
	FVector ForwardLocation = Character->GetActorForwardVector();
	FVector TraceEnd = TraceStart + (ForwardLocation * 1000.f); // MAX DISTANCE OF HOOK CHANGE TO UPROPERTY

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
