#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "AC_InteractionSystem.generated.h"

class USubsystem_Dialogue;
class UAC_DialogueSystem;
class UWidget_MoodMeter;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnNPCInteractSignature);

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class DIALOGUESYSTEMPLUGIN_API UAC_InteractionSystem : public UActorComponent
{
	GENERATED_BODY()

public:
	
	UAC_InteractionSystem();

protected:
	
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,FActorComponentTickFunction* ThisTickFunction) override;

	//VARIABLES
public:
	
	UPROPERTY()
	AActor* OwnerActor;
	
	UPROPERTY()
	bool IsOwnerMainCharacter;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite,Category = "Main Character Interaction")
	float InteractionDistance = 250.0f;
	
	UPROPERTY()
	USubsystem_Dialogue* DialogueSubsystem;

	UPROPERTY(VisibleAnywhere)
	UAC_DialogueSystem* TargetNPC_DialogueSystem;
	
	UPROPERTY(VisibleAnywhere)
	UAC_InteractionSystem* TargetNPC_InteractionSystem;
	
	UPROPERTY(VisibleAnywhere)
	UAC_DialogueSystem* OwnerMC_DialogueSystem;
	
	UPROPERTY(VisibleAnywhere)
	UAC_InteractionSystem* OwnerMC_InteractionSystem;
	
	UPROPERTY(VisibleAnywhere)
	UAC_DialogueSystem* OwnerNPC_DialogueSystem;
	
	UPROPERTY(VisibleAnywhere)
	UAC_InteractionSystem* OwnerNPC_InteractionSystem;
	
	//FUNCTIONS
public:
	
	UFUNCTION()
	void GetStaticVariables();
	
	UPROPERTY(BlueprintAssignable, Category = "NPC Interaction")
	FOnNPCInteractSignature OnNPCInteract;

	UFUNCTION(BlueprintCallable, Category = "NPC Interaction")
	void TriggerNPCInteraction();
	
	//MC stands for "Main Character"
	UFUNCTION(BlueprintCallable, Category = "Main Character Interaction")
	void MC_Interact();
};
