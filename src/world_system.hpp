#pragma once

// internal
#include "common.hpp"

// stlib
#include <vector>
#include <random>

#define SDL_MAIN_HANDLED
#include <SDL.h>
#include <SDL_mixer.h>

#include "render_system.hpp"
#include "physics_system.hpp"
//#include <behaviour_tree.hpp>


// Container for all our entities and game logic. Individual rendering / update is
// deferred to the relative update() methods
class WorldSystem
{
public:
    WorldSystem();
    // Creates a window
    GLFWwindow *create_window(int width, int height);

    // starts the game
    void init(RenderSystem *renderer);

    // restart level
    void restart_game(GameStateChange gc = GameStateChange::RESET);

    // Releases all associated resources
    ~WorldSystem();
    void clearEnemy();
    // Steps the game ahead by ms milliseconds
    bool step(float elapsed_ms);

    // Check for collisions
    void handle_collisions();

    // Should the game be over ?
    bool is_over() const;

    void setRenderRequests();
    void progressTutorial(float elapsed_ms_since_last_update);
    void escapeTutorial();

    void progressDoorLockWarningTutorial();
    void progressPreBossTutorial();

    int fbWidth = 0;
    int fbHeight = 0;

    void swap_help_screen();
    void addEnemy(std::string type, int frequency);
    // Handles removing components of battle window, should only be called by BattleSystem
    void findInitialFrameBufferSize();

    // Handles removing components of battle window, should only be called by BattleSystem
    void destroyBattleWindow();

    // Updates the item options rendered on the battle screen
    void updateBattleScreenItemOptions();

    void attach(std::function<void(Entity, bool)> fn);
    void save();
    void incrementEnemiesKilled();

    // gets the renderer from WorldSystem to use in other classes
    RenderSystem* getRenderer();
    void clearBattleMoveUsed();
    // calls from world init
//    void callCreateBattleMoveUsed(BattleMove enemy_move);
    void playMoveSoundEffect(BattleMove move); // called by BattleSystem

    Entity createAnimatronics(bool battleScreen);
	//ComponentContainer<BTNode*> btnodes;
private:
    // Input callback functions
    void on_key(int key, int, int action, int mod);
    void on_mouse_move(vec2 pos);
    void on_mouse_click(int button, int action, int mods);
    void openMenuOverlayScreen();
    void closeMenuOverlayScreen();

    // Updates whether an entity's animation should be paused or not
    void setSpriteAnimationPauseState(Entity entity);

    // Updates which (time) frame should currently be shown for the sprite animation
    void setSpriteAnimationFrame(float elapsed_time_ms, Entity entity);

    // creates the battle window and all its UI elements
    void drawBattleWindow(EnemyName name);

    // helper function that scales UI assets
    void scaleUIAsset(Entity entity, vec2 originalDimensions, float scaleFactor);

    // handles a button click from the battle screen
    void handleBattleScreenButtonClick(Entity button_clicked);

    // handles switching to the appropriate battle menu (attack, magic, or items)
    void switchBattleMenu(Entity menu_button);

    // renders player's attack list as buttons (options) on the battle screen
    void renderPlayerAttackOptions();

    // render the player's magic list as buttons (options) on the battle screen
    void renderPlayerMagicOptions();

    // display the player's inventory items as buttons (options) on the battle screen
    void renderPlayerItemOptions();

    // registers a turn with the chosen game item
    void useChosenGameItem(Entity item_button);

    // resets all player move buttons back to default texture
    void resetPlayerMoveDefaultTextures();

    // clears the player move options from the battle menu's text area
    void clearBattlePlayerMoveButtons();

    // determines which button was clicked, if any, and calls handleBattleScreenButtonClick
    void detectAndHandleBattleScreenClick(double x, double y);

    // helper function that returns whether a mouse click on coordinates x, y occured in a given (rectangular) region
    // where top_left_coords is the coordinate of the region's top left corner
    bool isClickInRegion(double x, double y, vec2 top_left_coords, double height, double width);

    void escapeDialogue(Entity e);

    void getScreenSize();

    bool hasMementoForCurrentLevel(Entity player_doll, int level);
	// returns true if current GameState is BATTLE or BOSS
	bool inBattleOrBossState();

    // OpenGL window handle
    GLFWwindow *window;

    // Number of fish eaten by the salmon, displayed in the window title
    unsigned int points;

    std::vector<std::function<void(Entity, bool)> > callbacks;
    // Game state
    RenderSystem *renderer;
    int enemiesKilled = 0;
    float current_speed;
    float player_speed = 100.f;
    float next_enemy_spawn;
    Entity player_doll;
    Entity shadows[5];
    Entity background;
    Entity helpScreen;
    Entity menuButton;
    Entity menuOverlay;
    Entity tutorialScreen;
    Entity doorLockTutorialScreen;
    Entity room1Dialogue;
    Entity room2Dialogue;

    // Entities related to rendering battle screen
    Entity battle_screen;
    Entity battle_doll;
    Entity battle_enemy;
    Entity healthbar;

