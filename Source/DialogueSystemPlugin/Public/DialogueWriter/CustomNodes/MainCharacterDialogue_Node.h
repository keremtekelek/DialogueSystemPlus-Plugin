#pragma once

#include "CoreMinimal.h"
#include "K2Node.h"
#include "EnumsAndStructs/GlobalEnumsAndStructs.h"
#include "GameplayTagContainer.h"
#include "ToolMenu.h"
#include "ToolMenuSection.h"
#include "EdGraphSchema_K2.h"
#include "MainCharacterDialogue_Node.generated.h"

class UDialogueWriter;
class UMainCharacterChoices_Node;
class UNPC_DialogueNode;

UCLASS(BlueprintType, Blueprintable)
class DIALOGUESYSTEMPLUGIN_API UMainCharacterDialogue_Node : public UK2Node
{
	GENERATED_BODY()
	
public:
	
	UMainCharacterDialogue_Node();
	
	
	//VARIABLES!!!
public:
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialogue Struct")
	FNPC_Dialogues MC_DialogueRow;
	
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
		return FLinearColor(0.7f, 0.2f, 0.3f); 
	}

	virtual bool ShouldShowNodeProperties() const override { return true; }
	
	virtual void PostEditImport() override;
	
	virtual bool IsNodePure() const override { return true; }
	
	virtual void GetNodeContextMenuActions(UToolMenu* Menu, UGraphNodeContextMenuContext* Context) const override;
};
