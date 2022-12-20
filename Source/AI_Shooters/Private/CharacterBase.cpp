// Fill out your copyright notice in the Description page of Project Settings.


#include "CharacterBase.h"
#include "Kismet/GameplayStatics.h"
#include "Components/CapsuleComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "GameFramework/CharacterMovementComponent.h"

ACharacterBase::ACharacterBase()
{
	// Set this character to call Tick() every frame
	PrimaryActorTick.bCanEverTick = false;

	bUseControllerRotationYaw = false;
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;

	Weapon = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Weapon"));
}

FHitResult ACharacterBase::TraceProvider(FVector Start, FVector End)
{
	FHitResult OutHit;
	FCollisionQueryParams CollisionParams;
	CollisionParams.AddIgnoredActor(this);

	GetWorld()->LineTraceSingleByChannel(OutHit, Start, End, ECC_Visibility, CollisionParams);

	return OutHit;
}

void ACharacterBase::ToggleCombat(const bool Newbool)
{
	GetMesh()->GetAnimInstance()->StopAllMontages(0.2f);
	AnimValues.bIsInCombat = Newbool;
	bUseControllerRotationYaw = Newbool;
	GetCharacterMovement()->bOrientRotationToMovement = !Newbool;
	FName NewSocket = Newbool ? "hand_rSocket" : "spine_03Socket";
	AttachWeapon(Weapon, NewSocket);
	GetCharacterMovement()->MaxWalkSpeed = (Newbool) ? 187.0f : 94.0f;
}

void ACharacterBase::ToggleCrouch(const bool Newbool)
{
	AnimValues.bIsCrouching = Newbool;
	const float Speed = AnimValues.bIsInCombat ? 187.0f : WalkSpeed;
	GetCharacterMovement()->MaxWalkSpeed = (Newbool) ? CrouchedWalkSpeed : Speed;
}

void ACharacterBase::ToggleADS(const bool Newbool)
{
	AnimValues.bADS = Newbool;
}

void ACharacterBase::ToggleSprinting(bool Newbool)
{
	if (Newbool)
	{
		GetCharacterMovement()->MaxWalkSpeed = SprintSpeed;
		return;
	}

	GetCharacterMovement()->MaxWalkSpeed = (AnimValues.bIsInCombat) ? 187.0f : WalkSpeed;
}

void ACharacterBase::AttachWeapon(USkeletalMeshComponent* WeaponMesh, const FName SocketName)
{
	if (WeaponMesh) { WeaponMesh->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, SocketName); }
}

void ACharacterBase::StartWeaponFire()
{
	if (GetMesh()->GetAnimInstance()->IsAnyMontagePlaying()) { return; }

	if (!AnimValues.bIsInCombat) { return; }

	ToggleADS(true);
	ToggleSprinting(false);
	AnimValues.bIsShooting = true;
	SpawnMuzzleFlash();

	FVector EyesLoc;
	FRotator EyesRot;
	GetController()->GetPlayerViewPoint(EyesLoc, EyesRot);

	const FVector Spread = (FVector(
		UKismetMathLibrary::RandomFloatInRange(-1.0f, 1.0f),
		UKismetMathLibrary::RandomFloatInRange(-1.0f, 1.0f),
		UKismetMathLibrary::RandomFloatInRange(-1.0f, 1.0f)
	));

	const FVector End = (BulletSpread) ? (EyesRot.Vector() * 2000.0f) + EyesLoc * Spread : (EyesRot.Vector() * 2000.0f) + EyesLoc;

	FHitResult HitInfo = TraceProvider(EyesLoc, End);
	UseAmmo();

	if (!HitInfo.bBlockingHit) { return; }

	UGameplayStatics::SpawnEmitterAtLocation(this, BulletImpact, HitInfo.Location, FRotator(0.0f, 0.0f, 0.0f), true);
	UGameplayStatics::ApplyPointDamage(HitInfo.GetActor(), BaseDamage, HitInfo.ImpactPoint, HitInfo, this->GetController(), this, nullptr);

	if (FireHandle.IsValid()) { return; }

	GetWorldTimerManager().SetTimer(FireHandle, this, &ACharacterBase::StartWeaponFire, FireRate, true, 0.0f);
}

void ACharacterBase::SpawnMuzzleFlash()
{
	UGameplayStatics::SpawnEmitterAttached(MuzzleFlash, Weapon, "MuzzleFlash");
}

void ACharacterBase::StopWeaponFire()
{
	GetWorldTimerManager().ClearTimer(FireHandle);
	AnimValues.bIsShooting = false;
}

