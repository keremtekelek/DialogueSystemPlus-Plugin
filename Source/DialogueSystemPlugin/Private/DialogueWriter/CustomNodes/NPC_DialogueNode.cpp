#include "DialogueWriter/CustomNodes/NPC_DialogueNode.h"
#include "DialogueWriter/DialogueWriter.h"
#include "DialogueWriter/CustomNodes/MainCharacterChoices_Node.h"
#include "DialogueWriter/CustomNodes/MainCharacterDialogue_Node.h"

FName FDialogueEditorClipboard::CopiedID = NAME_None;
FString FDialogueEditorClipboard::CopiedNodeType = TEXT("");

#define LOCTEXT_NAMESPACE "NPC_DialogueNode"

UNPC_DialogueNode::UNPC_DialogueNode()
{
	FGuid dialogueID_guid = FGuid::NewGuid();
	this->NPC_Row.DialogueID = FName(*dialogueID_guid.ToString());
}


FText UNPC_DialogueNode::GetNodeTitle(ENodeTitleType::Type TitleType) const
{
	
	return LOCTEXT("NodeTitle", "NPC Dialogue Node");
}

void UNPC_DialogueNode::AllocateDefaultPins()
{
	const UEdGraphSchema_K2* K2Schema = GetDefault<UEdGraphSchema_K2>();

	

	CreatePin(EGPD_Input, UEdGraphSchema_K2::PC_Struct, FDialogueFlow::StaticStruct(), UEdGraphSchema_K2::PN_Execute);

	
	//CreatePin(EGPD_Output, UEdGraphSchema_K2::PC_Exec, UEdGraphSchema_K2::PN_Then);
	CreatePin(EGPD_Output, UEdGraphSchema_K2::PC_Struct, FDialogueFlow::StaticStruct(), UEdGraphSchema_K2::PN_Then);
	
}

void UNPC_DialogueNode::GetMenuActions(FBlueprintActionDatabaseRegistrar& ActionRegistrar) const
{
	
	UClass* ActionKey = GetClass();
	if (ActionRegistrar.IsOpenForRegistration(ActionKey))
	{
		UBlueprintNodeSpawner* NodeSpawner = UBlueprintNodeSpawner::Create(GetClass());
		check(NodeSpawner != nullptr);
		ActionRegistrar.AddBlueprintAction(ActionKey, NodeSpawner);
	}
}

FText UNPC_DialogueNode::GetMenuCategory() const
{
	return LOCTEXT("MenuCategory", "Dialogue System");
}

bool UNPC_DialogueNode::IsCompatibleWithGraph(const UEdGraph* TargetGraph) const
{
	if (!TargetGraph)
	{
		return false;
	}

	
	UBlueprint* MyBlueprint = Cast<UBlueprint>(TargetGraph->GetOuter());
	if (!MyBlueprint || !MyBlueprint->GeneratedClass)
	{
		return false;
	}
		
	if (MyBlueprint->ParentClass->IsChildOf(UDialogueWriter::StaticClass()))
	{
		return true; 
	}

	return false;
}

bool UNPC_DialogueNode::IsConnectionDisallowed(const UEdGraphPin* MyPin, const UEdGraphPin* OtherPin, FString& OutReason) const
{
	

	if (MyPin->Direction == EGPD_Input && MyPin->PinType.PinCategory == UEdGraphSchema_K2::PC_Struct)
	{
		UK2Node* OtherNode = Cast<UK2Node>(OtherPin->GetOwningNode());

		
		bool IsChoice = OtherNode && OtherNode->IsA(UMainCharacterChoices_Node::StaticClass());
		bool IsNPC = OtherNode && OtherNode->IsA(UNPC_DialogueNode::StaticClass());
		bool IsMCDialogueNode = OtherNode && OtherNode->IsA(UMainCharacterDialogue_Node::StaticClass());
		

		if (!IsChoice && !IsNPC && !IsMCDialogueNode)
		{
			OutReason = TEXT("Blocked");
			return true; 
		}
	}

	
	else if (MyPin->Direction == EGPD_Output && MyPin->PinType.PinCategory == UEdGraphSchema_K2::PC_Struct)
	{
		UK2Node* OtherNode = Cast<UK2Node>(OtherPin->GetOwningNode());

		
		bool IsChoice = OtherNode && OtherNode->IsA(UMainCharacterChoices_Node::StaticClass());
		bool IsNPC = OtherNode && OtherNode->IsA(UNPC_DialogueNode::StaticClass());
		bool IsMCDialogueNode = OtherNode && OtherNode->IsA(UMainCharacterDialogue_Node::StaticClass());

		if (!IsChoice && !IsNPC && !IsMCDialogueNode)
		{
			OutReason = TEXT("Blocked");
			return true; 
		}
	}

	return Super::IsConnectionDisallowed(MyPin, OtherPin, OutReason);

}

