#pragma once

#include "CoreMinimal.h"
#include "DialogueWriterFactory.generated.h"

class UDialogueWriter;
UCLASS()
class DIALOGUESYSTEMPLUGIN_API UDialogueWriterFactory : public UFactory
{
	GENERATED_BODY()

public:
	UDialogueWriterFactory();

	
	#if WITH_EDITOR
		virtual UObject* FactoryCreateNew(UClass* Class, UObject* InParent, FName Name, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn) override;
		virtual bool CanCreateNew() const override;
	#endif
};