// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "BPC_AITagGame.generated.h"


struct FAivState : public TSharedFromThis<FAivState>
{
private:
	TFunction<void()> Enter;
	TFunction<void()> Exit;
	TFunction<TSharedPtr<FAivState>(const float)> Tick;

public:
	FAivState()
	{
		Enter = nullptr;
		Exit = nullptr;
		Tick = nullptr;
	}


	FAivState(TFunction<void()> InEnter = nullptr,
		TFunction<void()> InExit = nullptr,
		TFunction<TSharedPtr<FAivState>(const float)> InTick = nullptr)
	{
		Enter = InEnter;
		Exit = InExit;
		Tick = InTick;
	}

	FAivState(const FAivState& Other) = delete;
	FAivState& operator=(const FAivState& Other) = delete;
	FAivState(FAivState&& Other) = delete;
	FAivState& operator=(FAivState&& Other) = delete;


	void CallEnter()
	{
		if (Enter)
		{
			Enter();
		}
	}

	void CallExit()
	{
		if (Exit)
		{
			Exit();
		}
	}

	TSharedPtr<FAivState> CallTick(const float DeltaTime)
	{
		if (Tick)
		{
			TSharedPtr<FAivState> NewState = Tick(DeltaTime);

			if (NewState != nullptr && NewState != AsShared())
			{
				CallExit();
				NewState->CallEnter();
				return NewState;
			}
		}
		return AsShared();
	}
};


/**
 *
 */
UCLASS()
class TOPDOWNTEST_API ABPC_AITagGame : public AAIController
{
	GENERATED_BODY()


protected:
	TSharedPtr<FAivState> CurrentState;
	TSharedPtr<FAivState> CatchBall;
	TSharedPtr<FAivState> BringToPlayer;
	TSharedPtr<FAivState> Reset;
	TSharedPtr<FAivState> EmptyState;

	ABPC_AITagGame(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
	void OnPossess(APawn* InPawn) override;
	void Tick(float DeltaTime) override;

	FVector StartPosition;
	AActor* BallInstance;
	int PawnSquadNum;
	int PawnGridPosition;

public:
	void Restart(int32 GridPosition);
};
