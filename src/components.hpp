#pragma once

#include "common.hpp"
#include <vector>
#include <unordered_map>
#include "../ext/stb_image/stb_image.h"
#include <map>

// Player component
struct Player {
    int fBWidth;
    int fBHeight;
};

// Please keep this current when we add more enemies, and make sure the ENEMY_NAME_COUNT field is updated
enum class EnemyName
{
	DUST_BUNNY = 0,
	TEDDY_BEAR = 1,
	FLORA_1 = 2,
	FLORA_2 = 3,
	ENEMY_NAME_COUNT = FLORA_2 + 1
};

struct Enemy {
	EnemyName name;
};

struct Boss {
};

struct Flora {

};

struct Bunny
{

};

struct Bear
{

};

struct AIEntity {
    std::vector<std::pair<int, int>> coveredPositions = {};
};

struct AnimatedSprite {
    bool paused = true;
    // maps how many frames are drawn for each animation type in the sprite sheet
    // animation type is the key, and the number of frames is the value
    std::map<int, int> num_frames_per_type = {};

    float frame_height = 0.25;
    float frame_width = 0.25;

    float animation_speed_ms = 250;
    float ms_since_last_update = 0;

    int current_type = 0;  // y axis of sprite sheet
    int current_frame = 0; // x axis of sprite sheet
};

struct Wall {
};

struct Shadow {
	int num = 0;
};

struct Door
{
};

struct Health
{
	int health = 0;
	int healthDecrement = 0;
	int maxHP = 0;
};

struct Mana
{
	int mana = 0;
	int maxMana = 0;
};

enum class Direction {
    UP = 0,
    RIGHT = 1,
    DOWN = 2,
    LEFT = 3
};

// All data relevant to the shape and motion of entities
struct Motion {
    vec2 position = {0, 0};
    float angle = 0;
    vec2 velocity = {0, 0};
    vec2 scale = {10, 10};
    Direction dir = Direction::DOWN;
    int collision_proof = 0;
};

struct Momentum {
    float count_ms = 1000;
    float decrement = 0;
    vec2 velocity;
};

// Stucture to store collision information
struct Collision {
    // Note, the first object is stored in the ECS container.entities
    Entity other; // the second object involved in the collision
    Collision(Entity &other) { this->other = other; };
};

enum class BattleState {
    NONE = 1,
    START = 2,
    PLAYER_TURN = 3,
    ENEMY_TURN = 4,
    WON = 5,
    LOST = 6,
};

enum class EnemyState {
    NONE = 1,
    HEALTHY = 2,
    DAMAGED = 3,
    NEAR_DEATH = 4,
    BOLD = 5,
    THREATENED = 6,
    TAUNTED = 7,
};

struct StatusEffect
{
	bool NONE = true;

	bool TAUNTED = false;
	int tauntTimer = 0;

	bool POISONED = false;
	int poisonTimer = 0;
	int poisonDamage = 0;

	bool SHIELDED = false;
	float shield_amount = 0;
	int shieldTimer = 0;

	bool THORNS = false;
	float thorns_amount = 0;
	int thornTimer = 0;

	bool IGNITED = false;
	int ignite_damage = 0;
	int ignitedTimer = 0;

	int EVASION = 0;
	int evasionTimer = 0;

	float physical_defense = 0;
	float physical_reduction = 0;
	float physTimer = 0;

	float magical_defense = 0;
	float magical_reduction = 0;
	float magiTimer = 0;

	bool WET = false;
};

// An enemy that the player is currently engaging in battle with
struct CurrentEnemy {
    EnemyState enemyState = EnemyState::HEALTHY;
};

enum class MoveType
{
	NONE = 0,
	ATTACK = NONE + 1,
	MAGIC = ATTACK + 1,
	ITEM = MAGIC + 1,
};

struct BattleMove {
	std::string nameAsString = "";
	MoveType moveType = MoveType::NONE;
};

enum class AttackName
{
	NONE = 0,
	PUNCH = NONE + 1,
	SNEEZE = PUNCH + 1, // dust bunny
	SCRATCH = SNEEZE + 1,
	TRIP = SCRATCH + 1,
};

enum class AttackType {
    NORMAL = 0,
    NOTNORMAL = 1,
};

struct Attack
{
	AttackName name;
	AttackType type = AttackType::NORMAL;
	float damage = 0;

	std::string nameAsString()
	{
		switch (this->name)
		{
		case AttackName::PUNCH:
			return "PUNCH";
		case AttackName::SNEEZE:
			return "SNEEZE";
		case AttackName::SCRATCH:
			return "SCRATCH";
		case AttackName::TRIP:
			return "TRIP";
		default:
			return "";
		}
	}
};

struct AttackList
{
	std::vector<Attack> available_attacks;

	void addAttack(AttackName name, AttackType type, float damage)
	{
		Attack attack = {};
		attack.name = name;
		attack.type = type;
		attack.damage = damage;
		available_attacks.push_back(attack);
	}

    void addAttack(Attack a) {
        available_attacks.push_back(a);
    }

    bool hasAttack(AttackName name) {
        // https://stackoverflow.com/questions/15517991/search-a-vector-of-objects-by-object-attribute
        auto iterator = find_if(available_attacks.begin(), available_attacks.end(),
                                [&name](const Attack &attack) { return attack.name == name; });

        return iterator != available_attacks.end();
    }

