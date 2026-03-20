#include "DialogueWriter/DialogueWriter.h"
#include "DialogueWriter/CustomNodes/NPC_DialogueNode.h"
#include "DialogueWriter/CustomNodes/MainCharacterChoices_Node.h"
#include "DialogueWriter/CustomNodes/MainCharacterDialogue_Node.h"
#include "ActorComponents/AC_DialogueSystem.h"
#include "Interfaces/Interface_MainCharacter.h"
#include "Settings/DialogueSystemPluginSettings.h"

#if WITH_EDITOR
	#include "Engine/Blueprint.h"
	#include "Kismet2/KismetEditorUtilities.h" 
	#include "Editor.h"
	#include "EngineUtils.h"
#endif


void UDialogueWriter::GenerateDialogueData()
{
#if WITH_EDITOR
	UBlueprint* BP_DialogueWriter = Cast<UBlueprint>(this->GetClass()->ClassGeneratedBy);

	if (!(IsValid(BP_DialogueWriter)))
	{
		UE_LOG(LogTemp, Warning, TEXT("BP_DialogueWriter is not valid!"))
		return;
	}

	VisitedNPC_Nodes.Empty();
	VisitedMC_Nodes.Empty();
	VisitedMC_DialogueNodes.Empty();
	RootNPC_Nodes.Empty(); 
	ActiveIDs.Empty(); 
	
	// Checking for if graph has some nodes.
	bool GraphHasNodes = false;
	
    for (UEdGraph* Graph : BP_DialogueWriter->UbergraphPages)
    {
    	if (Graph && Graph->Nodes.Num() > 0)
    	{
    		GraphHasNodes = true;
    		break;
    	}
    }
    
    if(!GraphHasNodes)
    {
		ClearDataTables();
    	UE_LOG(LogTemp, Warning, TEXT("Graph has no nodes. Data Tables are cleared"))
    	
    	return;
    }
	
	for (UEdGraph* Graph : BP_DialogueWriter->UbergraphPages)
	{
		if (!Graph) continue;

		
		for (UEdGraphNode* Node : Graph->Nodes)
		{
			
			if (UMainCharacterChoices_Node* ChoiceNode = Cast<UMainCharacterChoices_Node>(Node))
			{
				UEdGraphPin* InputPin = ChoiceNode->FindPin(UEdGraphSchema_K2::PN_Execute);

				if (InputPin && InputPin->LinkedTo.Num() == 0)
				{
					UE_LOG(LogTemp, Error, TEXT("ERROR: Choice Node not cannot be Root node."))

					FText ErrorMsg = FText::FromString(TEXT("ERROR: Choice Node cannot be Root node"));
					FMessageDialog::Open(EAppMsgType::Ok, ErrorMsg);
					
					FKismetEditorUtilities::BringKismetToFocusAttentionOnObject(ChoiceNode);
				}
			}
			else if (UMainCharacterDialogue_Node* MC_DialogueNode = Cast<UMainCharacterDialogue_Node>(Node))
			{
				UEdGraphPin* InputPin =  MC_DialogueNode->FindPin(UEdGraphSchema_K2::PN_Execute);

				if (InputPin && InputPin->LinkedTo.Num() == 0)
				{
					UE_LOG(LogTemp, Error, TEXT("ERROR: Player Dialogue Node not cannot be Root node."))

					FText ErrorMsg = FText::FromString(TEXT("ERROR: Player Dialogue Node not cannot be Root node."));
					FMessageDialog::Open(EAppMsgType::Ok, ErrorMsg);
					
					FKismetEditorUtilities::BringKismetToFocusAttentionOnObject(MC_DialogueNode);
				}
				
				if (MC_DialogueNode->MC_DialogueRow.NextDialogueID == MC_DialogueNode->MC_DialogueRow.DialogueID)
				{
					UE_LOG(LogTemp, Error, TEXT("ERROR: Player Dialogue Node's NextDialogueID and DialogueID is equal!"))
					
					FText ErrorMsg = FText::FromString(TEXT("ERROR: Player Dialogue Node's NextDialogueID and DialogueID is equal!"));
					FMessageDialog::Open(EAppMsgType::Ok, ErrorMsg);
					FKismetEditorUtilities::BringKismetToFocusAttentionOnObject(MC_DialogueNode);
				}
			}
			else if (UNPC_DialogueNode* npc_DialogueNode = Cast<UNPC_DialogueNode>(Node))
			{
				UEdGraphPin* InputPin = npc_DialogueNode->FindPin(UEdGraphSchema_K2::PN_Execute);
				
				if (!InputPin || InputPin->LinkedTo.Num() == 0)
				{
					if (npc_DialogueNode->NPC_Row.TargetLevel == nullptr)
					{
						UE_LOG(LogTemp, Error, TEXT("ERROR: NPC Dialogue Node's Target Level is Null!"))
					
						
						FText ErrorMsg = FText::FromString(TEXT("ERROR: NPC Dialogue Node's Target Level is Null!"));
						FMessageDialog::Open(EAppMsgType::Ok, ErrorMsg);
						
						FKismetEditorUtilities::BringKismetToFocusAttentionOnObject(npc_DialogueNode);
					}
				}
			}
		}
	}

	// Getting Root NPC Nodes
	for (UEdGraph* Graph : BP_DialogueWriter->UbergraphPages)
	{
		if (!Graph)
		{
			continue;
		}
		
		for (UEdGraphNode* Node : Graph->Nodes)
		{
			if (!IsValid(Node))
			{
				continue;
			}
			
			if (UNPC_DialogueNode* NPCNode = Cast<UNPC_DialogueNode>(Node))
			{
				bool IsRoot = false;
				
				UEdGraphPin* InputPin = NPCNode->FindPin(UEdGraphSchema_K2::PN_Execute);
			
				if (!InputPin || InputPin->LinkedTo.Num() == 0)
				{
					IsRoot = true;
				
					RootNPC_Nodes.Add(NPCNode);
				}
			}
		}
	}
			
	for (UNPC_DialogueNode* Node : RootNPC_Nodes)
	{
		Node->NPC_Row.IsRoot = true;
		HandleAutomatedData(Node);
	}

	CleanGhostNodesFromTables();
		
	MarkDataTablesAsDirty();

	UE_LOG(LogTemp, Warning, TEXT("GenerateDialogueData Worked!"));
#endif
}