    // Parallax background related entities
    Entity parallax_background;
    Entity parallax_layer_0;
    Entity parallax_layer_1;
    Entity parallax_layer_2;
    Entity second_parallax_layer_0;
    Entity second_parallax_layer_1;
    Entity second_parallax_layer_2;

    // Animatronic related entities
    Entity left_arm_1;
    Entity right_arm_1;
    Entity right_arm_2;
    Entity left_arm_2;
    Entity left_leg_1;
    Entity right_leg_1;
    Entity left_leg_2;
    Entity right_leg_2;
    Entity torso;

    // enemy stuff
    std::unordered_map<std::string, int> enemyMap;
    std::vector<std::string> enemyTypes;

    // game state
    GameProgress gameProgress;

    // Other variables related to battle screen
    BattleMenuItemType selected_move_menu; // Either attacks, magic, or items

    // music references
    Mix_Chunk* battle_music;
    Mix_Music* main_theme_music;
    Mix_Chunk *lightning_sound;
    Mix_Chunk* battle_hit_sound;
    Mix_Chunk* fireball_sound;
    Mix_Chunk* splash_sound;
    Mix_Chunk* power_up_sound;
    Mix_Chunk* shield_sound;
    Mix_Chunk* taunt_sound;
    Mix_Chunk* darkness_sound;
    Mix_Chunk* thorns_sound;
    Mix_Chunk* poison_sound;
    Mix_Chunk* heal_sound;
    Mix_Chunk* menu_click_sound;
    Mix_Chunk* get_item_sound;

    // C++ random number generator
    std::default_random_engine rng;
    std::uniform_real_distribution<float> uniform_dist; // number between 0..1
    void reset();

    void generateParallaxWindows();

    void handleTransition();

    /*
    Various constants related to rendering battle screen
    */
    float SCALE = window_height_px / 160; // Ratio used to scale assets drawn in Aseprite

    std::map<AttackName, vec2> attackOptionOriginalDimensions = {
        { AttackName::PUNCH, {36, 12}},
        { AttackName::TRIP, {26, 12}},
    };

    std::map<MagicName, vec2> magicOptionOriginalDimensions = {
        { MagicName::LIGHTNING_STRIKE, {60, 14}},
        { MagicName::SHIELD, {40, 12}},
        { MagicName::SPLASH, {42, 14}},
        { MagicName::POISON_CLOUD, {42, 12}},
        { MagicName::TAUNT, {34, 10}},
        { MagicName::ARMOR_UP, {54, 12}},
        { MagicName::MAGIC_RESIST_UP, {54, 12}},
        { MagicName::MIND_AND_BODY, {62, 14}},
        { MagicName::QUICK_STEP, {64, 14}},
        { MagicName::THORNS, {44, 12}},
        { MagicName::DARKNESS, {56, 12}},
        { MagicName::FIREBALL, {46, 12}},
    };

    std::map<GameItemName, vec2> itemOptionOriginalDimensions = {
        { GameItemName::HEALING_POTION, {70, 12}},
        { GameItemName::HEALING_POTION_2, {70, 12}},
    };

    std::vector<vec2> ATTACK_OPTION_POSITIONS = {
        { 90 * SCALE, 132 * SCALE - 73 }, //top left
        { 90 * SCALE, 132 * SCALE }, // middle left
        { 90 * SCALE, 132 * SCALE + 73 }, // bottom left
        { 180 * SCALE, 132 * SCALE - 73 }, // top right
        { 180 * SCALE, 132 * SCALE + 2 }, // middle right
        { 180 * SCALE, 132 * SCALE + 73 }, // bottom right
    };

    std::vector<vec2> MAGIC_OPTION_POSITIONS = {
        { 90 * SCALE, 132 * SCALE - 73 + 2 }, //top left
        { 90 * SCALE, 132 * SCALE + 2}, // middle left
        { 90 * SCALE, 132 * SCALE + 73 + 2 }, // bottom left
        { 180 * SCALE, 132 * SCALE - 73 + 2}, // top right
        { 180 * SCALE, 132 * SCALE + 2}, // middle right
        { 180 * SCALE, 132 * SCALE + 73 + 2}, // bottom right
    };

    std::vector<vec2> ITEM_OPTION_POSITIONS = {
        {105 * SCALE, 132 * SCALE - 73 + 2}, //top left
        {105 * SCALE, 132 * SCALE + 2}, // middle left
        {105 * SCALE, 132 * SCALE + 73 + 2}, // bottom left
        {180 * SCALE, 132 * SCALE - 73 + 2}, // top right
        {180 * SCALE, 132 * SCALE + 2}, // middle right
        {180 * SCALE, 132 * SCALE + 73 + 2}, // bottom right
    };

    void RotateAnimatronicJoints(float angle) const;

    void createAnimatronics();

    void setNextDestination(vec2 &leftFootDestination, vec2 &rightFootDestination, vec2 &leftHandDestination,
                            vec2 &rightHandDestination) const;
    void RotateLimbs(float elapsedTime, float rotationCycleTime, const vec2 &Destination,
                     std::map<LimbGroup, float> &limbRotation, LimbGroup upperLimbGroup, LimbGroup lowerLimbGroup) const;

    void findRandomDestination(vec2 &destination) const;
};