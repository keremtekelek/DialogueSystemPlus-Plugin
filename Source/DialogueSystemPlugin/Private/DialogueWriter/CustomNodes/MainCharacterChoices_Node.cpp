#include "DialogueWriter/CustomNodes/MainCharacterChoices_Node.h"
#include "DialogueWriter/DialogueWriter.h"
#include "DialogueWriter/CustomNodes/NPC_DialogueNode.h"

#define LOCTEXT_NAMESPACE "MainCharacterChoices_Node"

UMainCharacterChoices_Node::UMainCharacterChoices_Node()
{
	// Choice ID
	FGuid Choice1_guid = FGuid::NewGuid();
	FGuid Choice2_guid = FGuid::NewGuid();
	FGuid Choice3_guid = FGuid::NewGuid();
	
	this->AllChoice_Row.Choice1.ChoiceID1 = FName(*Choice1_guid.ToString());
	this->AllChoice_Row.Choice2.ChoiceID2 = FName(*Choice2_guid.ToString());
	this->AllChoice_Row.Choice3.ChoiceID3 = FName(*Choice3_guid.ToString());
	
}


FText UMainCharacterChoices_Node::GetNodeTitle(ENodeTitleType::Type TitleType) const
{
	return LOCTEXT("NodeTitle", "Choice Node");
}

void UMainCharacterChoices_Node::AllocateDefaultPins()
{
	CreatePin(EGPD_Input, UEdGraphSchema_K2::PC_Struct, FDialogueFlow::StaticStruct(), UEdGraphSchema_K2::PN_Execute);
	
	UEdGraphPin* C1 = CreatePin(EGPD_Output, UEdGraphSchema_K2::PC_Struct, FDialogueFlow::StaticStruct(), TEXT("Choice1"));
	C1->PinFriendlyName = LOCTEXT("C1_Label", "Choice 1"); 

	// Choice 2
	UEdGraphPin* C2 = CreatePin(EGPD_Output, UEdGraphSchema_K2::PC_Struct, FDialogueFlow::StaticStruct(), TEXT("Choice2"));
	C2->PinFriendlyName = LOCTEXT("C2_Label", "Choice 2");

	// Choice 3
	UEdGraphPin* C3 = CreatePin(EGPD_Output, UEdGraphSchema_K2::PC_Struct, FDialogueFlow::StaticStruct(), TEXT("Choice3"));
	C3->PinFriendlyName = LOCTEXT("C3_Label", "Choice 3");
}

void UMainCharacterChoices_Node::GetMenuActions(FBlueprintActionDatabaseRegistrar& ActionRegistrar) const
{
	UClass* ActionKey = GetClass();
	if (ActionRegistrar.IsOpenForRegistration(ActionKey))
	{
		UBlueprintNodeSpawner* NodeSpawner = UBlueprintNodeSpawner::Create(GetClass());
		check(NodeSpawner != nullptr);
		ActionRegistrar.AddBlueprintAction(ActionKey, NodeSpawner);
	}
}

FText UMainCharacterChoices_Node::GetMenuCategory() const
{
	return LOCTEXT("MenuCategory", "Dialogue System");
}

bool UMainCharacterChoices_Node::IsCompatibleWithGraph(const UEdGraph* TargetGraph) const
{
	if (!TargetGraph)
	{
		return false;
	}
	
	
	UBlueprint* MyBlueprint = Cast<UBlueprint>(TargetGraph->GetOuter());
	if (!MyBlueprint || !MyBlueprint->ParentClass)
	{
		return false;
	}

	if (MyBlueprint->ParentClass->IsChildOf(UDialogueWriter::StaticClass()))
	{
		return true;
	}
	
	return false;
}

bool UMainCharacterChoices_Node::IsConnectionDisallowed(const UEdGraphPin* MyPin, const UEdGraphPin* OtherPin, FString& OutReason) const
{
	if (MyPin->Direction == EGPD_Output && MyPin->PinType.PinCategory == UEdGraphSchema_K2::PC_Struct)
	{
		UK2Node* OtherNode = Cast<UK2Node>(OtherPin->GetOwningNode());
		bool bIsNPC = OtherNode && OtherNode->IsA(UNPC_DialogueNode::StaticClass());

		if (!bIsNPC)
		{
			OutReason = TEXT("Blocked");
			return true; 
		}
	}

	return Super::IsConnectionDisallowed(MyPin, OtherPin, OutReason);
}

void UMainCharacterChoices_Node::PostEditImport()
{
	Super::PostEditImport();

	UEdGraph* ParentGraph = GetGraph();
	if (ParentGraph)
	{
		bool IsCopy = false;
		
		for (UEdGraphNode* Node : ParentGraph->Nodes)
		{
			if (Node == this) continue;

			if (UMainCharacterChoices_Node* OtherChoiceNode = Cast<UMainCharacterChoices_Node>(Node))
			{
				if (OtherChoiceNode->AllChoice_Row.Choice1.ChoiceID1 == this->AllChoice_Row.Choice1.ChoiceID1)
				{
					IsCopy = true;
					break;
				}
			}
		}

		if (this->AllChoice_Row.Choice1.ChoiceID1.IsNone() || IsCopy)
		{
			this->AllChoice_Row.Choice1.ChoiceID1 = FName(*FGuid::NewGuid().ToString());
			this->AllChoice_Row.Choice2.ChoiceID2 = FName(*FGuid::NewGuid().ToString());
			this->AllChoice_Row.Choice3.ChoiceID3 = FName(*FGuid::NewGuid().ToString());
			
			this->AllChoice_Row.Choice1.EndOfDialogue = false;
			this->AllChoice_Row.Choice1.SpeakerName = NAME_None;

			this->AllChoice_Row.Choice2.EndOfDialogue = false;
			this->AllChoice_Row.Choice2.SpeakerName = NAME_None;

			this->AllChoice_Row.Choice3.EndOfDialogue = false;
			this->AllChoice_Row.Choice3.SpeakerName = NAME_None;
			
			this->AllChoice_Row.Choice1.NextDialogueID = NAME_None;
			this->AllChoice_Row.Choice2.NextDialogueID = NAME_None;
			this->AllChoice_Row.Choice3.NextDialogueID = NAME_None;
			
			this->AllChoice_Row.Choice1.TargetLevel = nullptr;
			this->AllChoice_Row.Choice2.TargetLevel = nullptr;
			this->AllChoice_Row.Choice3.TargetLevel = nullptr;
		}
	}
	
}

#undef LOCTEXT_NAMESPACE