#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "Engine/DataTable.h"
#include "DialogueSystemPluginSettings.generated.h"

UCLASS(Config=Game, defaultconfig, meta=(DisplayName="Dialogue System Plugin Settings"))
class DIALOGUESYSTEMPLUGIN_API UDialogueSystemSettings : public UDeveloperSettings
{
	GENERATED_BODY()

public:

	
	UPROPERTY(EditAnywhere, Config, Category = "Data Tables")
	TSoftObjectPtr<UDataTable> DT_MainCharacterChoices;

	UPROPERTY(EditAnywhere, Config, Category = "Data Tables")
	TSoftObjectPtr<UDataTable> DT_MainCharacterDialogues;
	
	UPROPERTY(EditAnywhere, Config, Category = "Data Tables")
	TSoftObjectPtr<UDataTable> DT_Disturb;

	UPROPERTY(EditAnywhere, Config, Category = "Player Character Settings")
	FName PlayerCharacterName;
};