    Attack getAttack(AttackName name) {
        auto iterator = find_if(available_attacks.begin(), available_attacks.end(),
                                [&name](const Attack &attack) { return attack.name == name; });

        if (iterator != available_attacks.end()) {
            return *iterator;
        } else {
            return {};
        }
    }
};

enum class MagicName
{
	NONE = 0,
	LIGHTNING_STRIKE = NONE + 1,
	SHIELD = LIGHTNING_STRIKE + 1,
	POISON_CLOUD = SHIELD + 1,
	TAUNT = POISON_CLOUD + 1,
	FIREBALL = TAUNT + 1,
	THORNS = FIREBALL + 1,
	ARMOR_UP = THORNS + 1,
	MAGIC_RESIST_UP = ARMOR_UP + 1,
	MIND_AND_BODY = MAGIC_RESIST_UP + 1,
	QUICK_STEP = MIND_AND_BODY + 1,
	SPLASH = QUICK_STEP + 1,
	DARKNESS = SPLASH + 1,

};

enum class MagicType
{
	ATTACK = 0,
	DEFENSE = ATTACK + 1,
	STATUS_EFFECT = DEFENSE + 1, // Magic that changes the target's status
	NONE = STATUS_EFFECT + 1,
};

struct Magic
{
	MagicName name;
	int manaCost;

	std::string nameAsString()
	{
		switch (this->name)
		{
		case MagicName::LIGHTNING_STRIKE:
			return "LIGHTNING_STRIKE";
		case MagicName::SHIELD:
			return "SHIELD";
		case MagicName::POISON_CLOUD:
			return "POISON";
		case MagicName::TAUNT:
			return "TAUNT";
		case MagicName::FIREBALL:
			return "FIREBALL";
		case MagicName::THORNS:
			return "THORNS";
		case MagicName::ARMOR_UP:
			return "ARMOR_UP";
		case MagicName::MAGIC_RESIST_UP:
			return "MAGIC_RESIST_UP";
		case MagicName::MIND_AND_BODY:
			return "MIND_AND_BODY";
		case MagicName::QUICK_STEP:
			return "QUICK_STEP";
		case MagicName::SPLASH:
			return "SPLASH";
		case MagicName::DARKNESS:
			return "DARKNESS";

		default:
			return "";
		}
	}

	MagicType magicType = MagicType::NONE;
	AttackType attack_type = AttackType::NORMAL;
	// could be damage or shield or anything really
	float amount = 0;

	// x\ =\ t\cdot\log\left(t\ +\ 50\right)\ -\frac{t}{2}
	float physical_defense_boost = 0;
	float magic_defense_boost = 0;
	int timer = 0;
	int countdown = 0;
	bool isTemporary;
};

struct MagicList
{
	std::vector<Magic> available_magic_attacks;
	std::vector<Magic> available_magic_defenses;
	std::vector<Magic> available_magic_effects;

    void addMagic(Magic m) {
        MagicType mt = m.magicType;
        switch (mt) {
            case MagicType::ATTACK:
                available_magic_attacks.push_back(m);
                break;
            case MagicType::DEFENSE:
                available_magic_defenses.push_back(m);
                break;
            case MagicType::STATUS_EFFECT:
                available_magic_effects.push_back(m);
                break;
            default:
                break;
        }
    }

	void addMagicAttack(MagicName name, AttackType attack_type, float damage, int manaCost)
	{
		Magic magic_attack = {};
		magic_attack.name = name;
		magic_attack.magicType = MagicType::ATTACK;
		magic_attack.attack_type = attack_type;
		magic_attack.amount = damage;
		magic_attack.manaCost = manaCost;
		available_magic_attacks.push_back(magic_attack);
	}

	void addMagicDefense(MagicName name, int physical_defense_boost, int magic_defense_boost, int timer, float amount, int manaCost)
	{
		Magic magic_defense = {};
		magic_defense.name = name;
		magic_defense.amount = amount;
		magic_defense.magicType = MagicType::DEFENSE;
		magic_defense.physical_defense_boost = physical_defense_boost;
		magic_defense.magic_defense_boost = magic_defense_boost;
		magic_defense.timer = timer;
		magic_defense.manaCost = manaCost;
		available_magic_defenses.push_back(magic_defense);
	}

	void addMagicEffect(MagicName name, bool isTemporary, int timer, float amount, int manaCost)
	{
		Magic magic_effect = {};
		magic_effect.amount = amount;
		magic_effect.name = name;
		magic_effect.magicType = MagicType::STATUS_EFFECT;
		magic_effect.isTemporary = isTemporary;
		magic_effect.timer = timer;
		magic_effect.manaCost = manaCost;
		available_magic_effects.push_back(magic_effect);
	}

