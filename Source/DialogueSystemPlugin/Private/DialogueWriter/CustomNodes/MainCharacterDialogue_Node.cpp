#include "DialogueWriter/CustomNodes/MainCharacterDialogue_Node.h"
#include "DialogueWriter/DialogueWriter.h"
#include "DialogueWriter/CustomNodes/MainCharacterChoices_Node.h"
#include "DialogueWriter/CustomNodes/NPC_DialogueNode.h"

#define LOCTEXT_NAMESPACE "MainCharacterDialogue_Node"

UMainCharacterDialogue_Node::UMainCharacterDialogue_Node()
{
	FGuid dialogueID_guid = FGuid::NewGuid();
	this->MC_DialogueRow.DialogueID = FName(*dialogueID_guid.ToString());
}


FText UMainCharacterDialogue_Node::GetNodeTitle(ENodeTitleType::Type TitleType) const
{
	return LOCTEXT("NodeTitle", "Player Dialogue Node");
}

void UMainCharacterDialogue_Node::AllocateDefaultPins()
{
	const UEdGraphSchema_K2* K2Schema = GetDefault<UEdGraphSchema_K2>();

	

	CreatePin(EGPD_Input, UEdGraphSchema_K2::PC_Struct, FDialogueFlow::StaticStruct(), UEdGraphSchema_K2::PN_Execute);

	
	
	CreatePin(EGPD_Output, UEdGraphSchema_K2::PC_Struct, FDialogueFlow::StaticStruct(), UEdGraphSchema_K2::PN_Then);
	
}

void UMainCharacterDialogue_Node::GetMenuActions(FBlueprintActionDatabaseRegistrar& ActionRegistrar) const
{
	
	UClass* ActionKey = GetClass();
	if (ActionRegistrar.IsOpenForRegistration(ActionKey))
	{
		UBlueprintNodeSpawner* NodeSpawner = UBlueprintNodeSpawner::Create(GetClass());
		check(NodeSpawner != nullptr);
		ActionRegistrar.AddBlueprintAction(ActionKey, NodeSpawner);
	}
}

FText UMainCharacterDialogue_Node::GetMenuCategory() const
{
	return LOCTEXT("MenuCategory", "Dialogue System");
}

bool UMainCharacterDialogue_Node::IsCompatibleWithGraph(const UEdGraph* TargetGraph) const
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

bool UMainCharacterDialogue_Node::IsConnectionDisallowed(const UEdGraphPin* MyPin, const UEdGraphPin* OtherPin, FString& OutReason) const
{
	if (MyPin->Direction == EGPD_Input && MyPin->PinType.PinCategory == UEdGraphSchema_K2::PC_Struct)
	{
		UK2Node* OtherNode = Cast<UK2Node>(OtherPin->GetOwningNode());

		bool IsNPC = OtherNode && OtherNode->IsA(UNPC_DialogueNode::StaticClass());
		bool IsMC_DialogueNode = OtherNode && OtherNode->IsA(UMainCharacterDialogue_Node::StaticClass());

		if (!IsNPC && !IsMC_DialogueNode)
		{
			OutReason = TEXT("Blocked");
			return true; 
		}
	}

	
	else if (MyPin->Direction == EGPD_Output && MyPin->PinType.PinCategory == UEdGraphSchema_K2::PC_Struct)
	{
		UK2Node* OtherNode = Cast<UK2Node>(OtherPin->GetOwningNode());

		bool IsNPC = OtherNode && OtherNode->IsA(UNPC_DialogueNode::StaticClass());
		bool IsMC_DialogueNode = OtherNode && OtherNode->IsA(UMainCharacterDialogue_Node::StaticClass());

		if (!IsNPC && !IsMC_DialogueNode)
		{
			OutReason = TEXT("Blocked");
			return true; 
		}
	}

	return Super::IsConnectionDisallowed(MyPin, OtherPin, OutReason);

}

void UMainCharacterDialogue_Node::PostEditImport()
{
	Super::PostEditImport();
	
	UEdGraph* ParentGraph = GetGraph();
	if (ParentGraph)
	{
		bool IsCopy = false;

		for (UEdGraphNode* Node : ParentGraph->Nodes)
		{
			if (Node == this) continue;

			if (UMainCharacterDialogue_Node* OtherMCNode = Cast<UMainCharacterDialogue_Node>(Node))
			{
				if (OtherMCNode->MC_DialogueRow.DialogueID == this->MC_DialogueRow.DialogueID)
				{
					IsCopy = true;
					break;
				}
			}
		}
		
		if (this->MC_DialogueRow.DialogueID.IsNone() || IsCopy)
		{
			this->MC_DialogueRow.DialogueID = FName(*FGuid::NewGuid().ToString());
			
			this->MC_DialogueRow.RelatedNPC_Dialogues.Empty();
			this->MC_DialogueRow.RelatedGlobalEvents.Reset();
			this->MC_DialogueRow.IsRoot = false;
			this->MC_DialogueRow.EndOfDialogue = false;
			this->MC_DialogueRow.NextDialogueID = NAME_None;
			this->MC_DialogueRow.NextChoiceID = NAME_None;
			this->MC_DialogueRow.TargetLevel = nullptr;
		}
	}
	
}


void UMainCharacterDialogue_Node::GetNodeContextMenuActions(UToolMenu* Menu, UGraphNodeContextMenuContext* Context) const
{
	Super::GetNodeContextMenuActions(Menu, Context);
	
	UMainCharacterDialogue_Node* MutableThis = const_cast<UMainCharacterDialogue_Node*>(this);
	
	FToolMenuSection& Section = Menu->AddSection("DialogueActions", LOCTEXT("DialogueActions", "Dialogue System Actions"));
	
	FName CurrentNodeID = MutableThis->MC_DialogueRow.DialogueID;

	Section.AddSubMenu(
		"ShowDialogueID_SubMenu",
		LOCTEXT("ShowID_MainLabel", "Show Dialogue ID"), 
		LOCTEXT("ShowID_MainTooltip", "Shows the Player Dialogue Node's DialogueID"), 
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
					LOCTEXT("NoID_ErrorTooltip", "This node has no Dialogue ID. This will definitely cause errors!"), 
					FSlateIcon(), 
					FUIAction() 
				);
			}
		}),
		false, 
		FSlateIcon(FAppStyle::GetAppStyleSetName(), "Icons.Visible") 
	);
	
	
	FName CurrentNextDialogueID = MutableThis->MC_DialogueRow.NextDialogueID;

	Section.AddSubMenu(
		"ShowNextDialogueID_SubMenu",
		LOCTEXT("ShowNextID_MainLabel", "Show Next Dialogue ID"), 
		LOCTEXT("ShowNextID_MainTooltip", "Shows Player Dialogue Node's Next Dialogue ID"), 
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