#include "ActorComponents/AC_InteractionSystem.h"
#include "Interfaces/Interface_MainCharacter.h"
#include "ActorComponents/AC_DialogueSystem.h"
#include "Subsystems/Subsystem_Dialogue.h"
#include "UI/Widget_MoodMeter.h"


UAC_InteractionSystem::UAC_InteractionSystem()
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.TickInterval = 0.5f;
}



void UAC_InteractionSystem::BeginPlay()
{
	Super::BeginPlay();

	GetStaticVariables();
}



void UAC_InteractionSystem::TickComponent(float DeltaTime, ELevelTick TickType,FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	
}

void UAC_InteractionSystem::GetStaticVariables()
{
	//Getting Owner
	if (GetOwner())
	{
		OwnerActor = GetOwner();
	}
	
	//We are questioning whether the owner actor is the main character or not.
	if (OwnerActor->Implements<UInterface_MainCharacter>())
	{
		IsOwnerMainCharacter = true;
	}
	else
	{
		IsOwnerMainCharacter = false;
	}
	
	// Getting Dialogue Subsystem
	if (UWorld* World = GetWorld())
	{
		if (UGameInstance* GI = World->GetGameInstance())
		{
			DialogueSubsystem = GI->GetSubsystem<USubsystem_Dialogue>();
		}
	}
	
	
	//Getting AC_DialogueSystem from Owner
	
	UAC_DialogueSystem* DialogueComponent = OwnerActor->FindComponentByClass<UAC_DialogueSystem>(); 
	if (DialogueComponent)
	{
		if (IsOwnerMainCharacter)
		{
			OwnerMC_DialogueSystem = DialogueComponent;
		}
		else
		{
			OwnerNPC_DialogueSystem = DialogueComponent;
		}
	}
	
	if (IsOwnerMainCharacter)
	{
		OwnerMC_InteractionSystem = this;
	}
}

void UAC_InteractionSystem::TriggerNPCInteraction()
{
	if (OnNPCInteract.IsBound())
	{
		OnNPCInteract.Broadcast();
	}
}

//If the owner actor is main character, when main character is interacted, this function will be called!
void UAC_InteractionSystem::MC_Interact()
{
	if (!OwnerActor || !IsOwnerMainCharacter)
	{
		return;
	}
	
	FVector EyeLocation;
	FRotator EyeRotation;
	OwnerActor->GetActorEyesViewPoint(EyeLocation, EyeRotation);

	FVector EndLocation = EyeLocation + (EyeRotation.Vector() * InteractionDistance);

	FHitResult HitResult;
	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(OwnerActor); 

	
	bool Hit = GetWorld()->LineTraceSingleByChannel(
		HitResult,
		EyeLocation,
		EndLocation,
		ECC_Visibility, 
		QueryParams
	);

	
	DrawDebugLine(GetWorld(), EyeLocation, EndLocation, Hit ? FColor::Green : FColor::Red, false, 2.0f, 0, 1.0f);

	if (Hit && HitResult.GetActor())
	{
		AActor* HitActor = HitResult.GetActor();
		
		UE_LOG(LogTemp, Warning, TEXT("Line Trace Hit Actor: %s"), *HitActor->GetName());
		
		TargetNPC_InteractionSystem = HitActor->GetComponentByClass<UAC_InteractionSystem>();
		TargetNPC_DialogueSystem = HitActor->GetComponentByClass<UAC_DialogueSystem>();
		
		if (TargetNPC_InteractionSystem)
		{
			TargetNPC_InteractionSystem->TriggerNPCInteraction();
			DialogueSubsystem->AC_InteractionSystem = TargetNPC_InteractionSystem;
			
			if (TargetNPC_DialogueSystem)
			{
				if (TargetNPC_DialogueSystem->ShouldShowNPC_Mood)
				{
					FString moodString = FString::FromInt(TargetNPC_DialogueSystem->NPC_MoodValue);
					FText mood = FText::FromString(moodString);
					TargetNPC_DialogueSystem->WBP_MoodMeter->ShowMoodValue(true, mood);
					
					UE_LOG(LogTemp, Warning, TEXT("Mood value is: %d"), TargetNPC_DialogueSystem->NPC_MoodValue)
				}
				DialogueSubsystem->AC_DialogueSystem = TargetNPC_DialogueSystem;
				
				DialogueSubsystem->Interacted();
			}
			
			UE_LOG(LogTemp, Warning, TEXT("This actor has AC_InteractionSystem!"))
		}
		else
		{
			DialogueSubsystem->AC_InteractionSystem = nullptr;
			UE_LOG(LogTemp, Warning, TEXT("This actor doesn't have AC_InteractionSystem!"))
		}
	}
}