	bool hasMagic(MagicName name, MagicType type)
	{
		if (type == MagicType::ATTACK) {
			auto iterator = find_if(available_magic_attacks.begin(), available_magic_attacks.end(), [&name](const Magic& magic)
				{ return magic.name == name; });

			return iterator != available_magic_attacks.end();
		}
		else if (type == MagicType::DEFENSE) {
			auto iterator = find_if(available_magic_defenses.begin(), available_magic_defenses.end(), [&name](const Magic& magic)
				{ return magic.name == name; });

			return iterator != available_magic_defenses.end();
		} 
		else if (type == MagicType::STATUS_EFFECT) {
			auto iterator = find_if(available_magic_effects.begin(), available_magic_effects.end(), [&name](const Magic& magic)
				{ return magic.name == name; });

			return iterator != available_magic_effects.end();
		}
		

		return false;
	}

	Magic getAttack(MagicName name)
	{
		auto iterator = find_if(available_magic_attacks.begin(), available_magic_attacks.end(), [&name](const Magic &magic)
								{ return magic.name == name; });

		if (iterator != available_magic_attacks.end())
		{
			return *iterator;
		}

		iterator = find_if(available_magic_defenses.begin(), available_magic_defenses.end(), [&name](const Magic& magic)
			{ return magic.name == name; });
		
		if (iterator != available_magic_defenses.end())
		{
			return *iterator;
		}

		iterator = find_if(available_magic_effects.begin(), available_magic_effects.end(), [&name](const Magic& magic)
			{ return magic.name == name; });

		if (iterator != available_magic_effects.end())
		{
			return *iterator;
		}


		return {};
		
	}
};

enum class GameItemName {
	NONE = 0,
	HEALING_POTION = 1, //mini - just recovers health
	HEALING_POTION_2 = 2, //mega - recovers both health and mana
};

enum class MementoName {
	NONE = 0,
	PIG = NONE + 1,
	CUPCAKES = PIG + 1,
	BRUSH = CUPCAKES + 1,
	REMOTE = BRUSH + 1,
	BIKE = REMOTE + 1,
	FLOWER = BIKE + 1,
	TEA = FLOWER + 1,
	DRAWING = TEA + 1
};

struct GameItem
{
    GameItemName item_name;
    float timer = 0;
    int health = 0;
    int speed = 0;
    bool enemyRepel = false;
    bool timed = false;
	int world_id = 0; // only assign a non-zero value if this item appears in the overworld
	int mana = 0;

	std::string nameAsString() {
		switch (this->item_name)
		{
		case GameItemName::HEALING_POTION:
			return "MINI POTION";
		case GameItemName::HEALING_POTION_2:
			return "MEGA POTION";
		default:
			return "";
		}
	}
};

struct Inventory
{
	std::vector<GameItem> items;
	std::vector<int> all_collected_item_ids;

	void addItem(GameItem item) {
		items.push_back(item);
		if (item.world_id > 0) {
			all_collected_item_ids.push_back(item.world_id);
		}
	}

	GameItem getItem(int item_id) {
		auto iterator = find_if(items.begin(), items.end(),
			[&item_id](const GameItem& game_item) { return game_item.world_id == item_id; });

		if (iterator != items.end()) {
			return *iterator;
		}
		else {
			return {};
		}
	}

	void removeItem(int item_id) {
		auto iterator = find_if(items.begin(), items.end(),
			[&item_id](const GameItem& game_item) { return game_item.world_id == item_id; });

		if (iterator != items.end()) {
			items.erase(iterator);
		}
	}

    int MAX_ITEMS = 0;
};

struct MementoInventory
{
	std::vector<int> collected_memento_ids;

	void addItem(MementoName memento) {
		collected_memento_ids.push_back((int) memento);
	}
};

// Register this with a player or enemy so their turns can be applied in the Battle system
struct Turn {
    MoveType move_type = MoveType::NONE;
    AttackName attack_name = AttackName::NONE;
    MagicName magic_name = MagicName::NONE;
    GameItemName item_name = GameItemName::NONE;
	int item_id = 0; // Corresponds to GameItem world_id for player's inventory items

    Turn() = default;

    Turn(AttackName attack_name) {
        this->move_type = MoveType::ATTACK;
        this->attack_name = attack_name;
    }

    Turn(MagicName magic_name) {
        this->move_type = MoveType::MAGIC;
        this->magic_name = magic_name;
    }

    Turn(GameItemName item_name) {
        this->move_type = MoveType::ITEM;
        this->item_name = item_name;
    }

	Turn(GameItemName item_name, int item_id) {
		this->move_type = MoveType::ITEM;
		this->item_name = item_name;
		this->item_id = item_id;
	}
};

struct Squish {
	// for advanced geometry shader - target is squished or deformed when they receive punch or other attacks
	float countdown = 50;
};

struct TutorialTimer {
    float timePerSprite = 3000.f;
    bool tutorialCompleted = false;
    int tutorialIndex = 0;
};

// Data structure for toggling debug mode
struct Debug {
    bool in_debug_mode = 0;
    bool in_freeze_mode = 0;
};
extern Debug debugging;

enum class GameState {
    PLAYING = 0,
    GAMEOVER = 1,
    STORYSCREEN = 2,
    BOSS = 3,
    BATTLE = 4,
    MENUOVERLAY = 5,
    TUTORIAL = 6,
    POPUP = 7, // for dialogue and diary
    GOINGTOBOSS = 8,
    ENDINGBOSS = 9,
    TRANSITION = 10,
};