void UDialogueWriter::PostInitProperties()
{
	Super::PostInitProperties();
	
#if WITH_EDITOR
	if (HasAnyFlags(RF_ClassDefaultObject))
	{
		UClass* MyClass = GetClass();
		FString ClassName = MyClass->GetName();

		if (ClassName.StartsWith(TEXT("SKEL_")))
		{
			return;
		}

		if (ClassName.StartsWith(TEXT("REINST_")))
		{
			return;
		}

		if (UBlueprint* BP = Cast<UBlueprint>(MyClass->ClassGeneratedBy))
		{
			if (!BP->OnCompiled().IsBoundToObject(this))
			{
				BP->OnCompiled().AddUObject(this, &UDialogueWriter::OnBlueprintCompiled);
			}
		}
	}
#endif
}

void UDialogueWriter::HandleAutomatedData(UEdGraphNode* HandledNode)
{
	if (UNPC_DialogueNode* NPCNode = Cast<UNPC_DialogueNode>(HandledNode))
	{
		if (!NPCNode || VisitedNPC_Nodes.Contains(NPCNode))
		{
			return;
		}
		VisitedNPC_Nodes.Add(NPCNode);
		
		ActiveIDs.Add(NPCNode->NPC_Row.DialogueID);
		
		UEdGraphPin* ExecPin = NPCNode->FindPin(UEdGraphSchema_K2::PN_Then);
		if (ExecPin && ExecPin->LinkedTo.Num() > 0) 
		{
			NPCNode->NPC_Row.EndOfDialogue = false;
			
			for (UEdGraphPin* LinkedPin : ExecPin->LinkedTo)
			{
				if (LinkedPin && LinkedPin->GetOwningNode())
				{
					if (UMainCharacterChoices_Node* Next_MainCharacterChoicesNode = Cast<UMainCharacterChoices_Node>(LinkedPin->GetOwningNode()))
					{
						NPCNode->NPC_Row.NextChoiceID = Next_MainCharacterChoicesNode->AllChoice_Row.Choice1.ChoiceID1;
						
						Next_MainCharacterChoicesNode->AllChoice_Row.Choice1.SpeakerName = NPCNode->NPC_Row.SpeakerName;
						Next_MainCharacterChoicesNode->AllChoice_Row.Choice2.SpeakerName = NPCNode->NPC_Row.SpeakerName;
						Next_MainCharacterChoicesNode->AllChoice_Row.Choice3.SpeakerName = NPCNode->NPC_Row.SpeakerName;
						
						Next_MainCharacterChoicesNode->AllChoice_Row.Choice1.TargetLevel = NPCNode->NPC_Row.TargetLevel;
						Next_MainCharacterChoicesNode->AllChoice_Row.Choice2.TargetLevel = NPCNode->NPC_Row.TargetLevel;
						Next_MainCharacterChoicesNode->AllChoice_Row.Choice3.TargetLevel = NPCNode->NPC_Row.TargetLevel;
						
						AddToDataTable(NPCNode);
						HandleAutomatedData(Next_MainCharacterChoicesNode); 
					}
					else if (UNPC_DialogueNode* Next_NPCNode = Cast<UNPC_DialogueNode>(LinkedPin->GetOwningNode()))
					{
						NPCNode->NPC_Row.NextDialogueID = Next_NPCNode->NPC_Row.DialogueID;
						
						Next_NPCNode->NPC_Row.SpeakerName = NPCNode->NPC_Row.SpeakerName;
						Next_NPCNode->NPC_Row.TargetLevel = NPCNode->NPC_Row.TargetLevel;
						
						AddToDataTable(NPCNode);
						HandleAutomatedData(Next_NPCNode);
					}
					else if (UMainCharacterDialogue_Node* Next_PlayerDialogueNode = Cast<UMainCharacterDialogue_Node>(LinkedPin->GetOwningNode()))
					{
						NPCNode->NPC_Row.NextDialogueID = Next_PlayerDialogueNode->MC_DialogueRow.DialogueID;
						
						Next_PlayerDialogueNode->MC_DialogueRow.SpeakerName = NPCNode->NPC_Row.SpeakerName;
						Next_PlayerDialogueNode->MC_DialogueRow.TargetLevel = NPCNode->NPC_Row.TargetLevel;
						
						AddToDataTable(NPCNode);
						HandleAutomatedData(Next_PlayerDialogueNode);
					}
				}
			}
		}
		else
		{
			NPCNode->NPC_Row.EndOfDialogue = true;
			AddToDataTable(NPCNode);
		}
		
	}
	
	else if (UMainCharacterChoices_Node* MainCharacterChoicesNode = Cast<UMainCharacterChoices_Node>(HandledNode))
	{
		if (!MainCharacterChoicesNode || VisitedMC_Nodes.Contains(MainCharacterChoicesNode))
		{
			return;
		}
		VisitedMC_Nodes.Add(MainCharacterChoicesNode);
		ActiveIDs.Add(MainCharacterChoicesNode->AllChoice_Row.Choice1.ChoiceID1);
		
		const TArray<FName> ChoicePinNames = { TEXT("Choice1"), TEXT("Choice2"), TEXT("Choice3") };
		
		
		
		for (int32 i = 0; i < ChoicePinNames.Num(); i++)
		{
			UEdGraphPin* OutputPin = MainCharacterChoicesNode->FindPin(ChoicePinNames[i]);
			
			if (OutputPin && OutputPin->LinkedTo.Num() > 0)
			{
				MainCharacterChoicesNode->AllChoice_Row.Choice1.EndOfDialogue = false;
				MainCharacterChoicesNode->AllChoice_Row.Choice2.EndOfDialogue = false;
				MainCharacterChoicesNode->AllChoice_Row.Choice3.EndOfDialogue = false;
				
				for (UEdGraphPin* LinkedPin : OutputPin->LinkedTo)
				{
					if (LinkedPin && LinkedPin->GetOwningNode())
					{
						if (UNPC_DialogueNode* Next_NPCNode = Cast<UNPC_DialogueNode>(LinkedPin->GetOwningNode()))
						{
							if (i == 0) // Choice1
							{
								MainCharacterChoicesNode->AllChoice_Row.Choice1.NextDialogueID = Next_NPCNode->NPC_Row.DialogueID;
								Next_NPCNode->NPC_Row.SpeakerName = MainCharacterChoicesNode->AllChoice_Row.Choice1.SpeakerName;
								Next_NPCNode->NPC_Row.TargetLevel = MainCharacterChoicesNode->AllChoice_Row.Choice1.TargetLevel;
							}
							
							else if (i == 1) // Choice 2
							{
								MainCharacterChoicesNode->AllChoice_Row.Choice2.NextDialogueID = Next_NPCNode->NPC_Row.DialogueID;
								Next_NPCNode->NPC_Row.SpeakerName = MainCharacterChoicesNode->AllChoice_Row.Choice2.SpeakerName;
								Next_NPCNode->NPC_Row.TargetLevel = MainCharacterChoicesNode->AllChoice_Row.Choice2.TargetLevel;
							}
							
							else if (i == 2) // Choice 3
							{
								MainCharacterChoicesNode->AllChoice_Row.Choice3.NextDialogueID = Next_NPCNode->NPC_Row.DialogueID;
								Next_NPCNode->NPC_Row.SpeakerName = MainCharacterChoicesNode->AllChoice_Row.Choice3.SpeakerName;
								Next_NPCNode->NPC_Row.TargetLevel = MainCharacterChoicesNode->AllChoice_Row.Choice3.TargetLevel;
							}
							
							AddToDataTable(MainCharacterChoicesNode);
							HandleAutomatedData(Next_NPCNode);
						}
					}
				}
			}
			else
			{
				
			}
		}
	}
	else if (UMainCharacterDialogue_Node* MainCharacterDialogueNode = Cast<UMainCharacterDialogue_Node>(HandledNode))
	{
		if (!MainCharacterDialogueNode || VisitedMC_DialogueNodes.Contains(MainCharacterDialogueNode))
		{
			return;
		}
		VisitedMC_DialogueNodes.Add(MainCharacterDialogueNode);
		
		ActiveIDs.Add(MainCharacterDialogueNode->MC_DialogueRow.DialogueID);

		UEdGraphPin* ExecPin = MainCharacterDialogueNode->FindPin(UEdGraphSchema_K2::PN_Then);
		if (ExecPin && ExecPin->LinkedTo.Num() > 0) 
		{
			MainCharacterDialogueNode->MC_DialogueRow.EndOfDialogue = false;
			
			for (UEdGraphPin* LinkedPin : ExecPin->LinkedTo)
			{
				if (LinkedPin && LinkedPin->GetOwningNode())
				{
					if (UNPC_DialogueNode* Next_NPCNode = Cast<UNPC_DialogueNode>(LinkedPin->GetOwningNode()))
					{
						MainCharacterDialogueNode->MC_DialogueRow.NextDialogueID = Next_NPCNode->NPC_Row.DialogueID;
						Next_NPCNode->NPC_Row.SpeakerName = MainCharacterDialogueNode->MC_DialogueRow.SpeakerName;
						Next_NPCNode->NPC_Row.TargetLevel = MainCharacterDialogueNode->MC_DialogueRow.TargetLevel;
						
						AddToDataTable(MainCharacterDialogueNode);
						HandleAutomatedData(Next_NPCNode);
					}
					else if (UMainCharacterDialogue_Node* Next_PlayerDialogueNode = Cast<UMainCharacterDialogue_Node>(LinkedPin->GetOwningNode()))
					{
						MainCharacterDialogueNode->MC_DialogueRow.NextDialogueID = Next_PlayerDialogueNode->MC_DialogueRow.DialogueID;
						Next_PlayerDialogueNode->MC_DialogueRow.SpeakerName = MainCharacterDialogueNode->MC_DialogueRow.SpeakerName;
						Next_PlayerDialogueNode->MC_DialogueRow.TargetLevel = MainCharacterDialogueNode->MC_DialogueRow.TargetLevel;
						
						AddToDataTable(MainCharacterDialogueNode);
						HandleAutomatedData(Next_PlayerDialogueNode);
					}
				}
			}
		}
		else
		{
			MainCharacterDialogueNode->MC_DialogueRow.EndOfDialogue = true;
			AddToDataTable(MainCharacterDialogueNode);
		}
	}
}

