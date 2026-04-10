#include "Subsystems/Subsystem_Dialogue.h"
#include "Subsystems/Subsystem_EventManager.h"
#include "ActorComponents/AC_DialogueSystem.h"
#include "ActorComponents/AC_InteractionSystem.h"
#include "UI/Widget_MoodMeter.h"
#include "UI/Widget_Dialogue.h"
#include "Settings/DialogueSystemPluginSettings.h"
#include "GameFramework/PlayerController.h"


USubsystem_Dialogue::USubsystem_Dialogue()
{
	
}

void USubsystem_Dialogue::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	
	USubsystem_EventManager* eventSubsystem= Collection.InitializeDependency<USubsystem_EventManager>();
	//PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);

	if (eventSubsystem)
	{
		EventManager_Subsystem = eventSubsystem;
	}
	
	ProcessedChoices.Empty();
	ProcessedDialogues.Empty();
	ProcessedGlobalEvents.Reset();

	DSM_MainCharacter.Empty();
	DSM_NPC.Empty();
	
	LastDialogueNPC_Name = NAME_None;
	
	const UDialogueSystemSettings* Settings = GetDefault<UDialogueSystemSettings>();
	
	if (Settings)
	{
		DataTable_MainCharacter = Settings->DT_MainCharacterChoices.LoadSynchronous();
		DataTable_MainCharacterDialogue = Settings->DT_MainCharacterDialogues.LoadSynchronous();
	}
}

void USubsystem_Dialogue::Deinitialize()
{
	Super::Deinitialize();
}

void USubsystem_Dialogue::Tick(float DeltaTime)
{
	TimeSinceLastTick += DeltaTime;

	if (TimeSinceLastTick >= TickInterval)
	{
		//***WRITE CODES ABOUT TICK IN HERE!***

		if (EventManager_Subsystem)
		{
			ProcessedGlobalEvents = EventManager_Subsystem->TriggeredEvents;
		}
		
		//*** END OF THE CODES!***
		TimeSinceLastTick -= TickInterval; 
	}
}

ETickableTickType USubsystem_Dialogue::GetTickableTickType() const
{
	return (IsTemplate() ? ETickableTickType::Never : ETickableTickType::Always);
	
}

TStatId USubsystem_Dialogue::GetStatId() const
{
	RETURN_QUICK_DECLARE_CYCLE_STAT(USubsystem_EventManager, STATGROUP_Tickables);
}

// This function works when interaction started with Main Character with NPC.
void USubsystem_Dialogue::Interacted()
{
	if (IsValid(AC_InteractionSystem))
	{
		if (IsMainCharacterInDialogue)
		{
			// if the main character in dialogue, you cannot start a new dialogue
		}
		else
		{
			//Otherwise, you can start a dialogue
			
			/*
			 * But first, we should check to see if our dialogue partner now is same with last dialogue partner.
			 * If it is, we shouldn't let the player start the new dialogue. Otherwise, all score and dialogue logic will
			 * collapse. I mean not real collapse but wrong dialogues will display on the screen and we don't want that.
			 */
			
			GettingVariables();
			
			if (InteractedNPC_Name == LastDialogueNPC_Name)
			{
				UE_LOG(LogTemp, Warning, TEXT("Dialogue Partner and Last Dialogue Partner are the same!"));
				PlayDisturbDialogue();
			}
			else
			{
				StartDialogue();
			}
		}
	}
}

// Starting Dialogue
void USubsystem_Dialogue::StartDialogue()
{
	if (FilterDialogues()) 
	{
		IsMainCharacterInDialogue = true;
		LastDialogueNPC_Name = InteractedNPC_Name;
		LockOrReleaseTheMainCharacter(true);
		ControlDialogue();
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("StartDialogue failed!"));
		FinishDialogue(); 
	}
}

