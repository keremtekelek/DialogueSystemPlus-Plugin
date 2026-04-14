#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "Engine/DataTable.h"
#include "Engine/World.h"
#include "DialogueSystemPluginSettings.generated.h"

UCLASS(Config=Game, defaultconfig, meta=(DisplayName="Dialogue System Plugin Settings"))
class DIALOGUESYSTEMPLUGIN_API UDialogueSystemSettings : public UDeveloperSettings
{
	GENERATED_BODY()

public:

	
	UPROPERTY(EditAnywhere, Config, Category = "Main Character Data")
	TMap<TSoftObjectPtr<UWorld>, TSoftObjectPtr<UDataTable>> DT_MainCharacterChoicesMap;

	UPROPERTY(EditAnywhere, Config, Category = "Main Character Data")
	TMap<TSoftObjectPtr<UWorld>, TSoftObjectPtr<UDataTable>> DT_MainCharacterDialogueMap;
	
	UPROPERTY(EditAnywhere, Config, Category = "Data Tables")
	TSoftObjectPtr<UDataTable> DT_Disturb;

	UPROPERTY(EditAnywhere, Config, Category = "Player Character Settings")
	FName PlayerCharacterName;
};