// Sets the brightness of the screen
struct Game {
    GameState state = GameState::PLAYING;
    BattleState battle_state = BattleState::NONE;
    float timer = 0;
};

// A struct to refer to debugging graphics in the ECS
struct DebugComponent {
    // Note, an empty struct has size 1
};

// Single Vertex Buffer element for non-textured meshes (coloured.vs.glsl & salmon.vs.glsl)
struct ColoredVertex {
    vec3 position;
    vec3 color;
};

// Single Vertex Buffer element for textured sprites (textured.vs.glsl)
struct TexturedVertex {
    vec3 position;
    vec2 texcoord;
};

// Backgrounds
struct Background {
    int blur_state = 0;
};

struct ClickableArea {
    vec2 topLeft;
    int width;
    int height;
};

// Help screen
struct HelpScreen {
    int order = 0;
};

// for dialogue and diary entries
struct PopUp {
    int order = 0;
};

struct BattleScreen {
};

struct BattleDoll {
};

struct Healthbar
{
};

struct BattleEnemy
{
};

enum class BattleMenuItemType {
    NONE = 0, // Do not assign this to any assets
    BUTTON_AREA = 1,
    TEXT_AREA = 2,
    ATTACK_BUTTON = 3,
    MAGIC_BUTTON = 4,
    ITEMS_BUTTON = 5,
    LEARN_BUTTON = 6, // User can click on this to learn more about the move they selected
    GO_BUTTON = 7,      // It's more like a "submit" button, but I wanted to save space on the UI - Naoreen
    ATTACK_PUNCH = 8,
    ITEM_POTION_MINI = 9,
    ITEM_POTION_MEGA = 10,
	MAGIC_SHIELD = 11,
	MAGIC_LIGHTNING = 12,
};

struct BattleMenu {
    BattleMenuItemType menu_type;
};

struct BattleMenuButton {
    BattleMenuItemType button_type;
};

struct BattleParallaxBackground {
};

struct BattleUsedMove
{

};
struct GlowingObjects{

};

enum class GoalPositionGroup {
    RIGHT_HAND = 0,
    LEFT_HAND = 1,
    RIGHT_FOOT = 2,
    LEFT_FOOT = 3
};

enum class LimbGroup {
    RUA = 0,
    RLA = 1,
    LUA = 2,
    LLA = 3,
    RUL = 4,
    RLL = 5,
    LUL = 6,
    LLL = 7
};

struct AnimatronicMovementInfo {
//    std::map<GoalPositionGroup, vec2> goalPosition = {
//            {GoalPositionGroup::RIGHT_HAND, {0, 0}},
//            {GoalPositionGroup::LEFT_HAND, {0, 0}},
//            {GoalPositionGroup::RIGHT_FOOT, {0, 0}},
//            {GoalPositionGroup::LEFT_FOOT, {0, 0}},
//    };
    std::map<GoalPositionGroup, vec2> goalPosition = {
            {GoalPositionGroup::RIGHT_HAND, {1, 1}},
            {GoalPositionGroup::LEFT_HAND, {1, 1}},
            {GoalPositionGroup::RIGHT_FOOT, {0, 0}},
            {GoalPositionGroup::LEFT_FOOT, {0, 0}},
    };
    std::map<LimbGroup, float> limbRotation = {
            {LimbGroup::LUA, 0},
            {LimbGroup::LLA, 0},
            {LimbGroup::RUA, 0},
            {LimbGroup::RLA, 0},
    };
};

struct Animatronics {
    GoalPositionGroup gpg;
    LimbGroup limbGroup;
    vec2 originalPosition;
};

struct ScrollableBackground {
    vec2 playerRelativePos;
};

// Mesh data structure for storing vertex and index buffers
struct Mesh {
    static bool loadFromOBJFile(std::string obj_path, std::vector<ColoredVertex> &out_vertices,
                                std::vector<uint16_t> &out_vertex_indices, vec2 &out_size);

    vec2 original_size = {1, 1};
    std::vector<ColoredVertex> vertices;
    std::vector<uint16_t> vertex_indices;
};

/**
 * The following enumerators represent global identifiers refering to graphic
 * assets. For example TEXTURE_ASSET_ID are the identifiers of each texture
 * currently supported by the system.
 *
 * So, instead of referring to a game asset directly, the game logic just
 * uses these enumerators and the RenderRequest struct to inform the renderer
 * how to structure the next draw command.
 *
 * There are 2 reasons for this:
 *
 * First, game assets such as textures and meshes are large and should not be
 * copied around as this wastes memory and runtime. Thus separating the data
 * from its representation makes the system faster.
 *
 * Second, it is good practice to decouple the game logic from the render logic.
 * Imagine, for example, changing from OpenGL to Vulkan, if the game logic
 * depends on OpenGL semantics it will be much harder to do the switch than if
 * the renderer encapsulates all asset data and the game logic is agnostic to it.
 *
 * The final value in each enumeration is both a way to keep track of how many
 * enums there are, and as a default value to represent uninitialized fields.
 */

