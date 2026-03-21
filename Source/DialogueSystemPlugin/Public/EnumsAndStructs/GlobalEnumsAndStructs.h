#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "Sound/SoundBase.h"
#include "GameplayTagContainer.h"
#include "GlobalEnumsAndStructs.generated.h"

UENUM(BlueprintType)
enum class ENPCMood : uint8
{
    DoesntMatter   UMETA(DisplayName = "Doesn't Matter"),
	Neutral        UMETA(DisplayName = "Neutral"),    
	Happy	       UMETA(DisplayName = "Happy"),   
	NotHappy	   UMETA(DisplayName = "Not Happy")
	 
};

UENUM(BlueprintType)
enum class EChosenOption : uint8
{
	None        UMETA(DisplayName = "None"),    
	Choice1	    UMETA(DisplayName = "Choice1"),   
	Choice2		UMETA(DisplayName = "Choice2"),
	Choice3		UMETA(DisplayName = "Choice3"),
};

USTRUCT(BlueprintType)
struct FDialogueFlow
{
	GENERATED_BODY()
};


// NPC Dialogue Struct

USTRUCT(BlueprintType)
struct FNPC_Dialogues : public FTableRowBase
{
	GENERATED_BODY()
 
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText DialogueText;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite) 
    FName SpeakerName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USoundBase* DialogueSound;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FGameplayTagContainer RelatedGlobalEvents;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FGameplayTagContainer EventsToTrigger;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FGameplayTagContainer HasToRequireEvents;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	ENPCMood DesiredNPC_Mood;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSoftObjectPtr<UWorld> TargetLevel;
	
	
	UPROPERTY(BlueprintReadOnly, Category="Auto Generated Dialogue Data") 
	FName DialogueID;
	
	UPROPERTY(BlueprintReadOnly, Category="Auto Generated Dialogue Data") 
	TArray<FName> RelatedNPC_Dialogues;

	UPROPERTY(BlueprintReadOnly, Category="Auto Generated Dialogue Data") 
	FName NextDialogueID;
	
	UPROPERTY(BlueprintReadOnly, Category="Auto Generated Dialogue Data") 
	FName NextChoiceID;
	
	UPROPERTY(BlueprintReadOnly, Category="Auto Generated Dialogue Data") 
	bool IsRoot;
	
	UPROPERTY(BlueprintReadOnly, Category="Auto Generated Dialogue Data") 
	bool EndOfDialogue;
};

//Distrub Struct
USTRUCT(BlueprintType)
struct FDisturb : public FTableRowBase
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText DisturbText;
	
};


//**CHOICE STRUCTS**//

USTRUCT(BlueprintType)
struct FChoice1
{
	GENERATED_BODY()
 
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText ChoiceText;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int EffectsMood = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FGameplayTagContainer EventsToTrigger;
	
	UPROPERTY(BlueprintReadOnly, Category="Auto Generated Choice Data") 
    FName SpeakerName;

	UPROPERTY(BlueprintReadOnly,Category="Auto Generated Choice Data") 
	bool EndOfDialogue = false;

	UPROPERTY(BlueprintReadOnly,Category="Auto Generated Choice Data")  
	FName ChoiceID1;
	
	UPROPERTY(BlueprintReadOnly, Category="Auto Generated Choice Data") 
	FName NextDialogueID;
	
	UPROPERTY(BlueprintReadOnly, Category="Auto Generated Choice Data")
	TSoftObjectPtr<UWorld> TargetLevel;
};

USTRUCT(BlueprintType)
struct FChoice2
{
	GENERATED_BODY()
 
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText ChoiceText;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int EffectsMood = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FGameplayTagContainer EventsToTrigger;
	
	UPROPERTY(BlueprintReadOnly, Category="Auto Generated Choice Data") 
    FName SpeakerName;

	UPROPERTY(BlueprintReadOnly,Category="Auto Generated Choice Data") 
	bool EndOfDialogue = false;

	UPROPERTY(BlueprintReadOnly,Category="Auto Generated Choice Data") 
	FName ChoiceID2;
	
	UPROPERTY(BlueprintReadOnly, Category="Auto Generated Choice Data") 
	FName NextDialogueID;
	
	UPROPERTY(BlueprintReadOnly, Category="Auto Generated Choice Data")
	TSoftObjectPtr<UWorld> TargetLevel;
	
};

USTRUCT(BlueprintType)
struct FChoice3
{
	GENERATED_BODY()
 
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText ChoiceText;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int EffectsMood = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FGameplayTagContainer EventsToTrigger;
	
	UPROPERTY(BlueprintReadOnly, Category="Auto Generated Choice Data") 
    FName SpeakerName;

	UPROPERTY(BlueprintReadOnly,Category="Auto Generated Choice Data") 
	bool EndOfDialogue = false;

	UPROPERTY(BlueprintReadOnly,Category="Auto Generated Choice Data") 
	FName ChoiceID3;
	
	UPROPERTY(BlueprintReadOnly, Category="Auto Generated Choice Data") 
	FName NextDialogueID;
	
	UPROPERTY(BlueprintReadOnly, Category="Auto Generated Choice Data")
	TSoftObjectPtr<UWorld> TargetLevel;
	
	
};
 
USTRUCT(BlueprintType)
struct FMainCharacterChoices : public FTableRowBase
{
	GENERATED_BODY()
 
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FChoice1 Choice1;
 
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FChoice2 Choice2;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FChoice3 Choice3;
};