void UNPC_DialogueNode::PostEditImport()
{
	Super::PostEditImport();
	
	UEdGraph* ParentGraph = GetGraph();
	if (ParentGraph)
	{
		bool IsCopy = false;

		for (UEdGraphNode* Node : ParentGraph->Nodes)
		{
			if (Node == this) continue;
			
			if (UNPC_DialogueNode* OtherNPCNode = Cast<UNPC_DialogueNode>(Node))
			{
				if (OtherNPCNode->NPC_Row.DialogueID == this->NPC_Row.DialogueID)
				{
					IsCopy = true;
					break;
				}
			}
		}

		
		if (this->NPC_Row.DialogueID.IsNone() || IsCopy)
		{
			this->NPC_Row.DialogueID = FName(*FGuid::NewGuid().ToString());
			
			this->NPC_Row.RelatedNPC_Dialogues.Empty();
			this->NPC_Row.IsRoot = false; 
			this->NPC_Row.EndOfDialogue = false;
			this->NPC_Row.RelatedGlobalEvents.Reset();
			this->NPC_Row.NextChoiceID = NAME_None;
			this->NPC_Row.NextDialogueID = NAME_None;
			this->NPC_Row.TargetLevel = nullptr;
			this->NPC_Row.HasToRequireEvents.Reset();
		}
	}
}

