#pragma once
#include "CoreMinimal.h"

#if WITH_EDITOR
#include "AssetTypeActions_Base.h"
#include "DialogueWriter/DialogueWriter.h"

class FAssetAction_DialogueWriter : public FAssetTypeActions_Base
{
private:
    EAssetTypeCategories::Type MyAssetCategory;

public:
    
    FAssetAction_DialogueWriter(EAssetTypeCategories::Type InAssetCategory)
    {
        MyAssetCategory = InAssetCategory;
    }

    
    virtual FText GetName() const override { return FText::FromString("Dialogue Writer"); }
    
    
    virtual FColor GetTypeColor() const override { return FColor(0, 200, 255); }
    
    
    virtual UClass* GetSupportedClass() const override { return UDialogueWriter::StaticClass(); }
    
    
    virtual uint32 GetCategories() override { return MyAssetCategory; }
};
#endif