// Getting useful variables from DialogueSystem
void USubsystem_Dialogue::GettingVariables()
{
	if (IsValid(AC_InteractionSystem) && IsValid(AC_DialogueSystem))
	{
		
		WBP_MoodMeter = AC_DialogueSystem->WBP_MoodMeter;

		DataTable_NPC= AC_DialogueSystem->DataTable_NPC;
		DSM_NPC = AC_DialogueSystem->DSM_NPC;

		Mood_NPC = AC_DialogueSystem->NPC_Mood;
		MoodValue_NPC = AC_DialogueSystem->NPC_MoodValue;

		InteractedNPC_Name = AC_DialogueSystem->NPC_RealName;
	}
}

void USubsystem_Dialogue::ControlDialogue()
{
	if (NPC_EndOfDialogue)
	{
		if (ProcessedDialogues.Contains(NPC_DialogueID))
		{
			UE_LOG(LogTemp, Warning, TEXT("ProcessedDialogues has your best NPC_DialogueID, Dialogue's EndOfDialogue = true"))
			return;
		}

		ShowDialogue(NPC_DialogueText,NPC_ConversationPartner_Name);
		ProcessedDialogues.AddUnique(NPC_DialogueID);
		EventManager_Subsystem->TriggerEvent(NPC_EventsToTrigger);
		
		if (NPC_DialogueSound)
		{
			FVector SoundLocation = AC_DialogueSystem->OwnerActor->GetActorLocation();
			UGameplayStatics::PlaySoundAtLocation(this, NPC_DialogueSound,SoundLocation);

			GetWorld()->GetTimerManager().SetTimer(DelayCloseDialogueHandle,this, &USubsystem_Dialogue::CloseDialogueAfterSeconds,NPC_DialogueSound->Duration + 1.6f,false);
		}
		else
		{
			GetWorld()->GetTimerManager().SetTimer(DelayCloseDialogueHandle,this, &USubsystem_Dialogue::CloseDialogueAfterSeconds,CalculateDialogueDuration(NPC_DialogueText),false);
		}
	}
	else
	{
		if (ProcessedDialogues.Contains(NPC_DialogueID))
		{
			UE_LOG(LogTemp, Warning, TEXT("ProcessedDialogues has your best NPC_DialogueID, Dialogue's EndOfDialogue = false"))
			return;
		}
		
		ShowDialogue(NPC_DialogueText,NPC_ConversationPartner_Name);
		ProcessedDialogues.AddUnique(NPC_DialogueID);
		EventManager_Subsystem->TriggerEvent(NPC_EventsToTrigger);

		if (NPC_DialogueSound)
		{
			FVector SoundLocation = AC_DialogueSystem->OwnerActor->GetActorLocation();
			UGameplayStatics::PlaySoundAtLocation(this, NPC_DialogueSound,SoundLocation);

			if (!NPC_NextDialogueID.IsNone())
			{
				if (!GetWorld()->GetTimerManager().IsTimerActive(ShowNextDialogueHandle))
				{
					FName NextID = NPC_NextDialogueID;
					ShowNextDialogueDelegate.BindUObject(this, &USubsystem_Dialogue::ShowNextDialogueAfterSeconds,NextID);
					GetWorld()->GetTimerManager().SetTimer(ShowNextDialogueHandle,ShowNextDialogueDelegate,NPC_DialogueSound->Duration + 1.6f,false);
				}
			}
			else
			{
				if (!NPC_NextChoiceID.IsNone())
				{
					if (FilterChoices(NPC_NextChoiceID))
					{
						GetWorld()->GetTimerManager().SetTimer(DelayShowChoiceHandle,this, &USubsystem_Dialogue::ShowChoiceAfterSeconds,NPC_DialogueSound->Duration + 1.6f,false);
					}
				}
			}
		}
		else
		{
			if (!NPC_NextDialogueID.IsNone())
			{
				if (!GetWorld()->GetTimerManager().IsTimerActive(ShowNextDialogueHandle))
				{
					FName NextID = NPC_NextDialogueID;
					ShowNextDialogueDelegate.BindUObject(this, &USubsystem_Dialogue::ShowNextDialogueAfterSeconds,NextID);
					GetWorld()->GetTimerManager().SetTimer(ShowNextDialogueHandle,ShowNextDialogueDelegate,CalculateDialogueDuration(NPC_DialogueText),false);
				}
			}
			else
			{
				if (!NPC_NextChoiceID.IsNone())
				{
					if (FilterChoices(NPC_NextChoiceID))
					{
						GetWorld()->GetTimerManager().SetTimer(DelayShowChoiceHandle,this, &USubsystem_Dialogue::ShowChoiceAfterSeconds,CalculateDialogueDuration(NPC_DialogueText),false);
					}
				}
			}
		}
	}
}

