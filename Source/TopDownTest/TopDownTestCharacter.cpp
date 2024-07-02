// Copyright Epic Games, Inc. All Rights Reserved.

#include "TopDownTestCharacter.h"
#include "Engine/LocalPlayer.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/Controller.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"
#include "Kismet/GameplayStatics.h"
#include "AIController.h"


DEFINE_LOG_CATEGORY(LogTemplateCharacter);

//////////////////////////////////////////////////////////////////////////
// ATopDownTestCharacter

ATopDownTestCharacter::ATopDownTestCharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f); // ...at this rotation rate

	// Note: For faster iteration times these variables, and many more, can be tweaked in the Character Blueprint
	// instead of recompiling to adjust them
	GetCharacterMovement()->JumpZVelocity = 700.f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = 500.f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;
	GetCharacterMovement()->BrakingDecelerationFalling = 1500.0f;

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 400.0f; // The camera follows at this distance behind the character	
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named ThirdPersonCharacter (to avoid direct content references in C++)
}

void ATopDownTestCharacter::BeginPlay()
{
	// Call the base class  
	Super::BeginPlay();
	UGameplayStatics::GetPlayerController(GetWorld(), 0)->SetShowMouseCursor(true);
}

//////////////////////////////////////////////////////////////////////////
// Input

void ATopDownTestCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	// Add Input Mapping Context
	if (APlayerController* PlayerController = Cast<APlayerController>(GetController()))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}

	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent)) {

		// MoveCamera
		EnhancedInputComponent->BindAction(MoveCamera, ETriggerEvent::Triggered, this, &ATopDownTestCharacter::RotateCamera);
		// MouseClick
		EnhancedInputComponent->BindAction(Click, ETriggerEvent::Completed, this, &ATopDownTestCharacter::OnClick);
	}
	else
	{
		UE_LOG(LogTemplateCharacter, Error, TEXT("'%s' Failed to find an Enhanced Input component! This template is built to use the Enhanced Input system. If you intend to use the legacy system, then you will need to update this C++ file."), *GetNameSafe(this));
	}
}

void ATopDownTestCharacter::RotateCamera()
{
	FVector2D LookAxisVector;
	UGameplayStatics::GetPlayerController(GetWorld(), 0)->GetInputMouseDelta(LookAxisVector.X, LookAxisVector.Y);

	AddControllerYawInput(LookAxisVector.X);
	AddControllerPitchInput(-LookAxisVector.Y);
}


#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "NavigationSystem.h"
#include "NavigationPath.h"

void ATopDownTestCharacter::OnClick()
{

	APlayerController* PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	FVector MousePos;
	PlayerController->GetMousePosition(MousePos.X, MousePos.Y);

	FVector WorldLocation;
	FVector WorldDirection;
	PlayerController->DeprojectScreenPositionToWorld(MousePos.X, MousePos.Y, WorldLocation, WorldDirection);

	FHitResult OutHit;
	if (GetWorld()->LineTraceSingleByChannel(OutHit, WorldLocation, WorldLocation + WorldDirection * 10000, ECollisionChannel::ECC_Visibility))
	{
		const FVector Destination = OutHit.ImpactPoint;
		UAIBlueprintHelperLibrary::SimpleMoveToLocation(PlayerController, Destination);
		UNavigationPath* Path = UAIBlueprintHelperLibrary::GetCurrentPath(PlayerController);

		/*FNavLocation NavLoc;
		const UNavigationSystemV1* NavSystem = UNavigationSystemV1::GetCurrent(GetWorld());
		if (NavSystem && NavSystem->GetRandomReachablePointInRadius(Destination, 100, NavLoc)) {*/
			UAIBlueprintHelperLibrary::SimpleMoveToLocation(PlayerController, Destination);

			if (DecalInstance)
			{
				DecalInstance->Destroy();
			}
			DecalInstance = GetWorld()->SpawnActor<AActor>(Marker, Destination, FRotator::ZeroRotator);
		//}
		/*LastDestination = Destination;

		if (Path && !Path->IsPartial()) {
			if (DecalInstance)
			{
				DecalInstance->Destroy();
			}
			DecalInstance = GetWorld()->SpawnActor<AActor>(Marker, Destination, FRotator::ZeroRotator);
			LastDestination = Destination;
		}
		else
		{
			UAIBlueprintHelperLibrary::SimpleMoveToLocation(PlayerController, LastDestination);
		}*/
	}
}
