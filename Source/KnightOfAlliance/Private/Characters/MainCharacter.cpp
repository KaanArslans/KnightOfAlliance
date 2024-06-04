// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/MainCharacter.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Items/Item.h"
#include "Components/AttributeComponent.h"
#include "Items/Weapon/Weapon.h"
#include "Animation/AnimMontage.h"
#include "Components/BoxComponent.h"
#include "HUD/MainHUD.h"
#include "HUD/MainOverlay.h"
#include "Items/Souls.h"

// Sets default values
AMainCharacter::AMainCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.f, 400.f, 0.f);

	GetMesh()->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	GetMesh()->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block);
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldDynamic, ECollisionResponse::ECR_Overlap);
	GetMesh()->SetGenerateOverlapEvents(true);

	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(GetRootComponent());
	CameraBoom->TargetArmLength = 300.f;

	ViewCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("ViewCamera"));
	ViewCamera->SetupAttachment(CameraBoom);
	
	linetraceLenght = 350;

}

// Called when the game starts or when spawned
void AMainCharacter::BeginPlay()
{
	Super::BeginPlay();
	Tags.Add(FName("EngageableTarget"));

	InitializeMainOverlay();
}

void AMainCharacter::InitializeMainOverlay()
{
	APlayerController* PlayerController = Cast<APlayerController>(GetController());
	if (PlayerController) {
		AMainHUD* MainHUD = Cast<AMainHUD>(PlayerController->GetHUD());
		if (MainHUD) {
			MainOverlay = MainHUD->GetMainOverlay();
			if (MainOverlay && Attributes) {
				MainOverlay->SetHealthBarPercent(Attributes->GetHealthPercent());
				MainOverlay->SetStaminaBarPercent(Attributes->GetHealthPercent());
				MainOverlay->SetGold(0);
				MainOverlay->SetSouls(0);
			}
		}
	}
}

void AMainCharacter::Tick(float DeltaTime)
{
	if (Attributes && MainOverlay) {
		Attributes->RegenStamina(DeltaTime);
		MainOverlay->SetStaminaBarPercent(Attributes->GetStaminaPercent());
	}
}

// Called to bind functionality to input
void AMainCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	PlayerInputComponent->BindAxis(FName("MoveForward"), this, &AMainCharacter::MoveForward);
	PlayerInputComponent->BindAxis(FName("MoveRight"), this, &AMainCharacter::MoveRight);
	PlayerInputComponent->BindAxis(FName("Turn"), this, &AMainCharacter::Turn);
	PlayerInputComponent->BindAxis(FName("LookUp"), this, &AMainCharacter::LookUp);
	PlayerInputComponent->BindAction(FName("Jump"), IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction(FName("Equip"), IE_Pressed, this, &AMainCharacter::EKeyPressed);
	PlayerInputComponent->BindAction(FName("Attack"), IE_Pressed, this, &AMainCharacter::Attack);
	PlayerInputComponent->BindAction(FName("Roll"), IE_Pressed, this, &AMainCharacter::Roll);
	PlayerInputComponent->BindAction(FName("Dodge"), IE_Pressed, this, &AMainCharacter::Dodge);
	PlayerInputComponent->BindAction("Interact", IE_Pressed, this, &AMainCharacter::take);
}

void AMainCharacter::take()
{

	//UE_LOG(LogTemp, Warning, TEXT("This is a debug message"));


	if (ViewCamera == nullptr) {
		return;

	}

	FHitResult HitResult;
	FVector Start = ViewCamera->GetComponentLocation();
	Start += ViewCamera->GetForwardVector() * 400.f;
	FVector End = Start + ViewCamera->GetForwardVector() * linetraceLenght;
	GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, ECollisionChannel::ECC_Visibility);
	DrawDebugLine(GetWorld(), Start, End, FColor::Red, false, 2.f);
	DrawDebugPoint(GetWorld(), End, 20.f, FColor::Red, false, 2.f);
	DrawDebugPoint(GetWorld(), Start, 20.f, FColor::Blue, false, 2.f);
	AActor* Actor = HitResult.GetActor();

	//ADoor* Door = Cast<ADoor>(HitResult.GetActor());
	//AKe* Ke = Cast<AKe>(HitResult.GetActor());
	//AItem* item = Cast<AItem>(HitResult.GetActor());

	/*if (Ke != nullptr) {
		Ke->Destroy();
		HasKey = true;
	}
	if (Door != nullptr && HasKey) {
		Door->OnInteract();

	}
	if (item != nullptr) {
		item->enable = !item->enable;

	}*/

	if (Actor != nullptr) {
		FString ActorName = Actor->GetName();
		GEngine->AddOnScreenDebugMessage(1, 90.f, FColor::Cyan, FString("Item interacted: ") + ActorName);

	}
	else {
		GEngine->AddOnScreenDebugMessage(1, 90.f, FColor::Cyan, FString("No actor interaction :="));
	}
	
		

	
	
}

void AMainCharacter::Jump()
{
	if (ActionState == EActionState::EAS_Unoccupied) {
		Super::Jump();
	}
}

float AMainCharacter::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	HandleDamage(DamageAmount);
	SetHealthPercentOnHUD();
	return DamageAmount;
}

void AMainCharacter::SetOverlappingItem(AItem* Item)
{
	OverlappingItem = Item;
}

void AMainCharacter::AddSouls(ASouls* Souls)
{
	if (Attributes && MainOverlay) {
		Attributes->AddSouls(Souls->GetSouls());
		MainOverlay->SetSouls(Attributes->GetSouls());
	}
}

void AMainCharacter::SetHealthPercentOnHUD()
{
	if (MainOverlay && Attributes) {
		MainOverlay->SetHealthBarPercent(Attributes->GetHealthPercent());
	}
}