// Filtering Dialogues
bool USubsystem_Dialogue::FilterDialogues()
{
	FName BestNPC_DialogueRowName = ScoreNPC_Dialogues();

	if (BestNPC_DialogueRowName.IsNone())
	{
		UE_LOG(LogTemp, Warning, TEXT("Best NPC Dialogue Row Name == None"))
		return false; 
	}
	else
	{
		FNPC_Dialogues* npcFoundRow = DataTable_NPC->FindRow<FNPC_Dialogues>(BestNPC_DialogueRowName,"");
		if (npcFoundRow)
		{
			GetBestDialogue_RowProperties(*npcFoundRow);
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("Best NPC Dialogue Row is null"))
			return false;
		}
	}
	return true;
}

//Filtering Choices
bool USubsystem_Dialogue::FilterChoices(FName ChoiceID)
{
	FName NextChoiceID = ChoiceID;
	
	if (NextChoiceID.IsNone())
	{
		UE_LOG(LogTemp, Warning, TEXT("Next Choice Row Name == None"))
		return false; 
	}
	else
	{
		FMainCharacterChoices* FoundChoiceRow = DataTable_MainCharacter->FindRow<FMainCharacterChoices>(NextChoiceID,"");
		if (FoundChoiceRow)
		{
			GetNextChoice_RowProperties(*FoundChoiceRow);
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("Next Choice Row is null"))
			return false;
		}
	}
	return true;
}

//Finishes Dialogue
void USubsystem_Dialogue::FinishDialogue()
{
	PrintString("Finish Dialogue has been called",2.f,FColor::Red);
	
	GetWorld()->GetTimerManager().ClearTimer(DelayShowChoiceHandle);
	GetWorld()->GetTimerManager().ClearTimer(DelayCloseDialogueHandle);
	
	OpenOrCloseCursor(false);
	LockOrReleaseTheMainCharacter(false);
	
	WBP_Dialogue->CloseDialogue();
	WBP_Dialogue->CloseChoices();
	
	IsMainCharacterInDialogue = false;
	
}



// Scoring NPC Dialogues.
FName USubsystem_Dialogue::ScoreNPC_Dialogues()
{
	if (DataTable_NPC)
	{
		TArray<FNPC_Dialogues*> DialogueRows;

		DataTable_NPC->GetAllRows("", DialogueRows);

		for (const auto DialogueRow: DialogueRows)
		{
			DialogueScore_Value = 0;
			
			if (ProcessedDialogues.Contains(DialogueRow->DialogueID))
			{
				continue;
			}
			
			if (!DialogueRow->IsRoot)
			{
				continue;
			}
			
			if (!(DialogueRow->TargetLevel.GetAssetName() == GetCurrentMap()))
			{
				continue;
			}
			
			
			if (!DialogueRow->HasToRequireEvents.IsEmpty() && !ProcessedGlobalEvents.HasAll(DialogueRow->HasToRequireEvents))
			{
				continue;
			}
			

			FGameplayTagContainer RelatedTags = DialogueRow->RelatedGlobalEvents;
        
			for (auto It = RelatedTags.CreateConstIterator(); It; ++It)
			{
				if (ProcessedGlobalEvents.HasTag(*It))
				{
					AddScoreValue(10);
				}
				else
				{
					AddScoreValue(-10); 
				}
			}
			
			for (FName NPC_Dialogues: DialogueRow->RelatedNPC_Dialogues)
			{
				if (ProcessedDialogues.Contains(NPC_Dialogues))
				{
					AddScoreValue(10);
				}
				else
				{
					AddScoreValue(-10);  
				}
			}

			if (Mood_NPC == DialogueRow->DesiredNPC_Mood)
			{
				AddScoreValue(5);
			}

			DSM_NPC.Add(DialogueRow->DialogueID, DialogueScore_Value);
		}

		FName BestDialogueID = NAME_None;
		int HighestScore = INT_MIN;

		// Finding Highest Score Row
		for (const auto& Pair : DSM_NPC)
		{
			if (Pair.Value > HighestScore)
			{
				HighestScore = Pair.Value;
				BestDialogueID = Pair.Key;
			}
		}

		if (BestDialogueID != NAME_None)
		{
			UE_LOG(LogTemp, Log, TEXT("Best Dialogue: %s, Score: %d"), *BestDialogueID.ToString(), HighestScore);
		}

		DSM_NPC.Empty();
		DialogueScore_Value = 0;
		
		return BestDialogueID;
	}

	return NAME_None;
}



