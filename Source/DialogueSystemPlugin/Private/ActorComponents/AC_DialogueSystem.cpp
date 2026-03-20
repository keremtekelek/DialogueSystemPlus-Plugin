#include "ActorComponents/AC_DialogueSystem.h"
#include "UI/Widget_MoodMeter.h"
#include "Interfaces/Interface_MainCharacter.h"




UAC_DialogueSystem::UAC_DialogueSystem()
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = true;     
	PrimaryComponentTick.TickInterval = 0.5f;
	
}



void UAC_DialogueSystem::BeginPlay()
{
	Super::BeginPlay();
	
	//GettingStaticVariables();
	
	// Initiliazing Mood Widget Component
	if (MoodWidgetComponent)
	{
		if (!MoodWidgetComponent->GetUserWidgetObject())
		{
			MoodWidgetComponent->InitWidget();
		}

		UWidget_MoodMeter* Moodmeter = Cast<UWidget_MoodMeter>(MoodWidgetComponent->GetUserWidgetObject());

		if (Moodmeter)
		{
			WBP_MoodMeter = Moodmeter;
		}
	}
}



void UAC_DialogueSystem::TickComponent(float DeltaTime, ELevelTick TickType,FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	
	GettingDynamicVariables();
}


void UAC_DialogueSystem::OnRegister()
{
	Super::OnRegister();
	
	GettingStaticVariables();
	
	
	if (OwnerActor && !IsOwnerMainCharacter && !MoodWidgetComponent)
	{
		MoodWidgetComponent = NewObject<UWidgetComponent>(OwnerActor, UWidgetComponent::StaticClass(), TEXT("Mood Meter Widget Component"));

		if (MoodWidgetComponent)
		{
			MoodWidgetComponent->CreationMethod = EComponentCreationMethod::Instance;
			
			if (OwnerActor->GetRootComponent())
			{
				MoodWidgetComponent->SetupAttachment(OwnerActor->GetRootComponent());
			}

			MoodWidgetComponent->SetWidgetSpace(EWidgetSpace::Screen); 
			MoodWidgetComponent->SetDrawSize(FVector2D(200.0f, 50.0f)); 
			MoodWidgetComponent->SetRelativeLocation(FVector(0.0f, 0.0f, 95.0f));
			
			MoodWidgetComponent->RegisterComponent();
		}
	}
	
	if (MoodWidgetComponent)
	{
		MoodWidgetComponent->SetWidgetClass(MoodWidgetClass);
	}
}

void UAC_DialogueSystem::GettingStaticVariables()
{
	// Getting This Component's Owner
	OwnerActor = GetOwner();

	if (OwnerActor && OwnerActor->Implements<UInterface_MainCharacter>())
	{
		IsOwnerMainCharacter = true;
	}
	else
	{
		IsOwnerMainCharacter = false;
	}
}

// Getting Useful Dynamic Variables
void UAC_DialogueSystem::GettingDynamicVariables()
{
	NPC_Mood = GetMoodValue();
}

void UAC_DialogueSystem::AddMoodValue(int MoodValueToAdd)
{
	if (NPC_MoodValue + MoodValueToAdd > 100)
	{
		NPC_MoodValue = 100;
	}
	else if (NPC_MoodValue + MoodValueToAdd < 0)
	{
		NPC_MoodValue = 0;
	}
	else
	{
		NPC_MoodValue += MoodValueToAdd;
	}
}

ENPCMood UAC_DialogueSystem::GetMoodValue()
{
	if (0 <= NPC_MoodValue && NPC_MoodValue < 25)
	{
		return ENPCMood::NotHappy;
	}
	else if (25 <= NPC_MoodValue && NPC_MoodValue < 75)
	{
		return ENPCMood::Neutral;
	}
	else if (75 <= NPC_MoodValue && NPC_MoodValue <= 100)
	{
		return ENPCMood::Happy;
	}
	else
	{
		return ENPCMood::DoesntMatter;
	}
}
