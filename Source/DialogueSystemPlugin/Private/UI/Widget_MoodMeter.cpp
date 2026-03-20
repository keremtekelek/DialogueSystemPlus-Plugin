#include "UI/Widget_MoodMeter.h"

void UWidget_MoodMeter::NativeConstruct()
{
	Super::NativeConstruct();
	
	if (IsDesignTime())
	{
		return;
	}
	
	this->SetVisibility(ESlateVisibility::Visible);
	ShowMoodValue(false, FText());
}

void UWidget_MoodMeter::NativePreConstruct()
{
	Super::NativePreConstruct();
}

void UWidget_MoodMeter::ShowMoodValue(bool ShouldShowMoodValue, FText TextToDisplay)
{
	if (!MoodValue || !StaticText)
	{
		return; 
	}
	
	if (ShouldShowMoodValue)
	{
		MoodValue->SetVisibility(ESlateVisibility::Visible);
		MoodValue->SetText(TextToDisplay);
		
		StaticText->SetVisibility(ESlateVisibility::Visible);
	}
	else
	{
		MoodValue->SetVisibility(ESlateVisibility::Hidden);
		MoodValue->SetText(FText());
		
		StaticText->SetVisibility(ESlateVisibility::Hidden);
	}
}