void AMainCharacter::GetHit_Implementation(const FVector& ImpactPoint, AActor* Hitter)
{
	Super::GetHit_Implementation(ImpactPoint, Hitter);

	SetWeaponCollisionEnabled(ECollisionEnabled::NoCollision);
	if (Attributes && Attributes->GetHealthPercent() > 0.f)
	{
		ActionState = EActionState::EAS_HitReaction;
	}
}

void AMainCharacter::MoveForward(float Value)
{
	if (ActionState != EActionState::EAS_Unoccupied) return;
	if (Controller && (Value != 0.f)) {
		//forward direction
		const FRotator ControlRotation = GetControlRotation();
		const FRotator YawRotation(0.f, ControlRotation.Yaw, 0.f);

		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		AddMovementInput(Direction, Value);
	}
}

void AMainCharacter::MoveRight(float Value)
{
	if (ActionState != EActionState::EAS_Unoccupied) return;
	if (Controller && (Value != 0.f)) {
		//right direction
		const FRotator ControlRotation = GetControlRotation();
		const FRotator YawRotation(0.f, ControlRotation.Yaw, 0.f);

		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		AddMovementInput(Direction, Value);
	}
}

void AMainCharacter::Turn(float Value)
{
	AddControllerYawInput(Value);
}

void AMainCharacter::LookUp(float Value)
{
	AddControllerPitchInput(Value);
}

void AMainCharacter::EKeyPressed()
{
	AWeapon* OverlappingWeapon = Cast<AWeapon>(OverlappingItem);
	if (OverlappingWeapon) {
		EquipWeapon(OverlappingWeapon);
	}
	else {
		
		if (CanDisarm()) {
			PlayEquipMontage(FName("Unequip"));
			CharacterState = ECharacterState::ECS_Unequipped;
			ActionState = EActionState::EAS_EquippingWeapon;
		}
		else if (CanArm()) {
			PlayEquipMontage(FName("Equip"));
			CharacterState = ECharacterState::ECS_EquippedOneHandedWeapon;
			ActionState = EActionState::EAS_EquippingWeapon;
		}

	}
}

void AMainCharacter::Attack()
{
	Super::Attack();
	if (CanAttack()) {
		PlayAttackMontage();
		ActionState = EActionState::EAS_Attacking;
	}
}

void AMainCharacter::Roll()
{
	if (ActionState != EActionState::EAS_Unoccupied || !CheckStamina()) return;
	PlayRollMontage();
	ActionState = EActionState::EAS_Rolling;
	if (Attributes && MainOverlay) {
		Attributes->ConsumeStamina(Attributes->GetRollCost());
		MainOverlay->SetStaminaBarPercent(Attributes->GetStaminaPercent());
	}
}

void AMainCharacter::Dodge()
{
	/*
	if (ActionState != EActionState::EAS_Unoccupied || !CheckStamina()) return;
	PlayDodgeMontage();
	ActionState = EActionState::EAS_Rolling;
	if (Attributes && MainOverlay) {
		Attributes->ConsumeStamina(Attributes->GetRollCost());
		MainOverlay->SetStaminaBarPercent(Attributes->GetStaminaPercent());
	}
	*/
}

bool AMainCharacter::CheckStamina()
{
	return Attributes && Attributes->GetStamina() > Attributes->GetRollCost();
}

/*void AMainCharacter::Die_Implementation()
{
	Super::Die_Implementation();

	ActionState = EActionState::EAS_Dead;
	DisableMeshCollision();
}*/

bool AMainCharacter::CanAttack()
{
	return ActionState == EActionState::EAS_Unoccupied && CharacterState != ECharacterState::ECS_Unequipped;
}

bool AMainCharacter::CanDisarm()
{
	return ActionState == EActionState::EAS_Unoccupied && CharacterState != ECharacterState::ECS_Unequipped;
}

bool AMainCharacter::CanArm()
{
	return ActionState == EActionState::EAS_Unoccupied && CharacterState == ECharacterState::ECS_Unequipped
		&& EquippedWeapon;
}

void AMainCharacter::Die()
{
	Super::Die();
	ActionState = EActionState::EAS_Dead;
	DisableMeshCollision();
}

void AMainCharacter::Disarm()
{
	if (EquippedWeapon)
	{
		EquippedWeapon->AttachMeshToSocket(GetMesh(), FName("SpineSocket"));
	}
}

void AMainCharacter::Arm()
{
	if (EquippedWeapon)
	{
		EquippedWeapon->AttachMeshToSocket(GetMesh(), FName("RightHandSocket"));
	}
}

void AMainCharacter::FinishEquipping()
{
	ActionState = EActionState::EAS_Unoccupied;
}

void AMainCharacter::HitReactEnd()
{
	ActionState = EActionState::EAS_Unoccupied;
}

void AMainCharacter::PlayEquipMontage(const FName& SectionName)
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && EquipMontage) {
		AnimInstance->Montage_Play(EquipMontage);
		AnimInstance->Montage_JumpToSection(SectionName, EquipMontage);
	}
}

void AMainCharacter::EquipWeapon(AWeapon* Weapon)
{
	Weapon->Equip(GetMesh(), FName("RightHandSocket"), this, this);
	//Weapon->Equip(GetMesh(), FName("RightHandSocket"), this, this);
	CharacterState = ECharacterState::ECS_EquippedOneHandedWeapon;
	OverlappingItem = nullptr;
	EquippedWeapon = Weapon;
}

void AMainCharacter::AttackEnd()
{
	ActionState = EActionState::EAS_Unoccupied;
}

void AMainCharacter::RollEnd()
{
	Super::RollEnd();
	ActionState = EActionState::EAS_Unoccupied;
}
