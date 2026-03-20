#pragma once

#include "CoreMinimal.h"
#include "K2Node.h"
#include "EnumsAndStructs/GlobalEnumsAndStructs.h"
#include "EdGraphSchema_K2.h"
#include "BlueprintActionDatabaseRegistrar.h"
#include "BlueprintNodeSpawner.h"
#include "MainCharacterChoices_Node.generated.h"

class UNPC_DialogueNode;
class UDialogueWriter;

UCLASS(BlueprintType, Blueprintable)
class DIALOGUESYSTEMPLUGIN_API UMainCharacterChoices_Node : public UK2Node
{
	GENERATED_BODY()
	
public:
	
	UMainCharacterChoices_Node();
	
	//VARIABLES!!!
public:
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Choice Struct")
	FMainCharacterChoices AllChoice_Row;
	
	
	//FUNCTIONS!!!
public:
	
	virtual bool IsConnectionDisallowed(const UEdGraphPin* MyPin, const UEdGraphPin* OtherPin, FString& OutReason) const override;
	
	virtual FText GetNodeTitle(ENodeTitleType::Type TitleType) const override;
	virtual void AllocateDefaultPins() override;
	virtual void GetMenuActions(FBlueprintActionDatabaseRegistrar& ActionRegistrar) const override;
	virtual FText GetMenuCategory() const override;
	virtual bool IsCompatibleWithGraph(const UEdGraph* TargetGraph) const override;
	virtual bool ShouldShowNodeProperties() const override { return true; }
	virtual FLinearColor GetNodeTitleColor() const override { return FLinearColor(0.1f, 0.6f, 0.2f); }
	virtual void PostEditImport() override;
	virtual bool IsNodePure() const override { return true; }
};