// Getting Row Properties
void USubsystem_Dialogue::GetNextChoice_RowProperties(const FMainCharacterChoices& NextChoice_Row)
{
	Choice1_ID = NextChoice_Row.Choice1.ChoiceID1;
	Choice2_ID = NextChoice_Row.Choice2.ChoiceID2;
	Choice3_ID = NextChoice_Row.Choice3.ChoiceID3;

	Choice1_Text = NextChoice_Row.Choice1.ChoiceText;
	Choice2_Text = NextChoice_Row.Choice2.ChoiceText;
	Choice3_Text = NextChoice_Row.Choice3.ChoiceText;

	Choice1_EffectsMood = NextChoice_Row.Choice1.EffectsMood;
	Choice2_EffectsMood = NextChoice_Row.Choice2.EffectsMood;
	Choice3_EffectsMood = NextChoice_Row.Choice3.EffectsMood;

	Choice1_EndOfDialogue = NextChoice_Row.Choice1.EndOfDialogue;
	Choice2_EndOfDialogue = NextChoice_Row.Choice2.EndOfDialogue;
	Choice3_EndOfDialogue = NextChoice_Row.Choice3.EndOfDialogue;

	Choice1_EventsToTrigger = NextChoice_Row.Choice1.EventsToTrigger;
	Choice2_EventsToTrigger = NextChoice_Row.Choice2.EventsToTrigger;
	Choice3_EventsToTrigger = NextChoice_Row.Choice3.EventsToTrigger;
	
	Choice1_NextDialogueID = NextChoice_Row.Choice1.NextDialogueID;
	Choice2_NextDialogueID = NextChoice_Row.Choice2.NextDialogueID;
	Choice3_NextDialogueID = NextChoice_Row.Choice3.NextDialogueID;
}

void USubsystem_Dialogue::GetBestDialogue_RowProperties(const FNPC_Dialogues& BestNPC_Row)
{
	NPC_DialogueID = BestNPC_Row.DialogueID;
	NPC_DialogueText = BestNPC_Row.DialogueText;
	NPC_DialogueSound = BestNPC_Row.DialogueSound;
	NPC_EndOfDialogue = BestNPC_Row.EndOfDialogue;
	NPC_EventsToTrigger = BestNPC_Row.EventsToTrigger;
	NPC_NextDialogueID = BestNPC_Row.NextDialogueID;
	NPC_NextChoiceID = BestNPC_Row.NextChoiceID;
	NPC_ConversationPartner_Name = BestNPC_Row.SpeakerName;
	NPC_HasToRequireEvents = BestNPC_Row.HasToRequireEvents;
}


// Adding Score Value
void USubsystem_Dialogue::AddScoreValue(int ScoreToAdd)
{
	DialogueScore_Value += ScoreToAdd;
}

//Calculates Dialogue Duration
float USubsystem_Dialogue::CalculateDialogueDuration(FText DialogueText)
{
	FString DialogueTextString = DialogueText.ToString();
	int DialogueLength = DialogueTextString.Len();
	
	float CalculatedDuration = (DialogueLength * 0.05f) + MinimumDialogueLength;
	
	return CalculatedDuration;
}


