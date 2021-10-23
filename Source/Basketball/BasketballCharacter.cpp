// Fill out your copyright notice in the Description page of Project Settings.


#include "BasketballCharacter.h"

// Sets default values
ABasketballCharacter::ABasketballCharacter()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//if (!CollisionComponent)
	//{
		// Use a sphere as a simple collision representation.
	//	CollisionComponent = CreateDefaultSubobject<USphereComponent>(TEXT("RootComponent"));
		// Set the sphere's collision profile name to "Projectile".
	//	CollisionComponent->BodyInstance.SetCollisionProfileName(TEXT("Pawn"));
		// Event called when component hits something.
	//	CollisionComponent->OnComponentBeginOverlap.AddDynamic(this, &ABasketballCharacter::OnOverlapBegin);
		//CollisionComponent->OnComponent.AddDynamic(this, &ABasketballCharacter::OnOverlapBegin);
		// Set the sphere's collision radius.
	//	CollisionComponent->InitSphereRadius(40.0f);
		// Set the root component to be the collision component.
	//	RootComponent = CollisionComponent;
	//}

	// --- Configure First-Person Camera ---

	// Create a first person camera component.
	BasketballCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
	check(BasketballCameraComponent != nullptr);

	// Attach the camera component to out capule component.
	BasketballCameraComponent->SetupAttachment(CastChecked<USceneComponent, UCapsuleComponent>(GetCapsuleComponent()));

	// Position the camera slightly above the eyes.
	BasketballCameraComponent->SetRelativeLocation(FVector(0.0f, 0.0f, 50.0f + BaseEyeHeight));

	// Enable the pawn to control the camera rotation.
	BasketballCameraComponent->bUsePawnControlRotation = true;

	// --- Configure First-Person Mesh ---

	// Create a first person mesh component for the owning player.
	BasketballMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("FirstPersonMesh"));
	check(BasketballMesh != nullptr);

	// Only the owning player sees this mesh.
	BasketballMesh->SetOnlyOwnerSee(true);

	// Attach the BasketballMesh (FPS mesh) to the BasketballCameraComponent (FPS camera).
	BasketballMesh->SetupAttachment(BasketballCameraComponent);

	// Disable some enviromental shadows to preserve the illusion of having a single mesh.
	BasketballMesh->bCastDynamicShadow = false;
	BasketballMesh->CastShadow = false;

	//SetActorEnableCollision(true);
	// The owning player doesn't see the regular (third-person) body mesh.
	GetMesh()->SetOwnerNoSee(true);
}

// Called when the game starts or when spawned
void ABasketballCharacter::BeginPlay()
{
	Super::BeginPlay();
	check(GEngine != nullptr);

	// Display a debug message for five seconds.
	// The -1 "Key" value argument prevents the message from being updated or refreshed.
	//GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT("We are using BasketballCharacter."));
	isHoldingBall = 1;



}

// Called every frame
void ABasketballCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);



}

// Called to bind functionality to input
void ABasketballCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	// Set up "movement" bindings.
	PlayerInputComponent->BindAxis("MoveForward", this, &ABasketballCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &ABasketballCharacter::MoveRight);

	// Set up "look" bindings.
	PlayerInputComponent->BindAxis("Turn", this, &ABasketballCharacter::AddControllerYawInput);
	PlayerInputComponent->BindAxis("LookUp", this, &ABasketballCharacter::AddControllerPitchInput);

	// Set up "action" bindings.
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ABasketballCharacter::StartJump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ABasketballCharacter::StopJump);

	// Set up "fire" binding.
	PlayerInputComponent->BindAction("Fire", IE_Pressed, this, &ABasketballCharacter::Fire);
}

void ABasketballCharacter::MoveForward(float Value)
{
	// Find out which way is "forward" and record that the player wants to move that way.
	FVector Direction = FRotationMatrix(Controller->GetControlRotation()).GetScaledAxis(EAxis::X);
	AddMovementInput(Direction, Value);

}

void ABasketballCharacter::MoveRight(float Value)
{
	// Find out which way is "right" and record that the player wants to move that way.
	FVector Direction = FRotationMatrix(Controller->GetControlRotation()).GetScaledAxis(EAxis::Y);
	AddMovementInput(Direction, Value);

}

void ABasketballCharacter::StartJump()
{
	bPressedJump = true;
}

void ABasketballCharacter::StopJump()
{
	bPressedJump = false;
}

void ABasketballCharacter::Fire()
{
	// Attempt to fire a projectile.
	
	if (ProjectileClass && isHoldingBall > 0)
	{
		
		// Get the camera transform.
		FVector CameraLocation;
		FRotator CameraRotation;
		GetActorEyesViewPoint(CameraLocation, CameraRotation);


		// Set MuzzleOffset to spawn projectiles slightly in front of the camera.
		MuzzleOffset.Set(150.0f, 0.0f, 0.0f);

		// Transform MuzzleOffset from camera space to world space.
		FVector MuzzleLocation = CameraLocation + FTransform(CameraRotation).TransformVector(MuzzleOffset);

		// Skew the aim to be slightly upwards.
		FRotator MuzzleRotation = CameraRotation;
		MuzzleRotation.Pitch += 10.0f;

		UWorld* World = GetWorld();
		if (World)
		{
			FActorSpawnParameters SpawnParams;
			SpawnParams.Owner = this;
			SpawnParams.Instigator = GetInstigator();
			//AFPSProjectile* Projectile;
			if (isHoldingBall == 1) {
				
				// Spawn the projectile at the muzzle.
				AFPSProjectile* Projectile = World->SpawnActor<AFPSProjectile>(ProjectileClass, MuzzleLocation, MuzzleRotation, SpawnParams);
				if (Projectile)
				{
					// Set the projectile's initial trajectory.
					FVector LaunchDirection = MuzzleRotation.Vector();
					Projectile->FireInDirection(LaunchDirection);
				}
			}
			else if (isHoldingBall == 2) {
				// Spawn the projectile at the muzzle.
				
				GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT("Firing proj2"));
				
				AFPSProjectile3* Projectile3 = World->SpawnActor<AFPSProjectile3>(AFPSProjectile3::StaticClass(), MuzzleLocation, MuzzleRotation, SpawnParams);
				
				if (Projectile3)
				{
					
					// Set the projectile's initial trajectory.
					FVector LaunchDirection = MuzzleRotation.Vector();
					
					Projectile3->FireInDirection(LaunchDirection);
				}
			}
			/*
			// Spawn the projectile at the muzzle.
			AFPSProjectile* Projectile = World->SpawnActor<AFPSProjectile>(ProjectileClass, MuzzleLocation, MuzzleRotation, SpawnParams);
			if (Projectile)
			{
				// Set the projectile's initial trajectory.
				FVector LaunchDirection = MuzzleRotation.Vector();
				Projectile->FireInDirection(LaunchDirection);
			}
			*/
			
		}
		isHoldingBall = 0;
	}
}

//void ABasketballCharacter::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) {
//	if (OtherActor != this && OtherComp->IsSimulatingPhysics())
//	{
//		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, OtherActor->GetName());
//	}

//}
