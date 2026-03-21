#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/DataTable.h"
#include "EnumsAndStructs/GlobalEnumsAndStructs.h"
#include "Sound/SoundBase.h"
#include "GameplayTagContainer.h"
#include "GameplayTagsManager.h"
#include "CollisionQueryParams.h"
#include "Subsystem_Dialogue.generated.h"

class UAC_DialogueSystem;
class UAC_InteractionSystem;
class UWidget_Dialogue;
class UWidget_MoodMeter;
class USubsystem_EventManager;
class UDialogueSystemSubsystem;;

UENUM(BlueprintType)
enum class EScoreType : uint8
{
	Choice				UMETA(DisplayName = "Choice"),    
	Dialogue	        UMETA(DisplayName = "Dialogue")   
	  
};

UCLASS()
class DIALOGUESYSTEMPLUGIN_API USubsystem_Dialogue : public UGameInstanceSubsystem, public FTickableGameObject
{
	GENERATED_BODY()
	
public:
	
	USubsystem_Dialogue();
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;
	
	virtual void Tick(float DeltaTime) override; 
	virtual ETickableTickType GetTickableTickType() const override;
	virtual TStatId GetStatId() const override;
	
	//VARIABLES
public:
	
	
	UPROPERTY(EditAnywhere, Category = "Actor Components")
	UAC_InteractionSystem* AC_InteractionSystem;

	UPROPERTY(EditAnywhere, Category = "Actor Components")
	UAC_DialogueSystem* AC_DialogueSystem;
	
	UPROPERTY(VisibleAnywhere, Category = "Widgets")
	UWidget_Dialogue* WBP_Dialogue;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Widgets")
	UWidget_MoodMeter* WBP_MoodMeter;
	
	UPROPERTY(VisibleAnywhere, Category = "Subsystems")
	USubsystem_EventManager* EventManager_Subsystem;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "NPC Data")
	UDataTable* DataTable_NPC;

	// DSM stands for "Dialogue Score Map".
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "NPC Data")
	TMap<FName, int32> DSM_NPC;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "NPC Data")
	int MoodValue_NPC;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "NPC Data")
	ENPCMood Mood_NPC;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "NPC Data")
	FName InteractedNPC_Name;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Main Character Data")
	TMap<FName, int32> DSM_MainCharacter;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Main Character Data")
	UDataTable* DataTable_MainCharacter;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Main Character Data")
	UDataTable* DataTable_MainCharacterDialogue;
	
	// Processed Variables
	UPROPERTY(VisibleAnywhere)
	TArray<FName> ProcessedDialogues;

	UPROPERTY(VisibleAnywhere)
	TArray<FName> ProcessedChoices;

	UPROPERTY(VisibleAnywhere)
	FGameplayTagContainer ProcessedGlobalEvents;
	
	// Score Values
	UPROPERTY(VisibleAnywhere)
	int DialogueScore_Value = 0;

	UPROPERTY(VisibleAnywhere)
	int ChoiceScore_Value = 0;
	
	//Best NPC Row Properties

	UPROPERTY()
	FName NPC_DialogueID;
	
	UPROPERTY()
	FName NPC_NextDialogueID;
	
	UPROPERTY()
	FName NPC_NextChoiceID;
	
	UPROPERTY()
	FText NPC_DialogueText;

	UPROPERTY()
	USoundBase* NPC_DialogueSound;

	UPROPERTY()
	bool NPC_EndOfDialogue;

	UPROPERTY()
	FGameplayTagContainer NPC_EventsToTrigger;
	
	UPROPERTY()
	FGameplayTagContainer NPC_HasToRequireEvents;

	UPROPERTY()
	FName NPC_ConversationPartner_Name;
	
	//Best Main Character Choice Row Properties

	UPROPERTY()
	FName Choice1_ID;

	UPROPERTY()
	FName Choice2_ID;

	UPROPERTY()
	FName Choice3_ID;


	
	UPROPERTY()
	FText Choice1_Text;

	UPROPERTY()
	FText Choice2_Text;

	UPROPERTY()
	FText Choice3_Text;

	UPROPERTY()
	bool Choice1_EndOfDialogue;

	UPROPERTY()
	bool Choice2_EndOfDialogue;

	UPROPERTY()
	bool Choice3_EndOfDialogue;

	UPROPERTY()
	FGameplayTagContainer Choice1_EventsToTrigger;

	UPROPERTY()
	FGameplayTagContainer Choice2_EventsToTrigger;

	UPROPERTY()
	FGameplayTagContainer Choice3_EventsToTrigger;

	UPROPERTY()
	int Choice1_EffectsMood;

	UPROPERTY()
	int Choice2_EffectsMood;

	UPROPERTY()
	int Choice3_EffectsMood;
	
	UPROPERTY()
	FName Choice1_NextDialogueID;
	
	UPROPERTY()
	FName Choice2_NextDialogueID;
	
	UPROPERTY()
	FName Choice3_NextDialogueID;
	
	// Timer variables
	FTimerHandle DelayShowChoiceHandle;
	FTimerHandle DelayCloseDialogueHandle;
	FTimerHandle ShowNextDialogueHandle;
	FTimerDelegate ShowNextDialogueDelegate;
	

	//Chosen Button
	UPROPERTY()
	EChosenOption ChosenChoice = EChosenOption::None;
	
	// Tick Variables
	UPROPERTY()
	float TickInterval = 0.5f; 

	UPROPERTY()
	float TimeSinceLastTick = 0.0f;
	
	UPROPERTY()
	float MinimumDialogueLength = 0.7f;
	
	// Dialogue Properties
	
	UPROPERTY()
	bool IsMainCharacterInDialogue = false;
	
	UPROPERTY()
	FName LastDialogueNPC_Name;
	
	//UPROPERTY()
	//TSoftObjectPtr<UDataTable> DT_Disturb;
	
	UPROPERTY()
	APlayerController* PlayerController;
	
	
	// FUNCTIONS!!!