void UDialogueWriter::AddToDataTable(UEdGraphNode* NodeToAddDataTable)
{
	if (UNPC_DialogueNode* NPCNode = Cast<UNPC_DialogueNode>(NodeToAddDataTable))
	{
		FName TargetSpeakerID = NPCNode->NPC_Row.SpeakerName;

		if (UDataTable** FoundTablePtr = NPC_DataTableMap.Find(TargetSpeakerID))
		{
			UDataTable* TargetTable = *FoundTablePtr;
			
			if (TargetTable)
			{
				FNPC_Dialogues FinalData = NPCNode->NPC_Row;

				for (const FName& ManualID : NPCNode->ManualAdded_RelatedNPC_Dialogues)
				{
					FinalData.RelatedNPC_Dialogues.AddUnique(ManualID);
				}

				FName RowName = FinalData.DialogueID;

				if (TargetTable->GetRowMap().Contains(RowName))
				{
					FNPC_Dialogues* ExistingRow = TargetTable->FindRow<FNPC_Dialogues>(RowName, "");
					*ExistingRow = FinalData;
				}
				else
				{
					TargetTable->AddRow(RowName, FinalData);
				}
			}
		}
	}
	
	else if (UMainCharacterChoices_Node*  MainCharacterChoicesNode = Cast<UMainCharacterChoices_Node>(NodeToAddDataTable))
	{
		FName RowName = MainCharacterChoicesNode->AllChoice_Row.Choice1.ChoiceID1;

		if (DT_MainCharacterChoices->GetRowMap().Contains(RowName))
		{
			FMainCharacterChoices* ExistingRow = DT_MainCharacterChoices->FindRow<FMainCharacterChoices>(RowName, "");
			if (ExistingRow)
			{
				*ExistingRow = MainCharacterChoicesNode->AllChoice_Row;
			}
		}
		else
		{
			DT_MainCharacterChoices->AddRow(MainCharacterChoicesNode->AllChoice_Row.Choice1.ChoiceID1, MainCharacterChoicesNode->AllChoice_Row);
		}
	}

	else if (UMainCharacterDialogue_Node* MainCharacterDialoguesNode = Cast<UMainCharacterDialogue_Node>(NodeToAddDataTable))
	{
		FName RowName = MainCharacterDialoguesNode->MC_DialogueRow.DialogueID;

		if (DT_MainCharacterDialogues->GetRowMap().Contains(RowName))
		{
			FNPC_Dialogues* ExistingRow = DT_MainCharacterDialogues->FindRow<FNPC_Dialogues>(RowName, "");
			if (ExistingRow)
			{
				*ExistingRow = MainCharacterDialoguesNode->MC_DialogueRow;
			}
		}
		else
		{
			DT_MainCharacterDialogues->AddRow(MainCharacterDialoguesNode->MC_DialogueRow.DialogueID, MainCharacterDialoguesNode->MC_DialogueRow);
		}
	}
}

