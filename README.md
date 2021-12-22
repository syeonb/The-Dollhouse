# The Dollhouse 🏠
2D RPG game built using C++ and OpenGL.  
Inspired by games such as The Binding of Isaac, Pokemon and EarthBound.  
Created as part of the CPSC 427 (Video Game Programming) course at the University of British Columbia.  
Received jury award for Best Game and Best Graphics during CPSC 427's 2021W1 offering.  
The demo video of this game can be found [here](https://kapwi.ng/c/FxYvB78AYC).  

## Team Members
[Rosaline Baek](https://github.com/syeonb)  
[Liam Coyle](https://github.com/liamliam)  
[Naoreen Kabir](https://github.com/kNaoreen)  
[Peter Le](https://github.com/ptwell)  
[Preet Nakrani](https://github.com/preetnakrani)  
[Hannah Yang](https://github.com/hannahy00)  

## Gameplay
* The player finds themselves in a dollhouse which they should escape to win the game
* The player must progress through each room to reach the final hallway room, where the boss resides
* In order to unlock the door to the next room, the player must pick up a "memento" object to unlock the diary entry and progress the story
* Enemies such as the Dust Bunny and Teddy Bear appear in each room and pursue the player once the player is within detection range
* Once colliding into an enemy, a turn-based battle commences. The player can either choose normal attacks or magic attacks. Magic attacks drain Mana but are more effective, especially if used consecutively with related magic attacks. (For the most efficient attack, try splashing the enemy then striking them with lightening!)
* Pick up potions to regain health and mana points. The player's inventory has a limit of 6 potions which they can take during the turn-based battle. 
* Upon losing the game the player is given the option to quit the game or restart. Restarting the game will reset the save file.
* Once the player wins against two boss battles, they win the game. Upon pressing enter, a credits scene with all team members is displayed.

## Implementation Highlights
* **Overworld Enemy AI:** Enemies behave in three different ways by idling, roaming in their respective patterns to their enemy type, or chasing the player if they have detected the player within their detection range. Once within range, the enemy finds the fastest route to get to the player based on BFS in a projected grid of available area to move in, accounting for walls and other obstacles.
* **Battle Enemy AI:** The enemy AI determines what action to do depending on the statuses, health, and health difference of both the enemy and the player, as well as whether it has an item to use during battle.
* **Boss AI:** At the beginning of the boss battle the enemy AI generates a tree with depth 5 of all possible moves and responses to moves for the next turns. Every turn the enemy AI will use this tree to simulate all possible permutations the battle could take for the tree depth. It will then score each move option based on the average differential between player and enemy health it results. The enemy will make the best move most of the time (70%), and the second best move the rest of the time. The tree is only generated once and reused because the available moves are the same regardless of where in the battle you are (no disabling or gaining moves). 
* **Light & Shadow:** Light effects present in the scene are all handled in the fragment shader. Elapsed time in the application is used to calculate the varying light patterns (Christmas tree lights, fireplace, flickering ceiling lights). In the hallway, the ceiling lights are created using trigonometry and distance from the light source. The length, direction and intensity of the player's shadows are handled in the fragment and geometry shader, and are affected by the player's relative position to all the light sources present in the room. 
* **Animatronic (Kinematic Chain & Inverse Kinematics):** The second boss is an animatronic version of the first boss. The animatronic's limbs are all separately imported into the game as entities and are rotated to meet each arm and leg's goal position, which is randomly assigned every 7 seconds. The rotation of each limb is determined by inverse kinematics. Linear algebra was used to translate and rotate each limb around their respective origins. 
* **Saving, reloading, multithreading:** The game's status, such as the player's health, mana level and the player's position, is all saved as a JSON file automatically (gameProgress.json) - there is no need for the player to save the progress explicitly. Upon re-opening the game, the game reads the existing save data and reloads the game to its last state. JSON files were created for each level in order to load the necessary components for the given level (such as the position of doors, items and mementos). A multi-threading system and a synchronization system is implemented to parallelize these loading tasks. 
* **Game balance**: An equation was derived to determine whether the player's health was balanced with the damage of the enemies. A change log was made showing what changes have been made to make it more balanced, where each row of the change log represents one play test. The change log describes the area of change, reason for change, description of change and the results of that change. See M4_GameBalancing(Health).pdf for more details.  

## Useful Features
* **Speed Boost:** The game is designed for varied playing styles. It can be played without battling a single enemy if the player can dodge the overworld enemies and fetch the memento fast enough. To run past enemies, drag your mouse across the direction of your character's movement. A faster drag yields a greater speed boost. 
* **Health Potions:** A few blue (mega) potions and pink (mini) potions are scattered around in the overworld. Mini potions restore a small amount of the player's health while the mega potions restore a large amount of the player's health as well as their mana points. 
* **Clickable Menu:** On the top right of the screen, there is a clickable menu that lets the user either restart the game or close the game. Upon opening the game again after closing it, the game will be restored to its very last state. Clicking the menu button again after opening it will close the menu. 
* **Reset Shortcut Key:** In the case that an unexpected behaviour happens, or if the player wants to reset the game without using the clickable menu on the top-right of the screen, they can simply press the "R" key to reset the game and its save file. The game will return to the starting splash screen. 
* **Volume Control:** The player can increase and decrease the volume of the game by using up and down arrows. 
* **Tips Whiteboard:** In the attic, there is a whiteboard on the wall. Using the "N" and "M" key the player can view tips which hint at the gameplay in a mysterious fashion.  
* **Graphical debugging:** Press the "/" key during the game to view walls, player and enemy colliders, and the grid that the overworld enemy AI uses to calculate the most optimal path to approach the player.
* **Resizable window:** The game can be resized to be smaller or larger than the initial size. 

## Credits
* This project is based on the C++ & OpenGL assignment template and tinyECS provided by the CPSC 427 teaching team. 
* This project uses [nlohmann's JSON parsing library](https://github.com/nlohmann/json) to parse pre-saved level configurations and player progress.
* All visual assets were created by team members (Naoreen, Rosaline, Liam, Hannah).
* All music compositions were created by Naoreen. 
* Sound effects are creative commons licensed, with credits in the AUDIO_CREDIT.txt file.
