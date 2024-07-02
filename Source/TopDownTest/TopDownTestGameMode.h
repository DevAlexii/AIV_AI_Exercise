// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/TargetPoint.h"
#include "GameFramework/GameModeBase.h"
#include "BP_AITagGame.h"
#include "BP_BallTagGame.h"
#include "TopDownTestGameMode.generated.h"



DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FUpdateUIPointsDelegate, int, Squad,int,Points);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FUpdateUIGridPositionDelegate, int, GridPositionCalled);

UCLASS(minimalapi)
class ATopDownTestGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	ATopDownTestGameMode();
	void BeginPlay() override;
	AActor* GetBall() const;
	bool Restart(int& NewGridPosition);
	void GivePoints(const int SquadNum);


protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "GameSetup")
	TArray<ATargetPoint*> SpawnPoints;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GameSetup")
	TSubclassOf<ABP_BallTagGame> BallClass;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "GameSetup")
	AActor* BallInstance;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "GameSetup")
	int NPCsInBase;


	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "SquadInfo")
	TArray<APawn*>NPCs;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "SquadInfo")
	TArray<int>SquadPoints;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SquadInfo")
	int NumPlayerPerSquad;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SquadInfo")
	TSubclassOf<ABP_AITagGame> AIClass;

public:
	UPROPERTY(BlueprintAssignable, Category = "GameSetup")
	FUpdateUIPointsDelegate UpdateUIPoints;
	UPROPERTY(BlueprintAssignable, Category = "GameSetup")
	FUpdateUIGridPositionDelegate UpdateUIGridPosition;
};