// Timer Functions
void USubsystem_Dialogue::ShowChoiceAfterSeconds()
{
	WBP_Dialogue->ShowChoices(Choice1_Text, Choice2_Text, Choice3_Text);
	OpenOrCloseCursor(true);

	GetWorld()->GetTimerManager().ClearTimer(DelayShowChoiceHandle);

	PrintString("ShowChoiceAfterSeconds function has been called",2.f,FColor::Red);
}

void USubsystem_Dialogue::ShowNextDialogueAfterSeconds(FName NextDialogueID)
{
	FNPC_Dialogues* FoundRow = DataTable_NPC->FindRow<FNPC_Dialogues>(NextDialogueID,"");
	bool IsNPC = true;
	
	if (!FoundRow)
	{
		FoundRow = DataTable_MainCharacterDialogue->FindRow<FNPC_Dialogues>(NextDialogueID,"");
		IsNPC = false;
	}
	
	if (FoundRow->EndOfDialogue)
	{
		if (ProcessedDialogues.Contains(FoundRow->DialogueID))
		{
			return;
		}

		if (IsNPC)
		{
			ShowDialogue(FoundRow->DialogueText,FoundRow->SpeakerName);
		}
		else
		{
			ShowDialogue(FoundRow->DialogueText,NAME_None);
		}

		ProcessedDialogues.AddUnique(FoundRow->DialogueID);
		EventManager_Subsystem->TriggerEvent(FoundRow->EventsToTrigger);
		
		if (FoundRow->DialogueSound)
		{
			FVector SoundLocation = AC_DialogueSystem->OwnerActor->GetActorLocation();
			UGameplayStatics::PlaySoundAtLocation(this, FoundRow->DialogueSound,SoundLocation);

			GetWorld()->GetTimerManager().ClearTimer(ShowNextDialogueHandle);
			
			GetWorld()->GetTimerManager().SetTimer(DelayCloseDialogueHandle,this, &USubsystem_Dialogue::CloseDialogueAfterSeconds,FoundRow->DialogueSound->Duration + 1.6f,false);
		}
		else
		{
			GetWorld()->GetTimerManager().ClearTimer(ShowNextDialogueHandle);
			GetWorld()->GetTimerManager().SetTimer(DelayCloseDialogueHandle,this, &USubsystem_Dialogue::CloseDialogueAfterSeconds,CalculateDialogueDuration(FoundRow->DialogueText),false);
		}
	}
	else
	{
		if (ProcessedDialogues.Contains(FoundRow->DialogueID))
		{
			return;
		}
		if (IsNPC)
		{
			ShowDialogue(FoundRow->DialogueText,FoundRow->SpeakerName);
		}
		else
		{
			ShowDialogue(FoundRow->DialogueText,NAME_None);
		}
		
		ProcessedDialogues.AddUnique(FoundRow->DialogueID);
		EventManager_Subsystem->TriggerEvent(FoundRow->EventsToTrigger);

		if (FoundRow->DialogueSound)
		{
			FVector SoundLocation = AC_DialogueSystem->OwnerActor->GetActorLocation();
			UGameplayStatics::PlaySoundAtLocation(this, NPC_DialogueSound,SoundLocation);

			if (!FoundRow->NextDialogueID.IsNone())
			{
				FName NextID = FoundRow->NextDialogueID;
				ShowNextDialogueDelegate.BindUObject(this, &USubsystem_Dialogue::ShowNextDialogueAfterSeconds, NextID);
				GetWorld()->GetTimerManager().ClearTimer(ShowNextDialogueHandle);
				
				GetWorld()->GetTimerManager().SetTimer(ShowNextDialogueHandle,ShowNextDialogueDelegate,FoundRow->DialogueSound->Duration + 1.6f,false);
			}
			else if (!FoundRow->NextChoiceID.IsNone()) 
			{
				if (FilterChoices(FoundRow->NextChoiceID)) 
				{
					GetWorld()->GetTimerManager().ClearTimer(ShowNextDialogueHandle);
					GetWorld()->GetTimerManager().SetTimer(DelayShowChoiceHandle,this, &USubsystem_Dialogue::ShowChoiceAfterSeconds,FoundRow->DialogueSound->Duration + 1.6f,false);
				}
				else
				{
					UE_LOG(LogTemp, Error, TEXT("NextChoiceID has not found in DataTable!"));
					FinishDialogue();
				}
			}
			else
			{
				GetWorld()->GetTimerManager().ClearTimer(ShowNextDialogueHandle);
				GetWorld()->GetTimerManager().SetTimer(DelayCloseDialogueHandle,this, &USubsystem_Dialogue::CloseDialogueAfterSeconds,FoundRow->DialogueSound->Duration + 1.6f,false);
			}
			
		}
		else
		{
			if (!FoundRow->NextDialogueID.IsNone())
			{
				FName NextID = FoundRow->NextDialogueID;
				ShowNextDialogueDelegate.BindUObject(this, &USubsystem_Dialogue::ShowNextDialogueAfterSeconds, NextID);
				
				GetWorld()->GetTimerManager().ClearTimer(ShowNextDialogueHandle);
				GetWorld()->GetTimerManager().SetTimer(ShowNextDialogueHandle,ShowNextDialogueDelegate,CalculateDialogueDuration(FoundRow->DialogueText),false);
			}
			else if (!FoundRow->NextChoiceID.IsNone()) 
			{
				if (FilterChoices(FoundRow->NextChoiceID)) 
				{
					GetWorld()->GetTimerManager().ClearTimer(ShowNextDialogueHandle);
					GetWorld()->GetTimerManager().SetTimer(DelayShowChoiceHandle,this, &USubsystem_Dialogue::ShowChoiceAfterSeconds,CalculateDialogueDuration(FoundRow->DialogueText),false);
				}
				else
				{
					UE_LOG(LogTemp, Error, TEXT("NextChoiceID has not found in DataTable!"));
					FinishDialogue();
				}
			}
			else
			{
				GetWorld()->GetTimerManager().ClearTimer(ShowNextDialogueHandle);
				GetWorld()->GetTimerManager().SetTimer(DelayCloseDialogueHandle,this, &USubsystem_Dialogue::CloseDialogueAfterSeconds,CalculateDialogueDuration(FoundRow->DialogueText),false);
			}
		}
	}
}

