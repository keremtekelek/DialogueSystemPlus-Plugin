#include "DialogueSystemPlugin.h"
#include "Modules/ModuleManager.h"

#if WITH_EDITOR
	#include "AssetToolsModule.h"
	#include "IAssetTools.h"
	#include "DialogueWriter/AssetActions/DialogueWriter_AssetAction.h" 
#endif

#define LOCTEXT_NAMESPACE "FDialogueSystemPluginModule"

void FDialogueSystemPluginModule::StartupModule()
{
	#if WITH_EDITOR
	
		IAssetTools& AssetTools = FModuleManager::LoadModuleChecked<FAssetToolsModule>("AssetTools").Get();
		EAssetTypeCategories::Type DialogueCategory = AssetTools.RegisterAdvancedAssetCategory(FName(TEXT("DialogueSystem")), FText::FromString("Dialogue System Plus"));

		TSharedPtr<IAssetTypeActions> Action = MakeShareable(new FAssetAction_DialogueWriter(DialogueCategory));
		AssetTools.RegisterAssetTypeActions(Action.ToSharedRef());
	
	#endif
}

void FDialogueSystemPluginModule::ShutdownModule()
{
	
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FDialogueSystemPluginModule, DialogueSystemPlugin)