void UDialogueWriter::ClearDataTables()
{
	for (const auto& Pair : NPC_DataTableMap)
	{
		UDataTable* Table = Pair.Value;
		if (Table)
		{
			Table->EmptyTable();
		}
	}

	if (DT_MainCharacterChoices)
	{
		DT_MainCharacterChoices->EmptyTable();
	}
	if (DT_MainCharacterDialogues)
	{
		DT_MainCharacterDialogues->EmptyTable();
	}
}

void UDialogueWriter::MarkDataTablesAsDirty()
{
	for (const auto& Pair : NPC_DataTableMap)
	{
		UDataTable* Table = Pair.Value;
		if (Table)
		{
			Table->MarkPackageDirty();
			FDataTableEditorUtils::BroadcastPostChange(Table, FDataTableEditorUtils::EDataTableChangeInfo::RowList);
		}
	}

	if (DT_MainCharacterChoices)
	{
		DT_MainCharacterChoices->MarkPackageDirty();
		FDataTableEditorUtils::BroadcastPostChange(DT_MainCharacterChoices, FDataTableEditorUtils::EDataTableChangeInfo::RowList);
	}
	
	if (DT_MainCharacterDialogues)
	{
		DT_MainCharacterDialogues->MarkPackageDirty();
		FDataTableEditorUtils::BroadcastPostChange(DT_MainCharacterDialogues, FDataTableEditorUtils::EDataTableChangeInfo::RowList);
	}
}