enum class TEXTURE_ASSET_ID
{
	ENEMIES = 0,
	DOLL = ENEMIES + 1,
	ENEMY_ONE = DOLL + 1,
	ENEMY_TWO = ENEMY_ONE + 1,
	ENEMY_THREE = ENEMY_TWO + 1,
	BOSS = ENEMY_THREE + 1,
	TABLE = BOSS + 1,
	CHAIR = TABLE + 1,
	BED = CHAIR + 1,
	HEALTH_ITEM = BED + 1,
	SPEED_ITEM = HEALTH_ITEM + 1,
	ENEMY_REPEL = SPEED_ITEM + 1,
	BACKGROUND = ENEMY_REPEL + 1,
	HELP_1 = BACKGROUND + 1,
	HELP_2 = HELP_1 + 1,
	HELP_3 = HELP_2 + 1,
	HELP_4 = HELP_3 + 1,
	HELP_5 = HELP_4 + 1,
	MENU_BUTTON = HELP_5 + 1,
	MENU_OVERLAY = MENU_BUTTON + 1,
	TUTORIAL_ONE = MENU_OVERLAY + 1,
	TUTORIAL_TWO = TUTORIAL_ONE + 1,
	TUTORIAL_THREE = TUTORIAL_TWO + 1,
	TUTORIAL_FOUR = TUTORIAL_THREE + 1,
	TUTORIAL_FIVE = TUTORIAL_FOUR + 1,
	BATTLE_BACKGROUND_1 = TUTORIAL_FIVE + 1,
	BATTLE_MENU_BUTTON_AREA = BATTLE_BACKGROUND_1 + 1,
	BATTLE_MENU_TEXT_AREA = BATTLE_MENU_BUTTON_AREA + 1,
	BATTLE_MENU_BUTTON_ATTACK = BATTLE_MENU_TEXT_AREA + 1,
	BATTLE_MENU_BUTTON_MAGIC = BATTLE_MENU_BUTTON_ATTACK + 1,
	BATTLE_MENU_BUTTON_ITEMS = BATTLE_MENU_BUTTON_MAGIC + 1,
	BATTLE_MENU_BUTTON_LEARN = BATTLE_MENU_BUTTON_ITEMS + 1,
	BATTLE_MENU_BUTTON_GO = BATTLE_MENU_BUTTON_LEARN + 1,
	ATTACK_OPTIONS_PUNCH = BATTLE_MENU_BUTTON_GO + 1,
	ATTACK_OPTIONS_PUNCH_SELECTED = ATTACK_OPTIONS_PUNCH + 1,
	PLAYER_TEMP = ATTACK_OPTIONS_PUNCH_SELECTED + 1,
	ENEMY_TEMP = PLAYER_TEMP + 1,
	ROOM1_DIARY = ENEMY_TEMP + 1,
	ROOM1_SPEECH1 = ROOM1_DIARY + 1,
	ROOM1_SPEECH2 = ROOM1_SPEECH1 + 1,
	ROOM1_SPEECH3 = ROOM1_SPEECH2 + 1,
	ROOM1_DIARY1 = ROOM1_SPEECH3 + 1,
	ROOM1_DIARY2 = ROOM1_DIARY1 + 1,
	ROOM1_EXPLORE = ROOM1_DIARY2 + 1,
    LEVEL_1_TOYROOM = ROOM1_EXPLORE + 1,
	LEVEL_2_KITCHEN = LEVEL_1_TOYROOM + 1,
	LEVEL_3_BATHROOM = LEVEL_2_KITCHEN + 1,
	LEVEL_4_HALLWAY = LEVEL_3_BATHROOM + 1,
	LEVEL_4_LIVINGROOM = LEVEL_4_HALLWAY + 1,
	LEVEL_5_GARAGE = LEVEL_4_LIVINGROOM + 1,
	LEVEL_6_GARDEN = LEVEL_5_GARAGE + 1,
	LEVEL_7_STUDYROOM = LEVEL_6_GARDEN + 1,
	LEVEL_8_SISTERS_ROOM = LEVEL_7_STUDYROOM + 1,
	HEALTH_POTION_MINI = LEVEL_8_SISTERS_ROOM + 1,
	HEALTH_POTION_MEGA = HEALTH_POTION_MINI + 1,
	DOOR_ATTIC = HEALTH_POTION_MEGA + 1,
	DOOR_GREEN = DOOR_ATTIC + 1,
	DOOR_WHITE = DOOR_GREEN + 1,
	DOOR_BROWN = DOOR_WHITE + 1,
	DOOR_BROWN_WREATH = DOOR_BROWN + 1,
	DOOR_GARDEN = DOOR_BROWN_WREATH + 1,
	STAR = DOOR_GARDEN + 1,
    PARALLAX_BACKGROUND = STAR + 1,
    PARALLAX_LAYER0 = PARALLAX_BACKGROUND + 1,
    PARALLAX_LAYER1 = PARALLAX_LAYER0 + 1,
    PARALLAX_LAYER2 = PARALLAX_LAYER1 + 1,
	HEALTHBAR_ANNA = PARALLAX_LAYER2 + 1,
	HEALTHBAR_DUSTBUNNY = HEALTHBAR_ANNA + 1,
    HEALTHBAR_TEDDYBEAR = HEALTHBAR_DUSTBUNNY + 1,
    ENEMY_SNEEZE = HEALTHBAR_TEDDYBEAR + 1,
    ITEM_OPTIONS_POTION_MINI = ENEMY_SNEEZE + 1,
    ITEM_OPTIONS_POTION_MINI_SELECTED = ITEM_OPTIONS_POTION_MINI + 1,
    ITEM_OPTIONS_POTION_MEGA = ITEM_OPTIONS_POTION_MINI_SELECTED + 1,
    ITEM_OPTIONS_POTION_MEGA_SELECTED = ITEM_OPTIONS_POTION_MEGA + 1,
	MAGIC_OPTIONS_SHIELD = ITEM_OPTIONS_POTION_MEGA_SELECTED + 1,
	MAGIC_OPTIONS_SHIELD_SELECTED = MAGIC_OPTIONS_SHIELD + 1,
	MAGIC_OPTIONS_LIGHTNING = MAGIC_OPTIONS_SHIELD_SELECTED + 1,
	MAGIC_OPTIONS_LIGHTNING_SELECTED = MAGIC_OPTIONS_LIGHTNING + 1,
	MAGIC_OPTIONS_SPLASH = MAGIC_OPTIONS_LIGHTNING_SELECTED + 1,
	MAGIC_OPTIONS_SPLASH_SELECTED = MAGIC_OPTIONS_SPLASH + 1,
	MAGIC_OPTIONS_POISON = MAGIC_OPTIONS_SPLASH_SELECTED + 1,
	MAGIC_OPTIONS_POISON_SELECTED = MAGIC_OPTIONS_POISON + 1,
	MAGIC_OPTIONS_TAUNT = MAGIC_OPTIONS_POISON_SELECTED + 1,
	MAGIC_OPTIONS_TAUNT_SELECTED = MAGIC_OPTIONS_TAUNT + 1,
	ATTACK_OPTIONS_TRIP = MAGIC_OPTIONS_TAUNT_SELECTED + 1,
	ATTACK_OPTIONS_TRIP_SELECTED = ATTACK_OPTIONS_TRIP + 1,
	MAGIC_OPTIONS_ARMOR = ATTACK_OPTIONS_TRIP_SELECTED + 1,
	MAGIC_OPTIONS_ARMOR_SELECTED = MAGIC_OPTIONS_ARMOR + 1,
	MAGIC_OPTIONS_MIND_AND_BODY = MAGIC_OPTIONS_ARMOR_SELECTED + 1,
	MAGIC_OPTIONS_MIND_AND_BODY_SELECTED = MAGIC_OPTIONS_MIND_AND_BODY + 1,
	MAGIC_OPTIONS_RESIST_UP = MAGIC_OPTIONS_MIND_AND_BODY_SELECTED + 1,
	MAGIC_OPTIONS_RESIST_UP_SELECTED = MAGIC_OPTIONS_RESIST_UP + 1,
	MAGIC_OPTIONS_QUICKSTEP = MAGIC_OPTIONS_RESIST_UP_SELECTED + 1,
	MAGIC_OPTIONS_QUICKSTEP_SELECTED = MAGIC_OPTIONS_QUICKSTEP + 1,
	MAGIC_OPTIONS_THORNS = MAGIC_OPTIONS_QUICKSTEP_SELECTED + 1,
	MAGIC_OPTIONS_THORNS_SELECTED = MAGIC_OPTIONS_THORNS + 1,
	MAGIC_OPTIONS_FIREBALL = MAGIC_OPTIONS_THORNS_SELECTED + 1,
	MAGIC_OPTIONS_FIREBALL_SELECTED = MAGIC_OPTIONS_FIREBALL + 1,
	MAGIC_OPTIONS_DARKNESS = MAGIC_OPTIONS_FIREBALL_SELECTED + 1,
	MAGIC_OPTIONS_DARKNESS_SELECTED = MAGIC_OPTIONS_DARKNESS + 1,
	MEMENTO_OBJECT_1 = MAGIC_OPTIONS_DARKNESS_SELECTED + 1,
	MEMENTO_OBJECT_2 = MEMENTO_OBJECT_1 + 1,
	MEMENTO_OBJECT_3 = MEMENTO_OBJECT_2 + 1,
	MEMENTO_OBJECT_4 = MEMENTO_OBJECT_3 + 1,
	MEMENTO_OBJECT_5 = MEMENTO_OBJECT_4 + 1,
	MEMENTO_OBJECT_6 = MEMENTO_OBJECT_5 + 1,
	MEMENTO_OBJECT_7 = MEMENTO_OBJECT_6 + 1,
	MEMENTO_OBJECT_8 = MEMENTO_OBJECT_7 + 1,
	MEMENTO_TEXT_1 = MEMENTO_OBJECT_8 + 1,
	MEMENTO_TEXT_2 = MEMENTO_TEXT_1 + 1,
	MEMENTO_TEXT_3 = MEMENTO_TEXT_2 + 1,
	MEMENTO_TEXT_4 = MEMENTO_TEXT_3 + 1,
	MEMENTO_TEXT_5 = MEMENTO_TEXT_4 + 1,
	MEMENTO_TEXT_6 = MEMENTO_TEXT_5 + 1,
	MEMENTO_TEXT_7 = MEMENTO_TEXT_6 + 1,
	MEMENTO_TEXT_8 = MEMENTO_TEXT_7 + 1,
    ENEMY_ARMORUP = MEMENTO_TEXT_8 + 1,
    ENEMY_DARKNESS = ENEMY_ARMORUP + 1,
    ENEMY_FIREBALL = ENEMY_DARKNESS + 1,
    ENEMY_LIGHTNINGSTRIKE = ENEMY_FIREBALL + 1,
    ENEMY_MAGICRESISTUP = ENEMY_LIGHTNINGSTRIKE + 1,
    ENEMY_MINDANDBODY = ENEMY_MAGICRESISTUP + 1,
    ENEMY_POISON = ENEMY_MINDANDBODY + 1,
    ENEMY_QUICKSTEP = ENEMY_POISON + 1,
    ENEMY_SCRATCH = ENEMY_QUICKSTEP + 1,
    ENEMY_SHIELD = ENEMY_SCRATCH + 1,
    ENEMY_SPLASH = ENEMY_SHIELD + 1,
    ENEMY_TAUNT = ENEMY_SPLASH + 1,
    ENEMY_THORNS = ENEMY_TAUNT + 1,
    ENEMY_TRIP = ENEMY_THORNS + 1,
    PLAYER_ARMORUP = ENEMY_TRIP + 1,
    PLAYER_DARKNESS = PLAYER_ARMORUP + 1,
    PLAYER_FIREBALL = PLAYER_DARKNESS + 1,
    PLAYER_LIGHTNINGSTRIKE = PLAYER_FIREBALL + 1,
    PLAYER_MAGICRESISTUP = PLAYER_LIGHTNINGSTRIKE + 1,
    PLAYER_MINDANDBODY = PLAYER_MAGICRESISTUP + 1,
    PLAYER_POISON = PLAYER_MINDANDBODY + 1,
    PLAYER_QUICKSTEP = PLAYER_POISON + 1,
    PLAYER_SCRATCH = PLAYER_QUICKSTEP + 1,
    PLAYER_SHIELD = PLAYER_SCRATCH + 1,
    PLAYER_SPLASH = PLAYER_SHIELD + 1,
    PLAYER_TAUNT = PLAYER_SPLASH + 1,
    PLAYER_THORNS = PLAYER_TAUNT + 1,
    PLAYER_TRIP = PLAYER_THORNS + 1,
    PLAYER_SNEEZE = PLAYER_TRIP + 1,
    ENEMY_HEALTHPOTION = PLAYER_SNEEZE + 1,
    PLAYER_HEALTHPOTION = ENEMY_HEALTHPOTION + 1,
    ENEMY_PUNCH = PLAYER_HEALTHPOTION + 1,
    PLAYER_PUNCH = ENEMY_PUNCH + 1,
    NOT_ENOUGH_MANA = PLAYER_PUNCH + 1,
	SPLASH_SCREEN = NOT_ENOUGH_MANA + 1,
	TITLE_SCREEN = SPLASH_SCREEN + 1,
	GAMEOVER = TITLE_SCREEN + 1,
	DOOR_DOLLHOUSE = GAMEOVER + 1,
	FLORA_NORMAL = DOOR_DOLLHOUSE + 1,
	ANIMATRONIC_TORSO = FLORA_NORMAL + 1,
	ANIMATRONIC_ARM_1 = ANIMATRONIC_TORSO + 1,
	ANIMATRONIC_ARM_2 = ANIMATRONIC_ARM_1 + 1,
	ANIMATRONIC_LEG_1 = ANIMATRONIC_ARM_2 + 1,
	ANIMATRONIC_LEG_2 = ANIMATRONIC_LEG_1 + 1,
	HEALTHBAR_FLORA2 = ANIMATRONIC_LEG_2 + 1, 
	TEAM_SPRITESHEET_1 = HEALTHBAR_FLORA2 + 1,
	TEAM_SPRITESHEET_2 = TEAM_SPRITESHEET_1 + 1,
	CREDITS = TEAM_SPRITESHEET_2 + 1,
	YOUWIN = CREDITS + 1,
	HEALTHBAR_FLORA1 = YOUWIN + 1,
	TUTORIAL_ATTIC_1 = HEALTHBAR_FLORA1 + 1,
    TUTORIAL_ATTIC_2 = TUTORIAL_ATTIC_1 + 1,
    TUTORIAL_ATTIC_3 = TUTORIAL_ATTIC_2 + 1,
    TUTORIAL_ATTIC_4 = TUTORIAL_ATTIC_3 + 1,
    TUTORIAL_ATTIC_LOCK_ANNA = TUTORIAL_ATTIC_4 + 1,
    TUTORIAL_ATTIC_LOCK_WARNING = TUTORIAL_ATTIC_LOCK_ANNA + 1,
    TUTORIAL_DOOR_LOCK_WARNING = TUTORIAL_ATTIC_LOCK_WARNING + 1,
    TUTORIAL_HALLWAY_1 = TUTORIAL_DOOR_LOCK_WARNING + 1,
    TUTORIAL_HALLWAY_2 = TUTORIAL_HALLWAY_1 + 1,
    TUTORIAL_HALLWAY_3 = TUTORIAL_HALLWAY_2 + 1,
    TUTORIAL_HALLWAY_4 = TUTORIAL_HALLWAY_3 + 1,
    TUTORIAL_HALLWAY_5 = TUTORIAL_HALLWAY_4 + 1,
    TUTORIAL_HALLWAY_AGAIN = TUTORIAL_HALLWAY_5 + 1,
    TUTORIAL_PREBOSS_1 = TUTORIAL_HALLWAY_AGAIN + 1,
    TUTORIAL_PREBOSS_2 = TUTORIAL_PREBOSS_1 + 1,
    TUTORIAL_PREBOSS_3 = TUTORIAL_PREBOSS_2 + 1,
    TUTORIAL_PREBOSS_4 = TUTORIAL_PREBOSS_3 + 1,
    TUTORIAL_PREBOSS_5 = TUTORIAL_PREBOSS_4 + 1,
    TUTORIAL_PREBOSS_6 = TUTORIAL_PREBOSS_5 + 1,
    TUTORIAL_PREBOSS_7 = TUTORIAL_PREBOSS_6 + 1,
    TUTORIAL_PREBOSS_8 = TUTORIAL_PREBOSS_7 + 1,
    TUTORIAL_PREBOSS_9 = TUTORIAL_PREBOSS_8 + 1,
    TEXTURE_COUNT = TUTORIAL_PREBOSS_9 + 1,
};
const int texture_count = (int)TEXTURE_ASSET_ID::TEXTURE_COUNT;

