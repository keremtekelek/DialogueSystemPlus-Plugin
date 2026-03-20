#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "EnumsAndStructs/GlobalEnumsAndStructs.h"
#include "Kismet/GameplayStatics.h"
#include "Components/Button.h"        
#include "Components/CanvasPanel.h"   
#include "Components/TextBlock.h"
#include "Components/Border.h"
#include "Components/SizeBox.h"
#include "Animation/WidgetAnimation.h"
#include "Components/RichTextBlock.h"
#include "Widget_Dialogue.generated.h"


class USubsystem_Dialogue;
UCLASS()
class DIALOGUESYSTEMPLUGIN_API UWidget_Dialogue : public UUserWidget
{
	GENERATED_BODY()
	
protected:
	
	virtual void NativePreConstruct() override;
	virtual void NativeConstruct() override;
	
	//VARIABLES!!!
public:

	// Components
	UPROPERTY(meta = (BindWidget))
	UCanvasPanel* Canvas;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* DialogueText;

	UPROPERTY(meta = (BindWidget))
	UBorder* DialogueBorder;
	
	UPROPERTY(meta = (BindWidget))
	USizeBox* DialogueSizeBox;

	UPROPERTY(meta = (BindWidget))
	UButton* Choice1;

	UPROPERTY(meta = (BindWidget))
	UButton* Choice2;

	UPROPERTY(meta = (BindWidget))
	UButton* Choice3;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* Choice_1_text;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* Choice_2_text;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* Choice_3_text;

	UPROPERTY(meta = (BindWidget))
	UBorder* Choice1Border;

	UPROPERTY(meta = (BindWidget))
	UBorder* Choice2Border;

	UPROPERTY(meta = (BindWidget))
	UBorder* Choice3Border;
	
	UPROPERTY(meta = (BindWidget))
	URichTextBlock* SkipDialogueText;
	

	UPROPERTY()
	EChosenOption ChosenButton;

	
	UPROPERTY()
	USubsystem_Dialogue* Dialogue_Subsystem;
	
	UPROPERTY(Transient, meta = (BindWidgetAnim))
	UWidgetAnimation* ShowChoicesAnim;
	
	UPROPERTY(Transient, meta = (BindWidgetAnim))
	UWidgetAnimation* CloseChoicesAnim;
	
	UPROPERTY(Transient, meta = (BindWidgetAnim))
	UWidgetAnimation* ShowDialogueAnim;
	
	UPROPERTY(Transient, meta = (BindWidgetAnim))
	UWidgetAnimation* CloseDialogueAnim;
	
	UPROPERTY(Transient, meta = (BindWidgetAnim))
	UWidgetAnimation* ShowSkipDialogueAnim;
	
	UPROPERTY(Transient, meta = (BindWidgetAnim))
	UWidgetAnimation* CloseSkipDialogueAnim;
	
	//FUNCTIONS!!!
public:
	
	UFUNCTION(BlueprintCallable)
	void ShowDialogue(FText DialogueToDisplay);

	UFUNCTION(BlueprintCallable)
	void CloseDialogue();

	UFUNCTION()
	void ShowChoices(FText Choice1_Text, FText Choice2_Text, FText Choice3_Text);

	UFUNCTION()
	void CloseChoices();

	//OnClicked() Events
	UFUNCTION()
	void OnChoice1Clicked();

	UFUNCTION()
	void OnChoice2Clicked();

	UFUNCTION()
	void OnChoice3Clicked();



	//DEBUG Functions
	UFUNCTION()
	static void PrintString(const FString& Message, float time, FColor Color);
};