void USubsystem_Dialogue::CloseDialogueAfterSeconds()
{
	WBP_Dialogue->CloseDialogue();
	FinishDialogue();

	GetWorld()->GetTimerManager().ClearTimer(DelayCloseDialogueHandle);
}

void USubsystem_Dialogue::ShowDialogue(FText DialogueToShow,FName OwnerOfDialogue)
{
	if (AC_DialogueSystem->ExposeRealName)
	{
		FString Owner = AC_DialogueSystem->NPC_RealName.ToString();

		FText OwnerText = FText::FromString(Owner);
		FText Result = FText::Format(FText::FromString("{0}: {1}"),OwnerText,DialogueToShow);

		WBP_Dialogue->ShowDialogue(Result);
	}
	else
	{
		FString Owner = AC_DialogueSystem->NPC_OfficialName.ToString();

		FText OwnerText = FText::FromString(Owner);
		FText Result = FText::Format(FText::FromString("{0}: {1}"),OwnerText,DialogueToShow);

		WBP_Dialogue->ShowDialogue(Result);
	}
	
}

void USubsystem_Dialogue::SkipDialogue()
{
	FTimerManager& TimerManager = GetWorld()->GetTimerManager();

	if (TimerManager.IsTimerActive(DelayShowChoiceHandle))
	{
		TimerManager.ClearTimer(DelayShowChoiceHandle);
		ShowChoiceAfterSeconds(); 
		PrintString("Skipped to the Choices", 1.f, FColor::Green);
		return;
	}

	if (TimerManager.IsTimerActive(DelayCloseDialogueHandle))
	{
		TimerManager.ClearTimer(DelayCloseDialogueHandle);
		CloseDialogueAfterSeconds(); 
		PrintString("Skipped to the Close Dialogue", 1.f, FColor::Green);
		return;
	}

	if (TimerManager.IsTimerActive(ShowNextDialogueHandle))
	{
		TimerManager.ClearTimer(ShowNextDialogueHandle);
		TimerManager.SetTimer(ShowNextDialogueHandle, ShowNextDialogueDelegate, 0.001f, false);
        
		PrintString("Skipped to the Next Dialogue", 1.f, FColor::Green);
		return;
	}
	PrintString("Skip Dialogue Worked!", 1.5f, FColor::Yellow);
}

