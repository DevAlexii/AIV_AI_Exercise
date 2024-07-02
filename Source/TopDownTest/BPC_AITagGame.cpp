// Fill out your copyright notice in the Description page of Project Settings.


#include "BPC_AITagGame.h"
#include "BP_AITagGame.h"
#include "EngineUtils.h"
#include "TopDownTestGameMode.h"
#include "Navigation/CrowdFollowingComponent.h"

ABPC_AITagGame::ABPC_AITagGame(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer.SetDefaultSubobjectClass<UCrowdFollowingComponent>(TEXT("PathFollowingComponent")))
{
	Cast<UCrowdFollowingComponent>(GetPathFollowingComponent())->SetCrowdAvoidanceQuality(ECrowdAvoidanceQuality::High);
}


void ABPC_AITagGame::OnPossess(APawn* InPawn)
{
	PawnSquadNum = Cast<ABP_AITagGame>(InPawn)->SquadNum;
	PawnGridPosition = Cast<ABP_AITagGame>(InPawn)->GridPosition;
	Super::OnPossess(InPawn);
	StartPosition = InPawn->GetActorLocation();
	CatchBall = MakeShared<FAivState>(
		[this]() {
			BallInstance = Cast<ATopDownTestGameMode>(GetWorld()->GetAuthGameMode())->GetBall();
			if (BallInstance)
			{
				MoveToLocation(BallInstance->GetActorLocation());
			}
		},
		nullptr,
		[this](const float DeltaTime)->TSharedPtr<FAivState> {
			if (!BallInstance)
			{
				return CatchBall;
			}

			if (BallInstance->GetAttachParentActor())
			{
				return Reset;
			}

			const float Distance = FVector::Distance(BallInstance->GetActorLocation(), GetPawn()->GetActorLocation());
			if (Distance < 150)
			{
				if (BallInstance->GetAttachParentActor())
				{
					return Reset;
				}

				BallInstance->AttachToActor(GetPawn(), FAttachmentTransformRules::KeepRelativeTransform);
				BallInstance->SetActorRelativeLocation(FVector::ZeroVector);
				return BringToPlayer;
			}

			return nullptr;
		});

	Reset = MakeShared<FAivState>(
		[this]() {
			MoveToLocation(StartPosition);
		},
		nullptr,
		[this](const float DeltaTime)->TSharedPtr<FAivState> {

			float Distance = FVector::Distance(GetPawn()->GetActorLocation(), StartPosition);
			if (Distance < 50)
			{
				int NewGridPosition;
				if (Cast<ATopDownTestGameMode>(GetWorld()->GetAuthGameMode())->Restart(NewGridPosition))
				{
					if (NewGridPosition == PawnGridPosition)
					{
						return CatchBall;
					}
				}
				return EmptyState;
			}
			return nullptr;
		});

	EmptyState = MakeShared<FAivState>(
		nullptr,
		nullptr,
		nullptr);

	BringToPlayer = MakeShared<FAivState>(
		[this]() {
			MoveToActor(GetWorld()->GetFirstPlayerController()->GetPawn());
		},
		nullptr,
		[this](const float DeltaTime)->TSharedPtr<FAivState> {

			float Distance = FVector::Distance(GetPawn()->GetActorLocation(), GetWorld()->GetFirstPlayerController()->GetPawn()->GetActorLocation());
			if (Distance < 90)
			{
				BallInstance->SetActorHiddenInGame(true);
				BallInstance = nullptr;
				Cast<ATopDownTestGameMode>(GetWorld()->GetAuthGameMode())->GivePoints(PawnSquadNum);
				return Reset;
			}
			return nullptr;
		});


	CurrentState = EmptyState;
	if (CurrentState)
	{
		CurrentState->CallEnter();
	}
}


void ABPC_AITagGame::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (CurrentState)
	{
		CurrentState = CurrentState->CallTick(DeltaTime);
	}
}


void ABPC_AITagGame::Restart(int32 GridPosition)
{
	if (GridPosition == PawnGridPosition)
	{
		CurrentState = CatchBall;

		if (CurrentState)
		{
			CurrentState->CallEnter();
		}
	}
}