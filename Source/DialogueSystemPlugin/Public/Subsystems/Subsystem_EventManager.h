#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Tickable.h"
#include "GameplayTagContainer.h"
#include "Subsystem_EventManager.generated.h"


DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnGlobalEventTriggered, FGameplayTag, EventTag);

UCLASS()
class DIALOGUESYSTEMPLUGIN_API USubsystem_EventManager : public UGameInstanceSubsystem, public FTickableGameObject
{
	GENERATED_BODY()
	
protected:

	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

	virtual void Tick(float DeltaTime) override; 
	virtual ETickableTickType GetTickableTickType() const override;
	virtual TStatId GetStatId() const override;
	
	//VARIABLES!!!
public:

	UPROPERTY(BlueprintAssignable, Category = "Event System")
	FOnGlobalEventTriggered OnGlobalEventTriggered;
	
	// Variables about events
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	FGameplayTagContainer RemainingEvents;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	FGameplayTagContainer TriggeredEvents;

	

	// Tick Variables
	UPROPERTY()
	float TickInterval = 1.f; 

	UPROPERTY()
	float TimeSinceLastTick = 0.0f;
	
	//FUNCTIONS!!!
public:

	UFUNCTION(BlueprintCallable)
	void TriggerEvent(FGameplayTagContainer EventNameToTrigger);

	UFUNCTION()
	static void PrintString(const FString& Message, float time, FColor Color);
};
