// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "BP_AITagGame.generated.h"

UCLASS()
class TOPDOWNTEST_API ABP_AITagGame : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ABP_AITagGame();
	void Initialize(int32 InSquadNum,int32 InGridPosition);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "SquadInfo")
	int SquadNum;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "SquadInfo")
	int GridPosition;
};