void UDialogueWriter::CleanGhostNodesFromTables()
{
	TArray<UDataTable*> AllTablesToClean;

	
	for (const auto& Pair : NPC_DataTableMap)
	{
		if (Pair.Value) 
		{
			AllTablesToClean.Add(Pair.Value);
		}
	}

	
	if (DT_MainCharacterChoices)
	{
		AllTablesToClean.Add(DT_MainCharacterChoices);
	}
	if (DT_MainCharacterDialogues)
	{
		AllTablesToClean.Add(DT_MainCharacterDialogues);
	}

	
	for (UDataTable* Table : AllTablesToClean)
	{
		if (Table)
		{
			
			TArray<FName> RowNames = Table->GetRowNames();
			
			for (FName RowName : RowNames)
			{
				
				if (!ActiveIDs.Contains(RowName))
				{
					Table->RemoveRow(RowName);
				}
			}
		}
	}
}

void UDialogueWriter::TakeNPCsDataTablesAndName()
{
#if WITH_EDITOR
	
	const UDialogueSystemSettings* Settings = GetDefault<UDialogueSystemSettings>();
	
	if (Settings)
	{
		DT_MainCharacterChoices = Settings->DT_MainCharacterChoices.LoadSynchronous();
		DT_MainCharacterDialogues = Settings->DT_MainCharacterDialogues.LoadSynchronous();
	}
	
	UWorld* EditorWorld = GEditor ? GEditor->GetEditorWorldContext().World() : nullptr;
	
	if (!EditorWorld)
	{
		UE_LOG(LogTemp, Error, TEXT("There is no open level!"));
		return;
	}

	int AddedCount = 0;

	for (TActorIterator<AActor> ActorItr(EditorWorld); ActorItr; ++ActorItr)
	{
		AActor* Actor = *ActorItr;
		if (Actor)
		{
			if (Actor->Implements<UInterface_MainCharacter>())
			{
				
			}
			else
			{
				UAC_DialogueSystem* DialogueComp = Actor->FindComponentByClass<UAC_DialogueSystem>();
			
				if (DialogueComp)
				{
					FName SpeakerName = DialogueComp->NPC_RealName;
					UDataTable* FoundDataTable = DialogueComp->DataTable_NPC;

					if (!SpeakerName.IsNone() && FoundDataTable != nullptr)
					{
						NPC_DataTableMap.Add(SpeakerName, FoundDataTable);
						AddedCount++;
					}
				}
			}
		}
	}

	if (AddedCount > 0)
	{
		this->Modify(); 
	}
	else
	{
		
	}
#endif
}


#if WITH_EDITOR
void UDialogueWriter::OnBlueprintCompiled(UBlueprint* InBlueprint)
{
	if (InBlueprint->GeneratedClass != GetClass())
	{
		return;
	}
	
	TakeNPCsDataTablesAndName();
	GenerateDialogueData();
}
#endif
