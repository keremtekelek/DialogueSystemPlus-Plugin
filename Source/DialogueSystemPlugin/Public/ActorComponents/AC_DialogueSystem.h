

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Blueprint/UserWidget.h"
#include "Components/WidgetComponent.h"
#include "EnumsAndStructs/GlobalEnumsAndStructs.h"
#include "AC_DialogueSystem.generated.h"

class UWidget_MoodMeter;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class DIALOGUESYSTEMPLUGIN_API UAC_DialogueSystem : public UActorComponent
{
	GENERATED_BODY()

public:
	
	UAC_DialogueSystem();

protected:
	
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,FActorComponentTickFunction* ThisTickFunction) override;
	virtual void OnRegister() override;

	//VARIABLES!!!
public:
	
	
	// this property reads by dialogue writer for the npc data table map
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Changeable NPC Data")
	FName NPC_RealName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Changeable NPC Data")
	FName NPC_OfficialName;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Changeable NPC Data")
	int NPC_MoodValue{50};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Changeable NPC Data")
	UDataTable* DataTable_NPC;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Changeable NPC Data")
	bool ShouldShowNPC_Mood{true};
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Changeable NPC Data")
	bool ExposeRealName{false};

	UPROPERTY()
	AActor* OwnerActor;

	UPROPERTY()
	ENPCMood NPC_Mood;
	
	UPROPERTY()
	UWidget_MoodMeter* WBP_MoodMeter;

	UPROPERTY()
	TMap<FName, int32> DSM_NPC;
	
	
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
	TSubclassOf<UUserWidget> MoodWidgetClass;

	UPROPERTY(BlueprintReadOnly, Category = "UI")
	UWidgetComponent* MoodWidgetComponent;
	
	UPROPERTY(VisibleAnywhere)
	bool IsOwnerMainCharacter;
	
	
	
	//FUNCTIONS!!!
public:
	
	UFUNCTION()
	void GettingStaticVariables();

	UFUNCTION()
	void GettingDynamicVariables();
	
	UFUNCTION(BlueprintCallable)
	void AddMoodValue(int MoodValueToAdd);
	
	UFUNCTION()
	ENPCMood GetMoodValue();
	
	
	
};
