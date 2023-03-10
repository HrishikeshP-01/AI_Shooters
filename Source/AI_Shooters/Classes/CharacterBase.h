// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Perception/AISightTargetInterface.h"
#include "GameFramework/Character.h"
#include "CharacterBase.generated.h"

/**
 * 
 */
USTRUCT(BlueprintType)
struct FAnimValues
{
	GENERATED_BODY()
public:
	UPROPERTY(BlueprintReadWrite, Category = Movement)
		bool bIsCrouching = false;
	UPROPERTY(BlueprintReadWrite, Category = Movement)
		bool bIsInCombat = false;
	UPROPERTY(BlueprintReadWrite, Category = Movement)
		bool bIsShooting = false;
	UPROPERTY(BlueprintReadWrite, Category = Movement)
		bool bADS = false;
	UPROPERTY(BlueprintReadWrite, Category = Movement)
		bool bIsSitting = false;
};

UCLASS()
class AI_SHOOTERS_API ACharacterBase :public ACharacter//, public IAISightTargetInterface
{
	GENERATED_BODY()
public:
	// Sets the default values for this character's properties
	ACharacterBase();
	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "Components")
		class USkeletalMeshComponent* Weapon;
	FHitResult TraceProvider(FVector Start, FVector End);
	UPROPERTY(BlueprintReadOnly, Category = "AI")
		bool Dead = false;
	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "AI")
		FName PerceptionTarget = "spine_02";
	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "AI")
		float Health = 100.0f;
	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "AI")
		float BaseDamage = 0.01f;
	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "AI")
		float FireRate = 0.1f;
	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "AI")
		bool BulletSpread = true;
	UPROPERTY(BlueprintReadOnly, Category = "Animation")
		FAnimValues AnimValues;
	UFUNCTION(BlueprintCallable)
		void ToggleCombat(const bool Newbool);
	UFUNCTION(BlueprintCallable)
		void ToggleCrouch(const bool Newbool);
	UFUNCTION(BlueprintCallable)
		void ToggleADS(const bool Newbool);
	UFUNCTION(BlueprintCallable)
		void ToggleSprinting(bool Newbool);
	UFUNCTION(BlueprintCallable)
		void AttachWeapon(USkeletalMeshComponent* WeaponMesh, const FName SocketName);
	FTimerHandle FireHandle;
	UFUNCTION(BlueprintCallable)
		void StartWeaponFire();
	void SpawnMuzzleFlash();
	UFUNCTION(BlueprintCallable)
		void StopWeaponFire();
	UFUNCTION(BlueprintCallable)
		void PlayMontage(UAnimMontage* Montage, float Rate);
	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "Components")
		TArray<UAnimMontage*> IdleVariations;
	UFUNCTION(BlueprintCallable)
		UAnimMontage* ReturnRandomVariation();
	UFUNCTION(BlueprintCallable)
		void ToggleSitting(bool Newbool);
	UFUNCTION(BlueprintCallable)
		void SetCollision(bool Newbool);
	bool IsHostile(ACharacterBase* Agent);
	UFUNCTION(BlueprintImplementableEvent)
		void UpdateWidgetRef();
	UFUNCTION(BlueprintImplementableEvent)
		void UpdateWidgetVis(bool Newbool);
	UFUNCTION(BlueprintCallable)
		void MakeANoise(FVector Location);
	UFUNCTION()
		FHitResult CapsuleTrace();
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
		UAnimMontage* ReloadMontage;
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual float TakeDamage(float Damage, FDamageEvent const& DamageEvent, AController* Eventinstigator, AActor* DamageCauser)override;
	UFUNCTION(BlueprintCallable)
		void MoveForward(float Val);
	UFUNCTION(BlueprintCallable)
		void MoveRight(float Val);
	UFUNCTION(BlueprintCallable)
		void LookUp(float Val);
	UFUNCTION(BlueprintCallable)
		void Turn(float Val);
	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "Components")
		UAnimMontage* DeathMontage;
	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "Components")
		UAnimMontage* HitReactMontage;
	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "Components")
		UParticleSystem* BloodFX;
	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "Components")
		UParticleSystem* MuzzleFlash;
	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "Components")
		UParticleSystem* BulletImpact;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Movement")
		float SlowWalkSpeed = 94.5f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Movement")
		float SprintSpeed = 375.0f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Movement")
		float WalkSpeed = 110.0f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Movement")
		float CrouchedWalkSpeed = 100.0f;
	UFUNCTION(BlueprintCallable)
		void UseAmmo();
	int32 ShotsFired = 0;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Weapon")
		int32 MagSize = 20;
};