void UNPC_DialogueNode::GetNodeContextMenuActions(UToolMenu* Menu, UGraphNodeContextMenuContext* Context) const
{
    Super::GetNodeContextMenuActions(Menu, Context);

    if (!Context->Graph)
    {
        return;
    }
	
	UNPC_DialogueNode* MutableThis = const_cast<UNPC_DialogueNode*>(this);

    FToolMenuSection& Section = Menu->AddSection("DialogueActions", LOCTEXT("DialogueActions", "Dialogue System Actions"));
    
	Section.AddMenuEntry(
		"CopyNodeID",
		LOCTEXT("CopyNodeID", "Copy Node ID"),
		LOCTEXT("CopyNodeIDTooltip", "Copies ID to clipboard."),
		FSlateIcon(),
		FUIAction(FExecuteAction::CreateLambda([MutableThis]() 
		{
			FDialogueEditorClipboard::CopiedID = MutableThis->NPC_Row.DialogueID;
			FDialogueEditorClipboard::CopiedNodeType = TEXT("NPC");
            
			UE_LOG(LogTemp, Log, TEXT("Copied ID: %s"), *FDialogueEditorClipboard::CopiedID.ToString());
		}))
	);

    
	Section.AddMenuEntry(
		"PasteNodeID",
		LOCTEXT("PasteNodeID", "Paste Node ID"),
		LOCTEXT("PasteNodeIDTooltip", "Adds ID to Related NPC Dialogue's list."),
		FSlateIcon(),
		FUIAction(
			FExecuteAction::CreateLambda([MutableThis]()
			{
				FScopedTransaction Transaction(LOCTEXT("PasteNodeIDTx", "Paste Node ID"));
                
				MutableThis->Modify(); 
                
				if (FDialogueEditorClipboard::CopiedNodeType == TEXT("NPC"))
				{
					if (FDialogueEditorClipboard::CopiedID == MutableThis->NPC_Row.DialogueID)
					{
						FText ErrorMsg = FText::FromString(TEXT("ERROR: You can't paste a node's ID to itself"));
						FMessageDialog::Open(EAppMsgType::Ok, ErrorMsg);
					}
					else
					{
						MutableThis->ManualAdded_RelatedNPC_Dialogues.AddUnique(FDialogueEditorClipboard::CopiedID);
						MutableThis->NPC_Row.RelatedNPC_Dialogues.AddUnique(FDialogueEditorClipboard::CopiedID);
					}
				}
			}),
			FCanExecuteAction::CreateLambda([]()
			{
				return !FDialogueEditorClipboard::CopiedID.IsNone();
			})
		)
	);

    
	Section.AddMenuEntry(
		"ClearManualIDs",
		LOCTEXT("ClearManualIDs", "Clear Pasted IDs"),
		LOCTEXT("ClearManualIDsTooltip", "Removes only manually pasted IDs."),
		FSlateIcon(),
		FUIAction(
			FExecuteAction::CreateLambda([MutableThis]()
			{
				FScopedTransaction Transaction(LOCTEXT("ClearManualIDsTx", "Clear Manual IDs"));
				MutableThis->Modify();

				for (const FName& ID_to_Delete : MutableThis->ManualAdded_RelatedNPC_Dialogues)
				{
					if (MutableThis->NPC_Row.RelatedNPC_Dialogues.Contains(ID_to_Delete))
					{
						MutableThis->NPC_Row.RelatedNPC_Dialogues.Remove(ID_to_Delete);
					}
				}
				
				MutableThis->ManualAdded_RelatedNPC_Dialogues.Empty();
				UE_LOG(LogTemp, Warning, TEXT("Manual Added IDs Cleared."));
			}),
			FCanExecuteAction::CreateLambda([MutableThis]()
			{
				return MutableThis->ManualAdded_RelatedNPC_Dialogues.Num() > 0;
			})
		)
	);
	
	
	Section.AddSubMenu(
        "PastedDialogues",
        LOCTEXT("PastedDialogues", "Pasted Dialogues Preview"), 
        LOCTEXT("PastedDialoguesTooltip", "Shows manually added(pasted) dialogues ID's"), 
        FNewToolMenuDelegate::CreateLambda([MutableThis](UToolMenu* SubMenu)
        {
            
            if (MutableThis->ManualAdded_RelatedNPC_Dialogues.Num() == 0)
            {
                SubMenu->AddMenuEntry(
                    "EmptyList",
                    FToolMenuEntry::InitMenuEntry(
                        "EmptyList",
                        LOCTEXT("EmptyList", "No manual dialogue ID found"),
                        LOCTEXT("EmptyListTooltip", "Paste a Node ID first."),
                        FSlateIcon(),
                        FUIAction() 
                    )
                );
                return;
            }

            UEdGraph* CurrentGraph = MutableThis->GetGraph();
            if (!CurrentGraph)
            {
            	return;
            }

            for (const FName& LinkedID : MutableThis->ManualAdded_RelatedNPC_Dialogues)
            {
                FString DisplayText = "Node Not Found in Graph!";
                FString FullDialogue = "";

                for (UEdGraphNode* Node : CurrentGraph->Nodes)
                {
                    if (UNPC_DialogueNode* TargetNode = Cast<UNPC_DialogueNode>(Node))
                    {
                        if (TargetNode->NPC_Row.DialogueID == LinkedID)
                        {
                            FullDialogue = TargetNode->NPC_Row.DialogueText.ToString();
                            
                            if (FullDialogue.Len() > 30)
                            {
                                DisplayText = FullDialogue.Left(30) + "...";
                            }
                            else
                            {
                                DisplayText = FullDialogue;
                            }
                            break; 
                        }
                    }
                }

            	FString FullID = LinkedID.ToString();
				FString FormattedID = FullID;

				if (FullID.Len() > 10)
				{
					FormattedID = FString::Printf(TEXT("%s...%s"), *FullID.Left(4), *FullID.Right(4));
				}
                
                FText MenuLabel = FText::Format(LOCTEXT("Format", "[{0}]: {1}"), FText::FromString(FormattedID), FText::FromString(DisplayText));
                FText TooltipText = FText::FromString(FullDialogue); 

                SubMenu->AddMenuEntry(
                    FName(*LinkedID.ToString()),
                    FToolMenuEntry::InitMenuEntry(
                        FName(*LinkedID.ToString()),
                        MenuLabel,
                        TooltipText,
                        FSlateIcon(),
                        FUIAction(
                        FExecuteAction::CreateLambda([CurrentGraph, LinkedID]
                        {
                        	for (UEdGraphNode* Node : CurrentGraph->Nodes)
                        	{
                        		if (UNPC_DialogueNode* TargetNode = Cast<UNPC_DialogueNode>(Node))
                        		{
                        			if (TargetNode->NPC_Row.DialogueID == LinkedID)
                        			{
                        				FKismetEditorUtilities::BringKismetToFocusAttentionOnObject(TargetNode);
										break;
									}
                        		}
                        	}
                        })
                        ) 
                    )
                );
            }
        })
    );
	
	
    FName CurrentNodeID = MutableThis->NPC_Row.DialogueID;

    Section.AddSubMenu(
        "ShowDialogueID_SubMenu",
        LOCTEXT("ShowID_MainLabel", "Show Dialogue ID"), 
        LOCTEXT("ShowID_MainTooltip", "Shows the NPC Dialogue Node's DialogueID"), 
        FNewMenuDelegate::CreateLambda([CurrentNodeID](FMenuBuilder& MenuBuilder)
        {
            if (!CurrentNodeID.IsNone() && !CurrentNodeID.ToString().IsEmpty())
            {
                FString FullIDString = CurrentNodeID.ToString();
                FString FormattedID = FullIDString;

                if (FullIDString.Len() > 8)
                {
                    FormattedID = FString::Printf(TEXT("%s...%s"), *FullIDString.Left(4), *FullIDString.Right(4));
                }

                MenuBuilder.AddMenuEntry(
                    FText::FromString(FormattedID), 
                    FText::Format(LOCTEXT("ID_Tooltip", "Full ID: {0}"), FText::FromString(FullIDString)), 
                    FSlateIcon(), 
                    FUIAction()
                );
            }
            else
            {
                MenuBuilder.AddMenuEntry(
                    LOCTEXT("NoID_ErrorLabel", "NO ID FOUND!"), 
                    LOCTEXT("NoID_ErrorTooltip", "This node has no Dialogue ID. This will definetly cause errors!"), 
                    FSlateIcon(), 
                    FUIAction() 
                );
            }
        }),
        false, 
        FSlateIcon(FAppStyle::GetAppStyleSetName(), "Icons.Visible") 
    );
	
	FName CurrentNextDialogueID = MutableThis->NPC_Row.NextDialogueID;

	Section.AddSubMenu(
		"ShowNextDialogueID_SubMenu",
		LOCTEXT("ShowNextID_MainLabel", "Show Next Dialogue ID"), 
		LOCTEXT("ShowNextID_MainTooltip", "Shows NPC Dialogue Node's Next Dialogue ID"), 
		FNewMenuDelegate::CreateLambda([CurrentNextDialogueID](FMenuBuilder& MenuBuilder)
		{
			if (!CurrentNextDialogueID.IsNone() && !CurrentNextDialogueID.ToString().IsEmpty())
			{
				FString FullIDString = CurrentNextDialogueID.ToString();
				FString FormattedID = FullIDString;

				if (FullIDString.Len() > 8)
				{
					FormattedID = FString::Printf(TEXT("%s...%s"), *FullIDString.Left(4), *FullIDString.Right(4));
				}

				MenuBuilder.AddMenuEntry(
					FText::FromString(FormattedID), 
					FText::Format(LOCTEXT("ID_Tooltip", "Full ID: {0}"), FText::FromString(FullIDString)), 
					FSlateIcon(), 
					FUIAction()
				);
			}
			else
			{
				MenuBuilder.AddMenuEntry(
					LOCTEXT("NoNextID_Label", "Empty"), 
					LOCTEXT("NoNextID_Tooltip", "There is no Next Dialogue ID to show!"), 
					FSlateIcon(), 
					FUIAction() 
				);
			}
		}),
		false, 
		FSlateIcon(FAppStyle::GetAppStyleSetName(), "Icons.Visible") 
	);
	
}

#undef LOCTEXT_NAMESPACE