void USubsystem_Dialogue::PlayDisturbDialogue()
{
	IsMainCharacterInDialogue = true;
	
	const UDialogueSystemSettings* Settings = GetDefault<UDialogueSystemSettings>();
	
	if (Settings)
	{
		UDataTable* DisturbTable = Settings->DT_Disturb.LoadSynchronous();
		
		if (DisturbTable)
		{
			FTimerManager& TimerManager = GetWorld()->GetTimerManager();
			if (DisturbTable)
			{
				TArray<FName> RowNames = DisturbTable->GetRowNames();
		
				if (!RowNames.IsEmpty())
				{
					int RandomIndex = FMath::RandRange(0, RowNames.Num() - 1);
					FName SelectedRowName = RowNames[RandomIndex];
			
					FDisturb* RandomDisturbRow = DisturbTable->FindRow<FDisturb>(SelectedRowName, "");
			
					if (RandomDisturbRow)
					{
						ShowDialogue(RandomDisturbRow->DisturbText, InteractedNPC_Name);
				
						if (TimerManager.IsTimerActive(DelayCloseDialogueHandle))
						{
							TimerManager.ClearTimer(DelayCloseDialogueHandle);
					
							GetWorld()->GetTimerManager().SetTimer(DelayCloseDialogueHandle,this, &USubsystem_Dialogue::CloseDialogueAfterSeconds,CalculateDialogueDuration(RandomDisturbRow->DisturbText),false);
						}
						else
						{
							GetWorld()->GetTimerManager().SetTimer(DelayCloseDialogueHandle,this, &USubsystem_Dialogue::CloseDialogueAfterSeconds,CalculateDialogueDuration(RandomDisturbRow->DisturbText),false);
						}
					}
				}
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("DT_Disturb is not valid!"));
			}
		}
	}
}


//Event related functions
void USubsystem_Dialogue::EventReceived(FGameplayTag EventTag)
{
	HandleGameEvent(EventTag);
}

void USubsystem_Dialogue::HandleGameEvent(FGameplayTag EventTag)
{
	
}


//Handles choice UI screen and cursor
void USubsystem_Dialogue::OpenOrCloseCursor(bool OpenOrCloseValue)
{
	PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	
	if (OpenOrCloseValue)
	{
		FInputModeUIOnly InputMode;
		InputMode.SetWidgetToFocus(WBP_Dialogue->TakeWidget());
		InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
		PlayerController->SetInputMode(InputMode);
		PlayerController->bShowMouseCursor = true;
	}
	else
	{
		FInputModeGameOnly InputMode;
		PlayerController->SetInputMode(InputMode);
		PlayerController->bShowMouseCursor = false;
		FSlateApplication::Get().SetAllUserFocusToGameViewport();
	}
}

void USubsystem_Dialogue::LockOrReleaseTheMainCharacter(bool LockOrReleaseValue)
{
	PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	
	if (PlayerController)
	{
		// locking the main character
		if (LockOrReleaseValue)
		{
			PlayerController->SetIgnoreMoveInput(true);
			PlayerController->StopMovement();
			
		}
	
		// releasing the main character
		else
		{
			while (PlayerController->IsMoveInputIgnored())
			{
				PlayerController->SetIgnoreMoveInput(false);
			}

			while (PlayerController->IsLookInputIgnored())
			{
				PlayerController->SetIgnoreLookInput(false);
			}
		}
	}
}

