# 💬 Dialogue System Plus Plugin

Built using Unreal Engine 5 and C++, this system allows developers to design complex dialogues using a visual, node-based workflow.

## 📖 Overview

Traditional dialogue systems often rely on CSV, JSON, or even Excel files, which can become problem for writers. This system eliminates those challenges by leveraging Custom K2Nodes within the Unreal Blueprint Graph, allowing for a fully visual authoring experience.

Furthermore, the dialogue is dynamic; branching paths and responses adapt in real-time based on the NPC's mood, your previous choices, triggered events in game and how you treat them. Detailed workflow and system mechanics are outlined below.

## ✨Features

### 🧩 1. Custom Graph Editor

Actually it's not real custom graph. Instead, I used a UObject-based Blueprint as my own custom graph.

#### 1.1 Custom K2Nodes

These custom K2Nodes are the actual pieces that make visual dialogue authoring possible. The system is built around three core node types: NPC Dialogue Node, Player Dialogue Node and Choice Node.

![customnodes](https://github.com/user-attachments/assets/dc005524-f115-4695-bb61-bc5267cbc3d4)


- **NPC Dialogue Node**: It allows us to write the dialogue lines to be spoken by the NPC.
- **Player Dialogue Node**: It allows us to write the dialogue lines to be spoken by the Player.
- **Choice Node**: It allows us to make choices.

#### 1.2 Auto-Compilation



https://github.com/user-attachments/assets/47f5fd62-c5c0-4088-8c52-50578a58bec2



Thanks to our DialogueWriter class, we bake the dialogues created with custom K2Nodes in BP_DialogueWriter to avoid optimization issues during gameplay. 
The logic is simple: when you finish writing and hit Compile button in BP_DialogueWriter, the system automatically exports all dialogue data from the graph into a DataTable. 
This ensures that at runtime, we don't waste resources searching through the Blueprint Graph; instead, we fetch the data directly from the DataTable.

Furthermore, NPC dialogues, player dialogues, and choices are stored in separate DataTables. This segregation prevents performance bottlenecks that might occur from keeping all dialogue data in a single, massive table.

### 💾 2. Dynamic NPC Memory System

Interactions with NPC are not static. These interactions are dynamically updated based on the variables listed below:

- Previous Choices and Dialogues
- Current NPC Mood
- Triggered Events on the Game

This architecture ensures that dialogues are context-aware, adapting in real-time to game events, past interactions, and the NPC’s emotional state.



https://github.com/user-attachments/assets/ffaf9a16-5f39-4e0f-8448-9db68a34daf5



### 🧠 3. Subsystems

This system utilizes three core subsystems to manage its logic: Dialogue Subsystem, Event Manager Subsystem, and Localization Subsystem.

#### 3.1 Dialogue Subsystem

This subsystem is the central hub for managing all dialogue operations, including:

- **Dialogue Scoring**: Logic for evaluating and selecting the most scored dialogue (detailed in the system design section).
- **Dialogue Display**: Handling the displaying dialogue.
- **Input Management During Dialogue**: 
- **"Do Not Disturb" Logic**: Allowing NPCs to give temporary, contextual responses when they are busy or unavailable.
- **Skip Functionality**: Managing the "Skip Dialogue" feature for players.
  
 ![Ekran görüntüsü_29-1-2026_20713_](https://github.com/user-attachments/assets/d8e07496-db68-4303-81e3-243574803350)

  
#### 3.2 Event Manager Subsystem

This subsystem allows us to trigger game events at any point during gameplay and maintains persistent memory of triggered game events.

![event](https://github.com/user-attachments/assets/1adf119c-0304-4257-a1f7-1ac3f181da51) |



### 🗣 4. Interaction System

Naturally, to speak with an NPC, we first need to interact with them. This system provides that functionality through a interfaces. Additionally, the system includes interaction icons to guide the player.



https://github.com/user-attachments/assets/15c0a24c-6695-41cc-8490-cfb6ac62345f



### 🕵 5. Real Time Name Expose

Imagine you are developing a game where the speaker's name appears in the subtitles, similar to Cyberpunk 2077. You might want to customize this name based on whether the player has actually met or learned the NPC's identity.

In the demo, for example, a character is initially labeled as "Apple Seller." However, after interacting and having him reveal his name, the subtitle dynamically updates to display his real name. 
Additionally, you can apply this same logic to the player's name as well.

https://github.com/user-attachments/assets/0af0f24f-87d0-43fd-9ad2-697ad6f65763




## 🛠️ System Design

### 1. General System Design

The image below illustrates the general system design.

<img width="5312" height="1200" alt="General Architecture" src="https://github.com/user-attachments/assets/36592f83-2cc5-4f5d-99b0-0b5be045fed5" />


### 2. Dialogue Subsystem Design

The image below illustrates the Dialogue Subsystem design.

<img width="7410" height="1773" alt="Subsystem" src="https://github.com/user-attachments/assets/06963401-d8f1-4613-bff9-1637c49e2a3c" />

### 3. Dialogue Writer System Design

The image below illustrates the Dialogue Writer design.


<img width="6280" height="1560" alt="Dialogue Writer Archhitecture" src="https://github.com/user-attachments/assets/603267a8-3218-41ce-9283-18101f30d617" />

## 🔮 Future Roadmap


This project is currently in the Prototype phase. Planned improvements for the next update includes:

- **Requirement Check:** Like many other advanced dialogue systems, I want to bring requirement checks to any dialogue we want.
- **Bark System** 
- **UI Update** 


## ⬇️ Installation

- **Clone or download the project. After that you should place the folder into the Plugins directory of your UE5 project.**
- **Add the 'Interface_MainCharacter' interface to your Player Character.**
- **Add the AC_InteractionSystem actor component to your Player Character.**
- **Create two separate Input Actions for "Interact" and "Skip Dialogue", then add these Input Actions to your Input Mapping Context.**
- **Then, perform the operation shown in the image below within your player character blueprint.**

![Ekran görüntüsü_20-3-2026_54037_](https://github.com/user-attachments/assets/354a3950-646a-4cf1-a9ee-66895a92ba1f)

- **Next, you need to create at least 4 Data Tables. One Data Table for 'Disturb' states, one for your player character's dialogues, one for your player character's choices,
  and one separate Data Table for each of your NPCs. Select the 'Disturb' row structure for the Disturb Data Table(You can fill in the Disturb Data Table if you wish), the 'MainCharacterChoices' row structure for the player character choices Data Table,
  and the 'NPC_Dialogues' row structure for both the player character dialogue Data Table and every NPC dialogue Data Table you create. This is how your Data Tables should look in the final setup:**

![Ekran görüntüsü_20-3-2026_6023_](https://github.com/user-attachments/assets/efd0b7d7-a2be-403f-a2cf-18238087feab)

- **After that, go to the Project Settings->Dialogue System Plus Settings. Then place your Data Tables into the appropriate slots (refer to the image below as an example).**

![Ekran görüntüsü_20-3-2026_6511_](https://github.com/user-attachments/assets/330998fa-e4ac-4c6e-be09-507cbca545f1)

- **Find the NPC you want to integrate into the dialogue system and open its Blueprint. Next, add the AC_InteractionSystem and AC_DialogueSystem actor components.
  After adding them, select AC_DialogueSystem within your NPC Blueprint and navigate to the Details panel. Set the Mood Widget Class to WBP_MoodMeter.
  In the NPC Real Name field, enter your NPC's hidden (unexposed) real name, and in the Official Name field, enter the name as it appears to players in the game.
  Set your NPC's default mood value in the NPC Mood Value field and assign the DT_ExampleNPCDialogues you created earlier to the Data Table NPC field.
  If you do not want to use the real name feature, uncheck the Expose Real Name option. Similarly, if you don't want the mood meter to appear when interacting with the NPC,
  uncheck the Should Show NPC Mood option. (Refer to the image below for an example) (Note: The plugin will crash if your NPC actor is not present in the scene.)**
  
  ![Ekran görüntüsü_20-3-2026_61737_](https://github.com/user-attachments/assets/89ebc631-6599-446d-a3fe-8e45f754c1c4)

- **After that, you will need to initialize the dialogue widget. To do this, access the Dialogue Subsystem within any Blueprint or C++ file (using Event BeginPlay or a similar function) and call its InitializeDialogueUI function. (Make sure to set the DialogueWidgetClass variable to WBP_Dialogue)**

  ![Ekran görüntüsü_20-3-2026_64633_](https://github.com/user-attachments/assets/58ce95a4-8b02-4aa8-80ca-be5582e62999)



- **Then, right-click in the Content Drawer, navigate to Dialogue System Plus, click on DialogueWriter to create it, and then open it.**

- **Once you open the DialogueWriter, you can create your dialogues as you wish (however, there are a few rules):**

  ### Rules

  - **Every dialogue must start with an NPC Dialogue node.**
  - **You cannot connect the output pin of a Choice node to a Player Dialogue node.**
  - **The Speaker Name and the name you entered into the 'NPC Real Name' variable in the AC_DialogueSystem must be exactly the same.**
  - **Entering 'TargetLevel' and 'SpeakerName' values is mandatory for Root NPC Dialogue nodes. However, there is no need to define these variables for subsequent nodes in the same branch, as the DialogueWriter class automatically propagates this data.**
  
                   


 




## ✍️ Author

**Kerem Kaan Tekelek**

- **Mail:** keremtekelek@gmail.com
- **Linkedin:** https://www.linkedin.com/in/kerem-kaan-tekelek/
