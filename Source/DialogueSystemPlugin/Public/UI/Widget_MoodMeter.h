#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/CanvasPanel.h" 
#include "Components/TextBlock.h"
#include "Components/HorizontalBox.h"
#include "Widget_MoodMeter.generated.h"


UCLASS()
class DIALOGUESYSTEMPLUGIN_API UWidget_MoodMeter : public UUserWidget
{
	GENERATED_BODY()
	
protected:
	virtual void NativePreConstruct() override;
	virtual void NativeConstruct() override;
	
public:

	UPROPERTY(meta = (BindWidget))
	UCanvasPanel* Canvas;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* MoodValue;
	
	UPROPERTY(meta = (BindWidget))
	UTextBlock* StaticText;
	
	UPROPERTY(meta = (BindWidget))
	UHorizontalBox* HorizontalBox;

public:

	UFUNCTION(BlueprintCallable)
	void ShowMoodValue(bool ShouldShowMoodValue, FText TextToDisplay);
};
