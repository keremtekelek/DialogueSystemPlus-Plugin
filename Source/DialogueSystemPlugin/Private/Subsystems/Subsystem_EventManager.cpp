#include "Subsystems/Subsystem_EventManager.h"

void USubsystem_EventManager::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

}

void USubsystem_EventManager::Deinitialize()
{
	Super::Deinitialize();
}


void USubsystem_EventManager::Tick(float DeltaTime)
{
	TimeSinceLastTick += DeltaTime;

	if (TimeSinceLastTick >= TickInterval)
	{
		//***WRITE CODES ABOUT TICK IN HERE!***
		
		
		 
		TimeSinceLastTick -= TickInterval; 
	}
}

ETickableTickType USubsystem_EventManager::GetTickableTickType() const
{
	return (IsTemplate() ? ETickableTickType::Never : ETickableTickType::Always);
}

TStatId USubsystem_EventManager::GetStatId() const
{
	
	RETURN_QUICK_DECLARE_CYCLE_STAT(USubsystem_EventManager, STATGROUP_Tickables);
}

void USubsystem_EventManager::TriggerEvent(FGameplayTagContainer EventNameToTrigger)
{
	for (auto It = EventNameToTrigger.CreateConstIterator(); It; ++It)
	{
		FGameplayTag Tag = *It;
        
		if (Tag.IsValid())
		{
			TriggeredEvents.AddTag(Tag);
			
			if (OnGlobalEventTriggered.IsBound())
			{
				OnGlobalEventTriggered.Broadcast(Tag);
			}
			
		}
	}
}


void USubsystem_EventManager::PrintString(const FString& Message, float Time, FColor Color)
{
	if (!GEngine)
	{
		return;
	}

	GEngine->AddOnScreenDebugMessage(-1,Time, Color, Message);
}