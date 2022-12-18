// Fill out your copyright notice in the Description page of Project Settings.


#include "CharacterBase.h"
#include "Kismet/GameplayStatics.h"
#include "Components/CapsuleComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "GameFramework/CharacterMovementComponent.h"

ACharacterBase::ACharacterBase()
{

}

void ACharacterBase::ToggleCombat(const bool Newbool)
{

}

void ACharacterBase::ToggleCrouch(const bool Newbool)
{

}

void ACharacterBase::ToggleADS(const bool Newbool)
{

}

void ACharacterBase::ToggleSprinting(bool Newbool)
{

}

void ACharacterBase::AttachWeapon(USkeletalMeshComponent* WeaponMesh, const FName SocketName)
{

}

void ACharacterBase::StartWeaponFire()
{

}

void ACharacterBase::SpawnMuzzleFlash()
{

}

void ACharacterBase::StopWeaponFire()
{

}

void ACharacterBase::PlayMontage(UAnimMontage* Montage, float Rate)
{

}

UAnimMontage* ACharacterBase::ReturnRandomVariation()
{
	return nullptr;
}

void ACharacterBase::ToggleSitting(bool Newbool)
{

}

void ACharacterBase::SetCollision(bool Newbool)
{

}

bool ACharacterBase::IsHostile(ACharacterBase* Agent)
{
	return false;
}

void ACharacterBase::MakeANoise(FVector Location)
{

}

FHitResult ACharacterBase::CapsuleTrace()
{
	FHitResult x;
	return x;
}

void ACharacterBase::BeginPlay()
{

}

float ACharacterBase::TakeDamage(float Damage, FDamageEvent const& DamageEvent, AController* Eventinstigator, AActor* DamageCauser)
{
	return 0.0f;
}

void ACharacterBase::MoveForward(float Val)
{

}

void ACharacterBase::MoveRight(float Val)
{

}

void ACharacterBase::LookUp(float Val)
{

}

void ACharacterBase::Turn(float Val)
{

}

void ACharacterBase::UseAmmo()
{

}