// Copyright Epic Games, Inc. All Rights Reserved.

#include "TopDownTestGameMode.h"
#include "TopDownTestCharacter.h"
#include "UObject/ConstructorHelpers.h"
#include "EngineUtils.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "BPC_AITagGame.h"



ATopDownTestGameMode::ATopDownTestGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPerson/Blueprints/BP_ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
	NumPlayerPerSquad = 2;
}

void ATopDownTestGameMode::BeginPlay()
{
	Super::BeginPlay();

	SpawnPoints.Empty();
	for (TActorIterator<ATargetPoint>It(GetWorld()); It; ++It)
	{
		SpawnPoints.Add(*It);
	}

	if (SpawnPoints.Num() < 3)
	{
		UE_LOG(LogTemp, Error, TEXT("Not Enough Points to start game"));
		UKismetSystemLibrary::QuitGame(GetWorld(), GetWorld()->GetFirstPlayerController(), EQuitPreference::Quit, true);
	}
	if (!AIClass)
	{
		UE_LOG(LogTemp, Error, TEXT("AIClass is Null"));
		UKismetSystemLibrary::QuitGame(GetWorld(), GetWorld()->GetFirstPlayerController(), EQuitPreference::Quit, true);
	}
	if (!BallClass)
	{
		UE_LOG(LogTemp, Error, TEXT("AIClass is Null"));
		UKismetSystemLibrary::QuitGame(GetWorld(), GetWorld()->GetFirstPlayerController(), EQuitPreference::Quit, true);
	}

	BallInstance = GetWorld()->SpawnActor<AActor>(BallClass, SpawnPoints[SpawnPoints.Num() - 1]->GetActorLocation(), FRotator::ZeroRotator);

	SquadPoints.Init(0, 2);

	for (int i = 0; i < SpawnPoints.Num() - 1; i++) // -1 Cause need only first two target point to spawn the last one is for the ball
	{
		for (int k = 0; k < NumPlayerPerSquad; k++)
		{
			const FVector SpawnLocation = SpawnPoints[i]->GetActorLocation() + FVector(-k * 100,0, 0);

			ABP_AITagGame* SpawnedActor = GetWorld()->SpawnActorDeferred<ABP_AITagGame>(AIClass, FTransform(FRotator{ 0,90,0 }, SpawnLocation));

			SpawnedActor->Initialize(i,k);

			UGameplayStatics::FinishSpawningActor(SpawnedActor, FTransform(FRotator{ 0,90,0 }, SpawnLocation));
			NPCs.Add(SpawnedActor);
		}
	}
	int StartGame;
	NPCsInBase = 2;
	Restart(StartGame);
}

AActor* ATopDownTestGameMode::GetBall() const
{
	return BallInstance;
}

bool ATopDownTestGameMode::Restart(int& NewGridPosition)
{
	NPCsInBase++;
	if (NPCsInBase >= 2)
	{
		BallInstance->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
		BallInstance->SetActorLocation(SpawnPoints[SpawnPoints.Num() - 1]->GetActorLocation());
		BallInstance->SetActorHiddenInGame(false);

		NewGridPosition = FMath::RandRange(0, NumPlayerPerSquad - 1);
		UpdateUIGridPosition.Broadcast(NewGridPosition + 1);

		for (APawn* AI : NPCs)
		{
			Cast<ABPC_AITagGame>(AI->GetController())->Restart(NewGridPosition);
		}
		NPCsInBase = 0;
		return true;
	}

	return false;
}

void ATopDownTestGameMode::GivePoints(const int SquadNum)
{
	SquadPoints[SquadNum]++;
	UpdateUIPoints.Broadcast(SquadNum,SquadPoints[SquadNum]);
}