FString USubsystem_Dialogue::GetCurrentMap()
{
	ULevel* NPCLevel = AC_DialogueSystem->GetOwner()->GetLevel();
		
	if (NPCLevel && NPCLevel->GetOuter())
	{
		FString NPCMapName = NPCLevel->GetOuter()->GetName();
			
		NPCMapName.RemoveFromStart(GetWorld()->StreamingLevelsPrefix);
			
		return FPackageName::GetShortName(NPCMapName);
	}

	FString CurrentMapName = GetWorld()->GetMapName();
	CurrentMapName.RemoveFromStart(GetWorld()->StreamingLevelsPrefix);
	
	return CurrentMapName;
}



void USubsystem_Dialogue::PrintString(const FString& Message, float Time, FColor Color)
{
	if (!GEngine)
	{
		return;
	}

	GEngine->AddOnScreenDebugMessage(-1,Time, Color, Message);
}


void USubsystem_Dialogue::MakeChoice(EChosenOption ChosenButton)
{
	FName TargetDialogueID = NAME_None;
	
	if (ChosenButton == EChosenOption::Choice1)
	{
		PrintString("Choice1 has been chosen",2.f,FColor::Red);
		
		if (Choice1_EndOfDialogue)
		{
			ProcessedChoices.AddUnique(Choice1_ID);
			FinishDialogue();
		}
		else
		{
			ProcessedChoices.Add(Choice1_ID);
			WBP_Dialogue->CloseChoices();
		}
		
		AC_DialogueSystem->AddMoodValue(Choice1_EffectsMood);
		EventManager_Subsystem->TriggerEvent(Choice1_EventsToTrigger);
		
		TargetDialogueID = Choice1_NextDialogueID;
		OpenOrCloseCursor(false);
		
	}
	else if (ChosenButton == EChosenOption::Choice2)
	{
		PrintString("Choice2 has been chosen",2.f,FColor::Red);
		if (Choice2_EndOfDialogue)
		{
			ProcessedChoices.AddUnique(Choice2_ID);
			FinishDialogue();
		}
		else
		{
			ProcessedChoices.AddUnique(Choice2_ID);
			WBP_Dialogue->CloseChoices();
		}
		AC_DialogueSystem->AddMoodValue(Choice2_EffectsMood);
		EventManager_Subsystem->TriggerEvent(Choice2_EventsToTrigger);
		
		TargetDialogueID = Choice2_NextDialogueID;
		OpenOrCloseCursor(false);
		
	}
	else if (ChosenButton == EChosenOption::Choice3)
	{
		PrintString("Choice3 has been chosen",2.f,FColor::Red);
		if (Choice3_EndOfDialogue)
		{
			ProcessedChoices.AddUnique(Choice3_ID);
			FinishDialogue();
		}
		else
		{
			ProcessedChoices.AddUnique(Choice3_ID);
			WBP_Dialogue->CloseChoices();
		}
		AC_DialogueSystem->AddMoodValue(Choice3_EffectsMood);
		EventManager_Subsystem->TriggerEvent(Choice3_EventsToTrigger);
		
		TargetDialogueID = Choice3_NextDialogueID;
		OpenOrCloseCursor(false);
		
	}
	
	if (!TargetDialogueID.IsNone())
	{
		FNPC_Dialogues* NextRow = DataTable_NPC->FindRow<FNPC_Dialogues>(TargetDialogueID, "");
        
		if (NextRow)
		{
			GetBestDialogue_RowProperties(*NextRow);
			WBP_Dialogue->CloseChoices();
			ControlDialogue();
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("MakeChoice called, Target ID (%s) not found"), *TargetDialogueID.ToString());
			FinishDialogue();
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("MakeChoice called but this choice has no NextDialogueID! "));
		FinishDialogue();
	}
}


void USubsystem_Dialogue::InitiliazeDialogueUI(TSubclassOf<UWidget_Dialogue> DialogueWidgetClass)
{
	if (!WBP_Dialogue)
	{
		WBP_Dialogue = CreateWidget<UWidget_Dialogue>(GetWorld(), DialogueWidgetClass);
		
		if (WBP_Dialogue)
		{
			WBP_Dialogue->AddToViewport();
		}
	}
}
