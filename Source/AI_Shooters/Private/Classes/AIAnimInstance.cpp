// Fill out your copyright notice in the Description page of Project Settings.


#include "Classes/AIAnimInstance.h"
#include "CharacterBase.h"
#include "Kismet/KismetMathLibrary.h"

UAIAnimInstance::UAIAnimInstance()
{

}

void UAIAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();
	Character = Cast<ACharacterBase>(TryGetPawnOwner());
}

void UAIAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	if (!Character || DeltaSeconds == 0.0f) { return; }

	FVector NewVelocity = Character->GetVelocity();
	Speed = NewVelocity.Size();
	Direction = CalculateDirection(NewVelocity, Character->GetActorRotation());

	SetCharacterProperties();
	if (bIsInCombat)
	{
		SetPitch();
		SetRootYawOffset();
		StartTurn = UKismetMathLibrary::Abs(RootYawOffset) > MaxTurnAngle;
		Recoil(DeltaSeconds);
		return;
	}
	RootYawOffset = 0.0f;
}

void UAIAnimInstance::SetCharacterProperties()
{
	bADS = Character->AnimValues.bADS;
	bIsCrouching = Character->AnimValues.bIsCrouching;
	bIsInCombat = Character->AnimValues.bIsInCombat;
	bIsShooting = Character->AnimValues.bIsShooting;
	bIsSitting = Character->AnimValues.bIsSitting;
	YawActual = RootYawOffset * -1.0f;
	bSprinting = SetSprinting();
}

void UAIAnimInstance::SetPitch()
{
	FRotator PawnRotation = Character->GetActorRotation();
	FRotator AimRotation = Character->GetBaseAimRotation();
	Pitch = UKismetMathLibrary::NormalizedDeltaRotator(AimRotation, PawnRotation).Pitch;
}

void UAIAnimInstance::SetRootYawOffset()
{
	if (Speed > 0.0f || IsAnyMontagePlaying()) { RootYawOffset = 0.0f; }

	YawLastTick = Yaw;
	Yaw = Character->GetActorRotation().Yaw;
	YawChangeOverFrame = YawLastTick - Yaw;

	RootYawOffset = UKismetMathLibrary::NormalizeAxis(YawChangeOverFrame + RootYawOffset);

	if (GetCurveValue(Turning) > 0.0f)
	{
		DistanceCurveValueLastFrame = DistanceCurveValue;
		DistanceCurveValue = GetCurveValue(DistanceToPivot);

		(RootYawOffset > 0.0f) ? TurnDirection = -1.0f : TurnDirection = 1.0f;
		DistanceCurveDifference = DistanceCurveValueLastFrame - DistanceCurveValue;

		RootYawOffset = RootYawOffset - (DistanceCurveDifference * TurnDirection);
		ABSRootYawOffset = UKismetMathLibrary::Abs(RootYawOffset);
		if (ABSRootYawOffset > MaxTurnAngle)
		{
			YawToSubtract = ABSRootYawOffset - MaxTurnAngle;
			YawMultiplier = 0.0f;
			(RootYawOffset > 0.0f) ? YawMultiplier = 1.0f : YawMultiplier = -1.0f;
			YawToSubtract = YawToSubtract * YawMultiplier;

			RootYawOffset = RootYawOffset - YawToSubtract;
		}
	}
	return;
}

void UAIAnimInstance::Recoil(float Seconds)
{
	if (!bIsShooting)
	{
		HandRotation = FRotator(0.0f, 0.0f, 0.0f);
		return;
	}

	FRotator NewRotation;
	(HandRotation == FRotator(0.0f, 0.0f, 0.0f)) ? NewRotation = FRotator(0.0f, 0.0f, -3.0f) : NewRotation = FRotator(0.0f, 0.0f, 0.0f);
	HandRotation = UKismetMathLibrary::RInterpTo(HandRotation, NewRotation, Seconds * 2.0f, 15.0f);
}

bool UAIAnimInstance::SetSprinting()
{
	return (Speed > 300.0f);
}