enum class EFFECT_ASSET_ID
{
	COLOURED = 0,
	SHADOW = COLOURED + 1,
	TEXTURED = SHADOW + 1,
	TEXTURED_ANIMATION = TEXTURED + 1,
	BLUR = TEXTURED_ANIMATION + 1,
	REBLUR = BLUR + 1,
	HELP_SCREEN = REBLUR + 1,
	PEBBLE = HELP_SCREEN + 1,
	DRAW_TO_SCREEN = PEBBLE + 1,
    GLOWING_TEXTURE = DRAW_TO_SCREEN + 1,
	HEALTHBAR_PLAYER = GLOWING_TEXTURE + 1,
	HEALTHBAR_ENEMY = HEALTHBAR_PLAYER + 1,
	EFFECT_COUNT = HEALTHBAR_ENEMY + 1
};
const int effect_count = (int)EFFECT_ASSET_ID::EFFECT_COUNT;

enum class GEOMETRY_BUFFER_ID
{
	SHADOW = 0,
	SPRITE = SHADOW + 1,
	SPRITE_ANIMATION = SPRITE + 1,
	DEBUG_LINE = SPRITE_ANIMATION + 1,
	SCREEN_TRIANGLE = DEBUG_LINE + 1,
	HELP_SCREEN = SCREEN_TRIANGLE + 1,
	GEOMETRY_COUNT = HELP_SCREEN + 1
};
const int geometry_count = (int)GEOMETRY_BUFFER_ID::GEOMETRY_COUNT;

