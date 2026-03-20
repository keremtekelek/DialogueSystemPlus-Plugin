#include "DialogueWriter/Factory/DialogueWriterFactory.h"
#include "DialogueWriter/DialogueWriter.h"

#if WITH_EDITOR
	#include "Kismet2/KismetEditorUtilities.h"
#endif

UDialogueWriterFactory::UDialogueWriterFactory()
{
	
	SupportedClass = UDialogueWriter::StaticClass();
	bCreateNew = true;
	bEditAfterNew = true;
}

#if WITH_EDITOR
UObject* UDialogueWriterFactory::FactoryCreateNew(UClass* Class, UObject* InParent, FName Name, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn)
{
	return FKismetEditorUtilities::CreateBlueprint(UDialogueWriter::StaticClass(), InParent, Name, BPTYPE_Normal, UBlueprint::StaticClass(), UBlueprintGeneratedClass::StaticClass(), NAME_None);
}

bool UDialogueWriterFactory::CanCreateNew() const
{
	return true;
}
#endif