// Fill out your copyright notice in the Description page of Project Settings.


#include "BP_AITagGame.h"

// Sets default values
ABP_AITagGame::ABP_AITagGame()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

void ABP_AITagGame::Initialize(int32 InSquadNum, int32 InGridPosition)
{
	SquadNum = InSquadNum;
	GridPosition = InGridPosition;
}

// Called when the game starts or when spawned
void ABP_AITagGame::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ABP_AITagGame::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void ABP_AITagGame::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

