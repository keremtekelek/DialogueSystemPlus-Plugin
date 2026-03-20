#pragma once

#include "CoreMinimal.h"
#include "K2Node.h"
#include "EnumsAndStructs/GlobalEnumsAndStructs.h"
#include "EdGraphSchema_K2.h"
#include "Engine/Blueprint.h"
#include "BlueprintActionDatabaseRegistrar.h" 
#include "BlueprintNodeSpawner.h"
#include "ToolMenu.h"
#include "ToolMenuSection.h"
#include "ScopedTransaction.h"
#include "Kismet2/KismetEditorUtilities.h"
#include "NPC_DialogueNode.generated.h"

class UDialogueWriter;
class UMainCharacterChoices_Node;
class UMainCharacterDialogue_Node;


UCLASS(BlueprintType, Blueprintable)
class DIALOGUESYSTEMPLUGIN_API UNPC_DialogueNode : public UK2Node
{
	GENERATED_BODY()
	
public:
	
	UNPC_DialogueNode();
	
	//VARIABLES!!!
public:
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialogue Struct")
	FNPC_Dialogues NPC_Row;
	

	UPROPERTY()
	TArray<FName> ManualAdded_RelatedNPC_Dialogues;
	
	//FUNCTIONS!!!
public:
	
	virtual FText GetNodeTitle(ENodeTitleType::Type TitleType) const override;

	virtual bool IsConnectionDisallowed(const UEdGraphPin* MyPin, const UEdGraphPin* OtherPin, FString& OutReason) const override;
	
	virtual void AllocateDefaultPins() override;


	virtual void GetMenuActions(FBlueprintActionDatabaseRegistrar& ActionRegistrar) const override;
	
	virtual FText GetMenuCategory() const override;
	
	virtual bool IsCompatibleWithGraph(const UEdGraph* TargetGraph) const override;

	virtual FLinearColor GetNodeTitleColor() const override 
	{ 
		return FLinearColor(0.2f, 0.2f, 0.8f); 
	}

	virtual bool ShouldShowNodeProperties() const override { return true; }
	
	virtual void PostEditImport() override;
	
	virtual bool IsNodePure() const override { return true; }
	
	virtual void GetNodeContextMenuActions(UToolMenu* Menu, UGraphNodeContextMenuContext* Context) const override;
};
