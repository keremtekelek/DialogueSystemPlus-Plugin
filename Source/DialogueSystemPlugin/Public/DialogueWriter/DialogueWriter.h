#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "EnumsAndStructs/GlobalEnumsAndStructs.h"
#include "K2Node_CallFunction.h"
#include "EdGraph/EdGraph.h"
#include "EdGraphSchema_K2.h"
#include "Engine/DataTable.h"
#include "Sound/SoundBase.h"
#include "DataTableEditorUtils.h"
#include "DialogueWriter.generated.h"

class UBlueprint;
class UNPC_DialogueNode;
class UMainCharacterChoices_Node;
class UMainCharacterDialogue_Node;
class UAC_DialogueSystem;



struct FDialogueEditorClipboard
{
	static FName CopiedID;
	static FString CopiedNodeType;

	static void Reset()
	{
		CopiedID = NAME_None;
		CopiedNodeType.Empty();
	}
};

UCLASS(Blueprintable, BlueprintType, Abstract)
class DIALOGUESYSTEMPLUGIN_API UDialogueWriter : public UObject
{
	GENERATED_BODY()
	
	//VARIABLES!!!
public:
	
	UPROPERTY()
	TMap<FName, UDataTable*> NPC_DataTableMap;
	
	UPROPERTY()
	UDataTable* DT_MainCharacterChoices;

	UPROPERTY()
	UDataTable* DT_MainCharacterDialogues;
	
	UPROPERTY()
	TArray<UEdGraphNode*> VisitedNPC_Nodes;

	UPROPERTY()
	TArray<UEdGraphNode*> VisitedMC_Nodes;

	UPROPERTY()
	TArray<UEdGraphNode*> VisitedMC_DialogueNodes;
	
	UPROPERTY()
	TArray<UNPC_DialogueNode*> RootNPC_Nodes;
	
	UPROPERTY()
	TSet<FName> ActiveIDs;
	
	
	
	//FUNCTIONS!!!
public:
	
	UFUNCTION(BlueprintCallable, CallInEditor, Category = "Dialogue Tools")
	void GenerateDialogueData();

	UFUNCTION(BlueprintCallable, Category = "Data Table Functions")
	void ClearDataTables();

	UFUNCTION(BlueprintCallable, Category = "Data Table Functions")
	void MarkDataTablesAsDirty();
	
	UFUNCTION(BlueprintCallable, Category = "Data Table Functions")
	void AddToDataTable(UEdGraphNode* NodeToAddDataTable);
	
	UFUNCTION(BlueprintCallable, Category = "Data Table Functions")
	void HandleAutomatedData(UEdGraphNode* HandledNode);
	
	UFUNCTION(BlueprintCallable, Category = "Data Table Functions")
	void CleanGhostNodesFromTables();
	
	UFUNCTION()
	void TakeNPCsDataTablesAndName();
	
	
protected:
	
	virtual void PostInitProperties() override;
	
private:
	
	#if WITH_EDITOR
		void OnBlueprintCompiled(UBlueprint* InBlueprint);
	#endif
};
