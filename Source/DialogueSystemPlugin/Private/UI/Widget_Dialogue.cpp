#include "UI/Widget_Dialogue.h"
#include "Subsystems/Subsystem_Dialogue.h"

void UWidget_Dialogue::NativeConstruct()
{
	Super::NativeConstruct();

	if (Choice1)
	{
		Choice1->OnClicked.RemoveDynamic(this, &UWidget_Dialogue::OnChoice1Clicked);
		Choice1->OnClicked.AddDynamic(this, &UWidget_Dialogue::OnChoice1Clicked);
	}
    
	if (Choice2)
	{
		Choice2->OnClicked.RemoveDynamic(this, &UWidget_Dialogue::OnChoice2Clicked);
		Choice2->OnClicked.AddDynamic(this, &UWidget_Dialogue::OnChoice2Clicked);
	}
    
        
	if (Choice3)
	{
		Choice3->OnClicked.RemoveDynamic(this, &UWidget_Dialogue::OnChoice3Clicked);
        Choice3->OnClicked.AddDynamic(this, &UWidget_Dialogue::OnChoice3Clicked);
	}

	CloseDialogue();
	CloseChoices();
	
	
	UGameInstance* GI = GetGameInstance();
	if (GI)
	{
		Dialogue_Subsystem = GI->GetSubsystem<USubsystem_Dialogue>();
	}
}

void UWidget_Dialogue::NativePreConstruct()
{
	Super::NativePreConstruct();
}

// Widget Functions

void UWidget_Dialogue::ShowDialogue(FText DialogueToDisplay)
{
	DialogueText->SetVisibility(ESlateVisibility::Visible);
	DialogueText->SetText(DialogueToDisplay);
	DialogueBorder->SetVisibility(ESlateVisibility::Visible);
	SkipDialogueText->SetVisibility(ESlateVisibility::Visible);
	
	PlayAnimation(ShowDialogueAnim,0.0f,1,EUMGSequencePlayMode::Forward, 1.0f);
	PlayAnimation(ShowSkipDialogueAnim,0.0f,1,EUMGSequencePlayMode::Forward, 1.0f);
	
}

void UWidget_Dialogue::CloseDialogue()
{
	DialogueText->SetVisibility(ESlateVisibility::Hidden);
	DialogueText->SetText(FText());
	DialogueBorder->SetVisibility(ESlateVisibility::Hidden);
	SkipDialogueText->SetVisibility(ESlateVisibility::Hidden);
	
	PlayAnimation(CloseDialogueAnim,0.0f,1,EUMGSequencePlayMode::Forward, 1.0f);
	PlayAnimation(CloseSkipDialogueAnim,0.0f,1,EUMGSequencePlayMode::Forward, 1.0f);
}

void UWidget_Dialogue::ShowChoices(FText Choice1_Text, FText Choice2_Text, FText Choice3_Text)
{
	Choice1->SetVisibility(ESlateVisibility::Visible);
	Choice2->SetVisibility(ESlateVisibility::Visible);
	Choice3->SetVisibility(ESlateVisibility::Visible);

	Choice_1_text->SetVisibility(ESlateVisibility::Visible);
	Choice_2_text->SetVisibility(ESlateVisibility::Visible);
	Choice_3_text->SetVisibility(ESlateVisibility::Visible);

	Choice_1_text->SetText(Choice1_Text);
	Choice_2_text->SetText(Choice2_Text);
	Choice_3_text->SetText(Choice3_Text);
	
	PlayAnimation(ShowChoicesAnim,0.0f,1,EUMGSequencePlayMode::Forward, 1.0f);
}

void UWidget_Dialogue::CloseChoices()
{
	Choice1->SetVisibility(ESlateVisibility::Hidden);
	Choice2->SetVisibility(ESlateVisibility::Hidden);
	Choice3->SetVisibility(ESlateVisibility::Hidden);

	Choice_1_text->SetVisibility(ESlateVisibility::Hidden);
	Choice_2_text->SetVisibility(ESlateVisibility::Hidden);
	Choice_3_text->SetVisibility(ESlateVisibility::Hidden);

	Choice_1_text->SetText(FText());
	Choice_2_text->SetText(FText());
	Choice_3_text->SetText(FText());
	
	PlayAnimation(CloseChoicesAnim,0.0f,1,EUMGSequencePlayMode::Forward, 1.0f);
}

void UWidget_Dialogue::OnChoice1Clicked()
{
	if (Dialogue_Subsystem)
	{
		PrintString("Choice1 clicked and Subsystem is valid",2.f,FColor::Green);
		Dialogue_Subsystem->MakeChoice(EChosenOption::Choice1);
	}
	else
	{
		
		PrintString("Choice1 clicked and Subsystem is NOT valid",2.f,FColor::Green);
	}
}

void UWidget_Dialogue::OnChoice2Clicked()
{
	if (Dialogue_Subsystem)
	{
		PrintString("Choice2 clicked and Subsystem is valid",2.f,FColor::Yellow);
		Dialogue_Subsystem->MakeChoice(EChosenOption::Choice2);
	}
	else
	{
		PrintString("Choice2 clicked and Subsystem is NOT valid",2.f,FColor::Yellow);
	}
}

void UWidget_Dialogue::OnChoice3Clicked()
{
	if (Dialogue_Subsystem)
	{
		PrintString("Choice3 clicked and Subsystem is valid",2.f,FColor::Blue);
		Dialogue_Subsystem->MakeChoice(EChosenOption::Choice3);
	}
	else
	{
		PrintString("Choice3 clicked and Subsystem is NOT valid",2.f,FColor::Blue);
	}
}



void UWidget_Dialogue::PrintString(const FString& Message, float Time, FColor Color)
{
	if (!GEngine)
	{
		return;
	}

	GEngine->AddOnScreenDebugMessage(-1,Time, Color, Message);
}