public:
	
	UFUNCTION(BlueprintCallable, Category = "Interaction")
	void Interacted();

	UFUNCTION(BlueprintCallable)
	void GettingVariables();

	UFUNCTION(BlueprintCallable)
	void StartDialogue();

	UFUNCTION(BlueprintCallable)
	void FinishDialogue();
	
	UFUNCTION(BlueprintCallable)
	void ControlDialogue();
	
	UFUNCTION(BlueprintCallable)
	void SkipDialogue();
	
	UFUNCTION(BlueprintCallable)
	void MakeChoice(EChosenOption ChosenButton);
	
	UFUNCTION(BlueprintCallable, Category = "Dialogue System|UI")
	void InitiliazeDialogueUI(TSubclassOf<UWidget_Dialogue> InDialogueWidgetClass);
	
	UFUNCTION(BlueprintCallable, Category = "Dialogue Scoring")
	FName ScoreNPC_Dialogues();

	UFUNCTION(BlueprintCallable, Category = "Dialogue Scoring")
	void AddScoreValue(int ScoreToAdd);

	UFUNCTION(BlueprintCallable, Category = "Dialogue Scoring")
	void GetBestDialogue_RowProperties(const FNPC_Dialogues& BestNPC_Row);

	UFUNCTION(BlueprintCallable, Category = "Dialogue Scoring")
	void GetNextChoice_RowProperties(const FMainCharacterChoices& NextChoice_Row);

	UFUNCTION(BlueprintCallable, Category = "Dialogue Scoring")
	bool FilterDialogues();
	
	UFUNCTION(BlueprintCallable, Category = "Dialogue Scoring")
	bool FilterChoices(FName ChoiceID);

	
	
	// Timer Functions
	UFUNCTION(BlueprintCallable, Category = "Timer Functions")
	void ShowChoiceAfterSeconds();

	UFUNCTION(BlueprintCallable, Category = "Timer Functions")
	void CloseDialogueAfterSeconds();
	
	UFUNCTION(BlueprintCallable, Category = "Timer Functions")
	void ShowNextDialogueAfterSeconds(FName NextDialogueID);
	
	UFUNCTION(BlueprintCallable)
	float CalculateDialogueDuration(FText DialogueText);

	UFUNCTION()
	void ShowDialogue(FText DialogueToShow,FName OwnerOfDialogue);

	//EventManager functions
	
	UFUNCTION(Category = "Event Manager Functions")
	void HandleGameEvent(FGameplayTag EventTag);

	UFUNCTION(Category = "Event Manager Functions")
	void EventReceived(FGameplayTag EventTag);
	
	// if the bool value is true, that means lock the main character. If it is false, then that means release the main character
	UFUNCTION()
	void LockOrReleaseTheMainCharacter(bool LockOrReleaseValue);

	//
	UFUNCTION()
	void OpenOrCloseCursor(bool OpenOrCloseValue);
	
	UFUNCTION()
	void PlayDisturbDialogue();
	
	UFUNCTION()
	FString GetCurrentMap();
	
	
	
	UFUNCTION()
	static void PrintString(const FString& Message, float time, FColor Color);
};