struct RenderRequest
{
	TEXTURE_ASSET_ID used_texture = TEXTURE_ASSET_ID::TEXTURE_COUNT;
	EFFECT_ASSET_ID used_effect = EFFECT_ASSET_ID::EFFECT_COUNT;
	GEOMETRY_BUFFER_ID used_geometry = GEOMETRY_BUFFER_ID::GEOMETRY_COUNT;
};

struct GameProgress
{
	int level = 1;
	Motion motion;
	Health health;
	Mana mana;
	std::vector<Attack> attacks;
    std::vector<Magic> magic;
    std::vector<GameItem> inventory_items;
	// list of item ids of every item the player has picked up in overworld, prevents players from abusing save and reload to get item duplicates
	std::vector<int> all_collected_item_ids;
	std::vector<int> collected_memento_ids;
};

enum class GameStateChange {
    RESET = 0,
    NEXT = RESET + 1,
    LOAD = NEXT + 1,
	GAMEOVER = LOAD + 1
};


// Behaviour Tree Components

enum class BTStates {
	BT_Running,
	BT_Success,
	BT_Failure,
	BT_None
};

struct Behaviour
{
	int detection_range = 0;
	BTStates IDLE = BTStates::BT_None;
	int IDLE_timer = 0;
	BTStates CHASE = BTStates::BT_None;
	BTStates ROAM = BTStates::BT_None;


};

struct Chasers
{

};


struct Roaming {
	EnemyName type;
	bool MOVE_LEFT = false;
	bool MOVE_RIGHT = false;
	bool MOVE_UP = false;
	bool MOVE_DOWN = false;
	int roaming_timer = 0;

	Roaming(EnemyName type) {
		this->type = type;
	}
};


// if moveType is none then it is the root
struct TreeNode {
    MoveType moveType;
    TreeNode * parent;
    Attack a;
    Magic m;
    GameItem g;
    std::vector<TreeNode*> children;
    bool enemy;
};

struct BattleMenuPlayerMove {
	TEXTURE_ASSET_ID default_texture;
	TEXTURE_ASSET_ID selected_texture;
	int item_id = 0; // For item moves only

	AttackName attack_name = AttackName::NONE;
	MagicName magic_name = MagicName::NONE;
	GameItemName item_name = GameItemName::NONE;
};