void ACharacterBase::PlayMontage(UAnimMontage* Montage, float Rate)
{
	if (!GetMesh()->GetAnimInstance()->IsAnyMontagePlaying()) {
		GetMesh()->GetAnimInstance()->Montage_Play(Montage, Rate);
	}
}

UAnimMontage* ACharacterBase::ReturnRandomVariation()
{
	int i = UKismetMathLibrary::RandomIntegerInRange(0, IdleVariations.Num() - 1);
	return IdleVariations[i];
}

void ACharacterBase::ToggleSitting(bool Newbool)
{
	GetCharacterMovement()->StopMovementImmediately();
	AnimValues.bIsSitting = Newbool;
}

void ACharacterBase::SetCollision(bool Newbool)
{
	if (Newbool)
	{
		GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
		return;
	}
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
}

bool ACharacterBase::IsHostile(ACharacterBase* Agent)
{
	return false;
}

void ACharacterBase::MakeANoise(FVector Location)
{
	MakeNoise(1.0f, nullptr, Location, 0.0f, "");
}

FHitResult ACharacterBase::CapsuleTrace()
{
	FHitResult OutHit;
	TArray<AActor*> ActorsToIgnore;
	ActorsToIgnore.AddUnique(this);

	FVector EyesLoc;
	FRotator EyesRot;
	GetController()->GetPlayerViewPoint(EyesLoc, EyesRot);

	const FVector End = (EyesRot.Vector() * 2000.0f) + EyesLoc + FVector(0.0f, 0.0f, 120.0f);

	UKismetSystemLibrary::SphereTraceSingle(GetWorld(), EyesLoc, End, 20.0f, ETraceTypeQuery::TraceTypeQuery_MAX, false, ActorsToIgnore, EDrawDebugTrace::None, OutHit, true, FColor::Green);
	return OutHit;
}

void ACharacterBase::BeginPlay()
{
	Super::BeginPlay();
	Weapon->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, "spine_03Socket");
	UpdateWidgetRef();
}

float ACharacterBase::TakeDamage(float Damage, FDamageEvent const& DamageEvent, AController* Eventinstigator, AActor* DamageCauser)
{
	Super::TakeDamage(Damage, DamageEvent, Eventinstigator, DamageCauser);
	
	UGameplayStatics::SpawnEmitterAtLocation(this, BloodFX, GetActorLocation());

	if (Health <= 0.0f) { return 0.0f; }

	FHitResult HitInfo;
	FVector ImpulseDir;
	DamageEvent.GetBestHitInfo(this, DamageCauser, HitInfo, ImpulseDir);

	ACharacterBase* Chr = Cast<ACharacterBase>(HitInfo.GetActor());
	if (!HitInfo.bBlockingHit || Chr == nullptr) { return 0.0f; }

	Chr->Health = (HitInfo.BoneName == "head") ? 0.0f : Health - Damage;

	if (Chr->Health <= 0.0f)
	{
		PlayMontage(DeathMontage, 1.0f);
		GetMesh()->bIgnoreRadialForce = true;
		PlayMontage(DeathMontage, 2.0f);
		GetMesh()->SetSimulatePhysics(true);
		GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		Dead = true;
		StopWeaponFire();
		GetCharacterMovement()->bUseRVOAvoidance = false;

		return 0.0f;
	}

	PlayMontage(HitReactMontage, 1.0f);
	return 0.0f;
}

void ACharacterBase::MoveForward(float Val)
{
	if ((Controller == NULL) || (Val == 0.0f)) { return; }

	const FRotator Rotation = Controller->GetControlRotation();
	const FRotator YawRotation(0, Rotation.Yaw, 0);
	const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	AddMovementInput(Direction, Val);
}

void ACharacterBase::MoveRight(float Val)
{
	if ((Controller == NULL) || (Val == 0.0f)) { return; }

	const FRotator Rotation = Controller->GetControlRotation();
	const FRotator YawRotation(0, Rotation.Yaw, 0);
	const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
	AddMovementInput(Direction, Val);
}

void ACharacterBase::LookUp(float Val)
{
	if (Val == 0.0f) { return; }

	AddControllerPitchInput(Val);
}

void ACharacterBase::Turn(float Val)
{
	if (Val == 0.0f) { return; }

	AddControllerYawInput(Val);
}

void ACharacterBase::UseAmmo()
{
	++ShotsFired;
	if (ShotsFired >= MagSize)
	{
		PlayMontage(ReloadMontage, 1.5);
		ShotsFired = 0;
	}
}