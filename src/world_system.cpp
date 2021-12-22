// Header
#include "world_system.hpp"
#include "world_init.hpp"
#include "physics_system.hpp"
// stlib
#include <cassert>
#include <sstream>
#include <iostream>
#include <fstream>
#include <chrono>

#include <chrono>
#include "factory.cpp"
#include "physics_system.hpp"
#include "battle_system.hpp"
#include "job_system.h"
#include <iostream>
#include <../ext/json/single_include/nlohmann/json.hpp>

using json = nlohmann::json;
using Clock = std::chrono::high_resolution_clock;
const float transitionDelay = 1000.f;

float elapsedTimeInRotationCycle = 0.f;
vec2 currentGoalPosition = vec2{0, 0};
float multiplicateFactor = 1.f;

// Game configuration
const size_t MAX_ENEMY = 3;
const size_t ENEMY_DELAY_MS = 2500 * 2;
const int MAX_LEVEL_COUNT = 11;
const int MAX_ENEMY_COUNT = 3;
int HELPSCREEN_INDEX = 0;
vec2 playerPrevPosition = vec2();
bool flag = true;
double old_xPos = 0;
double old_yPos = 0;
Direction prevDir = Direction::DOWN;
//float elapsed_ms = 0;

// story
int TUTORIAL_INDEX = 0;
bool doorHasNotBeenInteracted = true;
bool isDoorTutorial = false;
bool isPreBossTutorial = false;
bool preBossTutorialHasNotBeenCompleted = true;

std::array<TEXTURE_ASSET_ID, 4> helpScreenOptions = {TEXTURE_ASSET_ID::HELP_1,
                                                     TEXTURE_ASSET_ID::HELP_2,
                                                     TEXTURE_ASSET_ID::HELP_3,
                                                     TEXTURE_ASSET_ID::HELP_4};
std::array<TEXTURE_ASSET_ID, 5> tutorialScreenOptions = {TEXTURE_ASSET_ID::TUTORIAL_ONE,
                                                         TEXTURE_ASSET_ID::TUTORIAL_TWO,
                                                         TEXTURE_ASSET_ID::TUTORIAL_THREE,
                                                         TEXTURE_ASSET_ID::TUTORIAL_FOUR};

std::array<TEXTURE_ASSET_ID, 4> atticTutorial = {TEXTURE_ASSET_ID::TUTORIAL_ATTIC_1,
                                                 TEXTURE_ASSET_ID::TUTORIAL_ATTIC_2,
                                                 TEXTURE_ASSET_ID::TUTORIAL_ATTIC_3,
                                                 TEXTURE_ASSET_ID::TUTORIAL_ATTIC_4
                                                 };
std::array<TEXTURE_ASSET_ID, 2> regularDoorLocked = {TEXTURE_ASSET_ID::TUTORIAL_ATTIC_LOCK_ANNA,
                                                     TEXTURE_ASSET_ID::TUTORIAL_DOOR_LOCK_WARNING};
std::array<TEXTURE_ASSET_ID, 2> atticDoorLocked = {TEXTURE_ASSET_ID::TUTORIAL_ATTIC_LOCK_ANNA,
                                                   TEXTURE_ASSET_ID::TUTORIAL_ATTIC_LOCK_WARNING};
std::array<TEXTURE_ASSET_ID, 5> hallway = {TEXTURE_ASSET_ID::TUTORIAL_HALLWAY_1,
                                           TEXTURE_ASSET_ID::TUTORIAL_HALLWAY_2,
                                           TEXTURE_ASSET_ID::TUTORIAL_HALLWAY_3,
                                           TEXTURE_ASSET_ID::TUTORIAL_HALLWAY_4,
                                           TEXTURE_ASSET_ID::TUTORIAL_HALLWAY_5};
std::array<TEXTURE_ASSET_ID, 1> hallwayAgain = {TEXTURE_ASSET_ID::TUTORIAL_HALLWAY_AGAIN};
std::array<TEXTURE_ASSET_ID, 9> preBossBattle = {TEXTURE_ASSET_ID::TUTORIAL_PREBOSS_1,
                                                 TEXTURE_ASSET_ID::TUTORIAL_PREBOSS_2,
                                                 TEXTURE_ASSET_ID::TUTORIAL_PREBOSS_3,
                                                 TEXTURE_ASSET_ID::TUTORIAL_PREBOSS_4,
                                                 TEXTURE_ASSET_ID::TUTORIAL_PREBOSS_5,
                                                 TEXTURE_ASSET_ID::TUTORIAL_PREBOSS_6,
                                                 TEXTURE_ASSET_ID::TUTORIAL_PREBOSS_7,
                                                 TEXTURE_ASSET_ID::TUTORIAL_PREBOSS_8,
                                                 TEXTURE_ASSET_ID::TUTORIAL_PREBOSS_9};

std::array<TEXTURE_ASSET_ID, 2> room1Popups = {TEXTURE_ASSET_ID::ROOM1_SPEECH1,
                                               TEXTURE_ASSET_ID::ROOM1_EXPLORE};

std::array<TEXTURE_ASSET_ID, 4> room2Popups = {TEXTURE_ASSET_ID::ROOM1_SPEECH2,
                                               TEXTURE_ASSET_ID::ROOM1_SPEECH3,
                                               TEXTURE_ASSET_ID::ROOM1_DIARY1,
                                               TEXTURE_ASSET_ID::ROOM1_DIARY2};


std::string createSingleAttackString(Attack &attack);

std::string createSingleGameItemString(GameItem &item);

// Create the fish world
WorldSystem::WorldSystem()
        : points(0), next_enemy_spawn(0.f) {
    // Seeding rng with random device
    rng = std::default_random_engine(std::random_device()());
}

WorldSystem::~WorldSystem() {
    // Destroy music components
    if (battle_music != nullptr)
        Mix_FreeChunk(battle_music);
    if (main_theme_music != nullptr)
        Mix_FreeMusic(main_theme_music);
    if (lightning_sound != nullptr)
        Mix_FreeChunk(lightning_sound);
    if (battle_hit_sound != nullptr)
        Mix_FreeChunk(battle_hit_sound);
    if (fireball_sound != nullptr)
        Mix_FreeChunk(fireball_sound);
    if (splash_sound != nullptr)
        Mix_FreeChunk(splash_sound);
    if (power_up_sound != nullptr)
        Mix_FreeChunk(power_up_sound);
    if (shield_sound != nullptr)
        Mix_FreeChunk(shield_sound);
    if (taunt_sound != nullptr)
        Mix_FreeChunk(taunt_sound);
    if (darkness_sound != nullptr)
        Mix_FreeChunk(darkness_sound);
    if (thorns_sound != nullptr)
        Mix_FreeChunk(thorns_sound);
    if (poison_sound != nullptr)
        Mix_FreeChunk(poison_sound);
    if (heal_sound != nullptr)
        Mix_FreeChunk(heal_sound);
    if (menu_click_sound != nullptr)
        Mix_FreeChunk(menu_click_sound);
    if (get_item_sound != nullptr)
        Mix_FreeChunk(get_item_sound);
    Mix_CloseAudio();

    // Destroy all created components
    registry.clear_all_components();

    // Close the window
    glfwDestroyWindow(window);
}

// Debugging
namespace {
    void glfw_err_cb(int error, const char *desc) {
        fprintf(stderr, "%d: %s", error, desc);
    }
}

// World initialization
// Note, this has a lot of OpenGL specific things, could be moved to the renderer
GLFWwindow *WorldSystem::create_window(int width, int height) {
    ///////////////////////////////////////
    // Initialize GLFW
    glfwSetErrorCallback(glfw_err_cb);
    if (!glfwInit()) {
        fprintf(stderr, "Failed to initialize GLFW");
        return nullptr;
    }

    //-------------------------------------------------------------------------
    // If you are on Linux or Windows, you can change these 2 numbers to 4 and 3 and
    // enable the glDebugMessageCallback to have OpenGL catch your mistakes for you.
    // GLFW / OGL Initialization
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

#if __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
    //	glfwWindowHint(GLFW_RESIZABLE, 0);

    // Create the main window (for rendering, keyboard, and mouse input)
    window = glfwCreateWindow(width, height, "Salmon Game Assignment", nullptr, nullptr);
    if (window == nullptr) {
        fprintf(stderr, "Failed to glfwCreateWindow");
        return nullptr;
    }

    // Setting callbacks to member functions (that's why the redirect is needed)
    // Input is handled using GLFW, for more info see
    // http://www.glfw.org/docs/latest/input_guide.html
    glfwSetWindowUserPointer(window, this);
    auto key_redirect = [](GLFWwindow *wnd, int _0, int _1, int _2, int _3) {
        ((WorldSystem *) glfwGetWindowUserPointer(wnd))->on_key(_0, _1, _2, _3);
    };
    auto cursor_pos_redirect = [](GLFWwindow *wnd, double _0, double _1) {
        ((WorldSystem *) glfwGetWindowUserPointer(wnd))->on_mouse_move({_0, _1});
    };
    auto mouse_click_redirect = [](GLFWwindow *wnd, int _0, int _1, int _2) {
        ((WorldSystem *) glfwGetWindowUserPointer(wnd))->on_mouse_click(_0, _1, _2);
    };
    glfwSetKeyCallback(window, key_redirect);
    glfwSetCursorPosCallback(window, cursor_pos_redirect);
    glfwSetMouseButtonCallback(window, mouse_click_redirect);

    //////////////////////////////////////
    // Loading music and sounds with SDL
    if (SDL_Init(SDL_INIT_AUDIO) < 0) {
        fprintf(stderr, "Failed to initialize SDL Audio");
        return nullptr;
    }
    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) == -1) {
        fprintf(stderr, "Failed to open audio device");
        return nullptr;
    }

    battle_music = Mix_LoadWAV(audio_path("dollhouse_battle.wav").c_str());
    main_theme_music = Mix_LoadMUS(audio_path("TheDollhouse.wav").c_str());
    lightning_sound = Mix_LoadWAV(audio_path("lightning.wav").c_str());
    battle_hit_sound = Mix_LoadWAV(audio_path("battle_hit.wav").c_str());
    fireball_sound = Mix_LoadWAV(audio_path("fireball.wav").c_str());
    splash_sound = Mix_LoadWAV(audio_path("splash.wav").c_str());
    power_up_sound = Mix_LoadWAV(audio_path("power_up.wav").c_str());
    shield_sound = Mix_LoadWAV(audio_path("shield.wav").c_str());
    taunt_sound = Mix_LoadWAV(audio_path("taunt.wav").c_str());
    darkness_sound = Mix_LoadWAV(audio_path("darkness.wav").c_str());
    thorns_sound = Mix_LoadWAV(audio_path("thorns.wav").c_str());
    poison_sound = Mix_LoadWAV(audio_path("poison.wav").c_str());
    heal_sound = Mix_LoadWAV(audio_path("heal.wav").c_str());
    menu_click_sound = Mix_LoadWAV(audio_path("menu_click.wav").c_str());
    get_item_sound = Mix_LoadWAV(audio_path("get_item.wav").c_str());

    if (
            battle_music == nullptr ||
            main_theme_music == nullptr ||
            lightning_sound == nullptr ||
            battle_hit_sound == nullptr ||
            fireball_sound == nullptr ||
            splash_sound == nullptr ||
            power_up_sound == nullptr ||
            shield_sound == nullptr ||
            taunt_sound == nullptr ||
            darkness_sound == nullptr ||
            thorns_sound == nullptr ||
            poison_sound == nullptr ||
            heal_sound == nullptr ||
            menu_click_sound == nullptr ||
            get_item_sound == nullptr
            ) {
        fprintf(stderr,
                "Failed to load sounds\n %s\n %s\n %s\n %s\n %s\n %s\n %s\n %s\n %s\n %s\n %s\n %s\n %s\n %s\n %s\n %s\n make sure the data directory is present",
                audio_path("music.wav").c_str(),
                audio_path("dollhouse_battle").c_str(),
                audio_path("TheDollhouse.wav").c_str(),
                audio_path("lightning.wav").c_str(),
                audio_path("battle_hit.wav").c_str(),
                audio_path("fireball.wav").c_str(),
                audio_path("splash.wav").c_str(),
                audio_path("power_up.wav").c_str(),
                audio_path("shield.wav").c_str(),
                audio_path("taunt.wav").c_str(),
                audio_path("darkness.wav").c_str(),
                audio_path("thorns.wav").c_str(),
                audio_path("poison.wav").c_str(),
                audio_path("heal.wav").c_str(),
                audio_path("menu_click.wav").c_str(),
                audio_path("get_item.wav").c_str());
        return nullptr;
    }

    return window;
}

void WorldSystem::init(RenderSystem *renderer_arg) {
    this->renderer = renderer_arg;
    // Playing background music indefinitely
    Mix_PlayMusic(main_theme_music, -1);
    fprintf(stderr, "Loaded music\n");

    // Set all states to default
    restart_game(GameStateChange::LOAD);
}

std::string createSingleAttackString(Attack &attack) {
    std::string result = "{";

    result += "\"name\":";
    result += std::to_string((int) attack.name);
    result += ",";
    result += "\"damage\":";
    result += std::to_string(attack.damage);
    result += ",";
    result += "\"type\":";
    result += std::to_string((int) attack.type);
    result += "}";
    return result;
}

std::string createAttackString(std::vector<Attack> a) {
    std::string result = "[";
    std::vector<std::string> attacks;
    attacks.reserve(a.size());
    for (auto &i : a) {
        attacks.push_back(createSingleAttackString(i));
    }
    if (!attacks.empty()) {
        result += attacks[0];
    }
    for (int i = 1; i < attacks.size(); i++) {
        result += ",";
        result += attacks[i];
    }
    result += "]";
    return result;
}

std::string createSingleMagicString(Magic &magic) {
    std::string result = "{";
    result += "\"name\":";
    result += std::to_string((int) magic.name);
    result += ",";
    result += "\"magicType\":";
    result += std::to_string((int) magic.magicType);
    result += ",";
    result += "\"attackType\":";
    result += std::to_string((int) magic.attack_type);
    result += ",";
    result += "\"amount\":";
    result += std::to_string(magic.amount);
    result += ",";
    result += "\"physical_defense_boost\":";
    result += std::to_string(magic.physical_defense_boost);
    result += ",";
    result += "\"magic_defense_boost\":";
    result += std::to_string(magic.magic_defense_boost);
    result += ",";
    result += "\"timer\":";
    result += std::to_string(magic.timer);
    result += ",";
    result += "\"countdown\":";
    result += std::to_string(magic.countdown);
    result += ",";
    result += "\"isTemporary\":";
    result += ((magic.isTemporary) ? "true" : "false");
    result += "}";
    return result;
}

std::string createMagicString(std::vector<Magic> m) {
    std::string result = "[";
    std::vector<std::string> magic;
    magic.reserve(m.size());
    for (auto &i : m) {
        magic.push_back(createSingleMagicString(i));
    }
    if (!magic.empty()) {
        result += magic[0];
    }
    for (int i = 1; i < magic.size(); i++) {
        result += ",";
        result += magic[i];
    }
    result += "]";
    return result;
}

std::string createItemListString(std::vector<GameItem> g) {
    std::string result = "[";
    std::vector<std::string> items;
    items.reserve(g.size());
    for (auto &i : g) {
        items.push_back(createSingleGameItemString(i));
    }
    if (!items.empty()) {
        result += items[0];
    }
    for (int i = 1; i < items.size(); i++) {
        result += ",";
        result += items[i];
    }
    result += "]";
    return result;
}

std::string createSingleGameItemString(GameItem &item) {
    std::string result = "{";
    result += "\"item_name\":";
    result += std::to_string((int) item.item_name);
    result += ",";
    result += "\"timer\":";
    result += std::to_string(item.timer);
    result += ",";
    result += "\"health\":";
    result += std::to_string(item.health);
    result += ",";
    result += "\"mana\":";
    result += std::to_string(item.mana);
    result += ",";
    result += "\"speed\":";
    result += std::to_string(item.speed);
    result += ",";
    result += "\"enemyRepel\":";
    result += item.enemyRepel ? "true" : "false";
    result += ",";
    result += "\"timed\":";
    result += item.timed ? "true" : "false";
    result += ",";
    result += "\"world_id\":";
    result += std::to_string(item.world_id);
    result += "}";

    return result;
}

std::string createItemIdString(std::vector<int> id_list) {
    std::string result = "[";
    std::vector<std::string> item_id_string;
    item_id_string.reserve(id_list.size());
    for (auto &i : id_list) {
        item_id_string.push_back(std::to_string(i));
    }
    if (!item_id_string.empty()) {
        result += item_id_string[0];
    }
    for (int i = 1; i < item_id_string.size(); i++) {
        result += ",";
        result += item_id_string[i];
    }
    result += "]";

    return result;
}

std::string createMotionString(Motion m) {
    std::string result = "{";

    result += "\"position\":";
    result += "{";
    result += "\"x\":";
    result += std::to_string(m.position[0]);
    result += ",";
    result += "\"y\":";
    result += std::to_string(m.position[1]);
    result += "}";
    result += ",";

    result += "\"angle\":";
    result += std::to_string(m.angle);
    result += ",";

    result += "\"velocity\":";
    result += "{";
    result += "\"x\":";
    result += std::to_string(m.velocity[0]);
    result += ",";
    result += "\"y\":";
    result += std::to_string(m.velocity[1]);
    result += "}";
    result += ",";

    result += "\"scale\":";
    result += "{";
    result += "\"x\":";
    result += std::to_string(m.scale[0]);
    result += ",";
    result += "\"y\":";
    result += std::to_string(m.scale[1]);
    result += "}";
    result += ",";

    result += "\"dir\":";
    result += std::to_string((int) m.dir);
    result += ",";

    result += "\"collision_proof\":";
    result += ((m.collision_proof) ? "true" : "false");
    result += "}";
    return result;
}

void WorldSystem::handleTransition() {

    registry.remove_all_components_of(background);

    int i = 0;
    while (registry.motions.entities.size() > 1) {
        Entity &curr = registry.motions.entities[registry.motions.entities.size() - 1 - i];
        if ((unsigned int) curr == (unsigned int) player_doll) {
            i++;
        } else {
            registry.remove_all_components_of(curr);
        }
    }
    background = createBackground(renderer, {600.f, 400.f}, (TEXTURE_ASSET_ID::BACKGROUND));
    vec3 color = {0.f, 0.f, 0.f};
    if (registry.colors.has(background)) {
        registry.colors.remove(background);
    }
    registry.colors.insert(background, color);

}

// Update our game world
bool WorldSystem::step(float elapsed_ms_since_last_update) {
    if (registry.game.get(player_doll).state == GameState::TRANSITION) {
        registry.game.get(player_doll).timer -= elapsed_ms_since_last_update;
        if (registry.game.get(player_doll).timer < 0) {
            registry.game.get(player_doll).timer = 0;
            registry.game.get(player_doll).state = GameState::PLAYING;
            restart_game(GameStateChange::NEXT);
        }
        return true;
    }

    // ONLY FOR SCROLLABLE BACKGROUNDS
    if (gameProgress.level == 10 || gameProgress.level == 11) {
        Motion& doll_m = registry.motions.get(player_doll);

        if (gameProgress.level == 11) {
            RotateAnimatronicJoints(elapsed_ms_since_last_update);
            if (doll_m.position.x > 800 && doll_m.position.x < 1000) {
                if (preBossTutorialHasNotBeenCompleted) {
                    preBossTutorialHasNotBeenCompleted = false;
                    tutorialScreen = createPreBossTutorial(renderer);
                    registry.game.get(player_doll).state = GameState::TUTORIAL;
                    isPreBossTutorial = true;
                }
            }

            if (registry.game.get(player_doll).state != GameState::BOSS) {
                for (Entity e : registry.animatronics.entities) {
                    registry.motions.get(e).position.x = 1200 - doll_m.position.x + registry.animatronics.get(e).originalPosition.x;
                }
                registry.motions.get(torso).position.x = 1200 - doll_m.position.x + 600;
            }
        }
        
        if (gameProgress.level == 10) {
            registry.motions.get(registry.floras.entities[0]).position.x = 1200 - doll_m.position.x + 600;
        }
        
        if (gameProgress.level == 10 || gameProgress.level == 11) {
            Motion& background_m = registry.motions.get(background);
            background_m.position.x = 1200 - doll_m.position.x;
            for (int i = 0; i < 5; i++) {
                Motion& shadow_m = registry.motions.get(shadows[i]);
                shadow_m.position = { doll_m.position.x, doll_m.position.y + 55 };
            }
        }
       
    }

    if (registry.game.get(player_doll).state == GameState::ENDINGBOSS) {
        registry.game.get(player_doll).state = GameState::TRANSITION;
        registry.game.get(player_doll).timer = transitionDelay;
        handleTransition();
        // registry.game.get(player_doll).state = GameState::PLAYING; // TODO: remove this and uncoment above and call handle delay to add delay
        // restart_game(GameStateChange::NEXT);
        return true;
    }

    if (registry.game.get(player_doll).state == GameState::GOINGTOBOSS) {
        registry.game.get(player_doll).state = GameState::BOSS;
        drawBattleWindow(registry.enemies.get(registry.bosses.entities[0]).name);
    }

    if (registry.game.get(player_doll).state == GameState::TUTORIAL) {
        progressTutorial(elapsed_ms_since_last_update);
    }
    // Updating window title with points
    std::stringstream title_ss;
    title_ss << "Points: " << points;
    glfwSetWindowTitle(window, "The Dollhouse");

    // Remove debug info from the last step
    while (registry.debugComponents.entities.size() > 0)
        registry.remove_all_components_of(registry.debugComponents.entities.back());

    // Removing out of screen entities
    auto &motions_registry = registry.motions;

    // Remove entities that exit to the right, reset parallax background position when it moves out of the screen
    for (int i = (int) motions_registry.components.size() - 1; i >= 0; --i) {
        Motion &motion = motions_registry.components[i];
        if (motion.position.x - abs(motion.scale.x) > 1200.f) {
            Entity motionEntity = motions_registry.entities[i];
            if (registry.parallaxBackground.has(motionEntity)) {
                motion.position.x = -600.f;
            }
        }
    }

    // Spawning new enemy

    if (gameProgress.level != 10 && gameProgress.level != 11) {
        next_enemy_spawn -= elapsed_ms_since_last_update * current_speed;
        if (registry.enemies.components.size() < MAX_ENEMY && next_enemy_spawn < 0.f &&
            registry.game.has(player_doll) &&
            registry.game.get(player_doll).state == GameState::PLAYING &&
            gameProgress.level < 98) {
            // Reset timer
            vec2 position =
                    vec2(50.f + uniform_dist(rng) * (window_width_px - 100.f),
                         50.f + 250 + uniform_dist(rng) * (2 * 250 - 100.f));

            vec2 bounding = vec2(100.f, 100.f);
            int enemy_type = 0;

            unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
            std::minstd_rand0 generator(seed);

            if (gameProgress.level == 2) {
                if (registry.bears.entities.size() < 2) {

                    float chance = generator() % 100;

                    // 25% Chance of spawning bears on level 2: Max 2 bears
                    if (chance < 25) {
                        enemy_type = 1;
                    }
                }
            } else if (gameProgress.level == 3) {
                if (registry.bears.entities.size() < 3) {

                    float chance = generator() % 100;

                    // 45% Chance of spawning bears on level 3: Max 3 bears
                    if (chance < 45) {
                        enemy_type = 1;
                    }
                }
            } else if (gameProgress.level >= 4) {
                if (registry.bears.entities.size() < 4) {

                    float chance = generator() % 100;

                    // 65% Chance of spawning bears on level 4: Max 4 bears
                    if (chance < 65) {
                        enemy_type = 1;
                    }
                }
            }
            Entity new_enemy;
            if (enemy_type == 1) {
                new_enemy = createEnemy(renderer, position, EnemyName::TEDDY_BEAR);
            } else {
                new_enemy = createEnemy(renderer, position, EnemyName::DUST_BUNNY);
            }

            registry.motions.get(new_enemy).position = position;
            registry.motions.get(new_enemy).scale = bounding;


            for (std::function<void(Entity, bool)> fn : callbacks) {
                fn(new_enemy, false);
            }

            if (registry.motions.has(new_enemy)) {
                next_enemy_spawn = (ENEMY_DELAY_MS / 2) + uniform_dist(rng) * (ENEMY_DELAY_MS / 2);
                registry.motions.get(new_enemy).scale = vec2(64.f, 64.f);
            }
        }
    }

    // display the enum number that corresponds to the current state of the game
    //    Game& game = registry.game.get(player_doll);
    //    std::cout << static_cast<int>(game.state) << std::endl;

    for (int i = 0; i < registry.animatedSprites.entities.size(); i++) {
        setSpriteAnimationPauseState(registry.animatedSprites.entities[i]);
        setSpriteAnimationFrame(elapsed_ms_since_last_update, registry.animatedSprites.entities[i]);
    }


    save();
    return true;
}

void WorldSystem::RotateAnimatronicJoints(float elapsedTime) const {
    elapsedTimeInRotationCycle += elapsedTime;
    float rotationCycleTime = 4000;
    map<LimbGroup, float> &limbRotation = registry.animatronicMovementInfo.get(torso).limbRotation;
    map<GoalPositionGroup, vec2> &goalPosition = registry.animatronicMovementInfo.get(torso).goalPosition;
    vec2 &leftHandDestination = goalPosition[GoalPositionGroup::LEFT_HAND];
    vec2 &rightHandDestination = goalPosition[GoalPositionGroup::RIGHT_HAND];
    vec2 &leftFootDestination = goalPosition[GoalPositionGroup::LEFT_FOOT];
    vec2 &rightFootDestination = goalPosition[GoalPositionGroup::RIGHT_FOOT];

    if (elapsedTimeInRotationCycle >= rotationCycleTime) {
        elapsedTimeInRotationCycle = 0;
        if (multiplicateFactor == 1) {
            setNextDestination(leftFootDestination, rightFootDestination, leftHandDestination, rightHandDestination);
            currentGoalPosition += leftHandDestination;
        }
        // multiplicate factor is used to create creepier movement instead of smooth movements:
        // it changes the rotation direction of certain limbs after each cycle
        multiplicateFactor *= -1;
    }
    // based on goal position, calculate the rotational angle of the limbs
    RotateLimbs(elapsedTime, rotationCycleTime, leftHandDestination, limbRotation, LimbGroup::LUA, LimbGroup::LLA);
    RotateLimbs(elapsedTime, rotationCycleTime, rightHandDestination, limbRotation, LimbGroup::RUA, LimbGroup::RLA);
    RotateLimbs(elapsedTime, rotationCycleTime, leftFootDestination, limbRotation, LimbGroup::LUL, LimbGroup::LLL);
    RotateLimbs(elapsedTime, rotationCycleTime, rightFootDestination, limbRotation, LimbGroup::RUL, LimbGroup::RLL);
}

void WorldSystem::RotateLimbs(float elapsedTime, float rotationCycleTime, const vec2 &Destination,
                              std::map<LimbGroup, float> &limbRotation,
                              LimbGroup upperLimbGroup, LimbGroup lowerLimbGroup) const {
    float limbLength = 1;
    float dsquared = Destination.x * Destination.x + Destination.y * Destination.y;
    float piMinusTwoLowerLimbRotation = acos((2 * limbLength * limbLength - dsquared) / (2 * limbLength * limbLength));
    float lowerLimbRotation = (M_PI - piMinusTwoLowerLimbRotation);
    float upperLimbRotation = atan(Destination.y / Destination.x) - asin(sin(lowerLimbRotation / sqrt(2)));
    if (upperLimbGroup == LimbGroup::LUA) {
        limbRotation[upperLimbGroup] += upperLimbRotation * elapsedTime / rotationCycleTime;
        limbRotation[lowerLimbGroup] += lowerLimbRotation * elapsedTime / rotationCycleTime * multiplicateFactor;
    } else if (upperLimbGroup == LimbGroup::RUA) {
        limbRotation[upperLimbGroup] -= upperLimbRotation * elapsedTime / rotationCycleTime;
        limbRotation[lowerLimbGroup] += lowerLimbRotation * elapsedTime / rotationCycleTime * multiplicateFactor;
    } else if (upperLimbGroup == LimbGroup::RUL) {
            limbRotation[upperLimbGroup] -= abs(upperLimbRotation * elapsedTime / rotationCycleTime) * multiplicateFactor / 4;
            limbRotation[lowerLimbGroup] += lowerLimbRotation * elapsedTime / rotationCycleTime * multiplicateFactor / 3;
    }
    else {
        limbRotation[upperLimbGroup] += abs(upperLimbRotation * elapsedTime / rotationCycleTime) * multiplicateFactor / 4;
        limbRotation[lowerLimbGroup] += lowerLimbRotation * elapsedTime / rotationCycleTime * multiplicateFactor / 3;
    }
}

void WorldSystem::setNextDestination(vec2 &leftFootDestination, vec2 &rightFootDestination, vec2 &leftHandDestination,
                                     vec2 &rightHandDestination) const {
    default_random_engine rng = default_random_engine(random_device()());
    uniform_real_distribution<float> uniform_dist;
    findRandomDestination(leftHandDestination);
    findRandomDestination(rightHandDestination);
    findRandomDestination(leftFootDestination);
    findRandomDestination(rightFootDestination);
}

void WorldSystem::findRandomDestination(vec2 &leftHandDestination) const {
    default_random_engine rng = default_random_engine(random_device()());
    uniform_real_distribution<float> uniform_dist;
    float randX = (uniform_dist(rng)) * 2 - 1;
    float randY = (uniform_dist(rng)) * 2 - 1;
    leftHandDestination = vec2{randX, randY};
}

void WorldSystem::progressTutorial(float elapsed_ms_since_last_update) {
    Background &bg_motion = registry.backgrounds.get(background);
    RenderRequest &rr = registry.renderRequests.get(tutorialScreen);
    TutorialTimer &tutorialTimer = registry.tutorialTimer.get(tutorialScreen);
    float &time = tutorialTimer.timePerSprite;
    int &index = tutorialTimer.tutorialIndex;
    bool &isComplete = tutorialTimer.tutorialCompleted;
    time -= elapsed_ms_since_last_update;

    if (time <= 0) {
        int maxIndex = 0;
        switch(gameProgress.level) {
            case 1: maxIndex = atticTutorial.size();
            break;
            case 10: maxIndex = hallway.size();
            break;
            case 11: maxIndex = hallwayAgain.size();
            break;
            default:
                break;
        }
        if (isDoorTutorial || isPreBossTutorial) {
            return;
        }
        if (TUTORIAL_INDEX == maxIndex - 1) {
            TUTORIAL_INDEX = 0;
            escapeTutorial();
        } else {
            switch(gameProgress.level) {
                case 1: rr.used_texture = atticTutorial[TUTORIAL_INDEX];
                break;
                case 10: rr.used_texture = hallway[TUTORIAL_INDEX];
                break;
                case 11: rr.used_texture = hallwayAgain[TUTORIAL_INDEX];
                break;
            }
            TUTORIAL_INDEX++;
        }
        time = 4000.f;
    }
}

void WorldSystem::findInitialFrameBufferSize() {
    if (fbWidth == 0 && fbHeight == 0) {
        int w, h;
        glfwGetFramebufferSize(window, &w, &h);
        fbWidth = w;
        fbHeight = h;
    }
}

void WorldSystem::escapeTutorial() {
//    isComplete = true;
    registry.remove_all_components_of(tutorialScreen);
    Game &game = registry.game.get(player_doll);
    game.state = GameState::PLAYING;
    Background &bg_motion = registry.backgrounds.get(background);
    bg_motion.blur_state = 0;
    TUTORIAL_INDEX = 0;
}

void WorldSystem::progressDoorLockWarningTutorial(){
    if (gameProgress.level == 1) {
        if (TUTORIAL_INDEX == 0) {
            registry.motions.get(tutorialScreen).position = {600, 400};
            registry.backgrounds.get(background).blur_state = 1;
            registry.renderRequests.get(tutorialScreen).used_texture = TEXTURE_ASSET_ID::TUTORIAL_ATTIC_LOCK_WARNING;
        } else {
            escapeTutorial();
        }
    } else {
        if (TUTORIAL_INDEX == 0) {
            registry.motions.get(tutorialScreen).position = {600, 400};
            registry.backgrounds.get(background).blur_state = 1;
            registry.renderRequests.get(tutorialScreen).used_texture = TEXTURE_ASSET_ID::TUTORIAL_DOOR_LOCK_WARNING;
        } else {
            escapeTutorial();
        }
    }
};

void WorldSystem::progressPreBossTutorial(){{
        isPreBossTutorial = false;
        escapeTutorial();
    }
};

//void WorldSystem::escapeDialogue(Entity e) {
//    registry.remove_all_components_of(e);
//    Game &game = registry.game.get(player_doll);
//    game.state = GameState::PLAYING;
//}

// Reset the world state to its initial state
void WorldSystem::restart_game(GameStateChange gc) {
    // Debugging for memory/component leaks
    enemiesKilled = 0;
    TUTORIAL_INDEX = 0;
    doorHasNotBeenInteracted = true;
    isDoorTutorial = false;
    isPreBossTutorial = false;
    // if (gc == GameStateChange::NEXT && gameProgress.level + 1 > MAX_LEVEL_COUNT) {
    //     gc = GameStateChange::RESET;
    // }
    registry.list_all_components();
    JsonReader jr;
    if (gc == GameStateChange::RESET) {
        printf("Restarting\n");
        GameProgressFactory gpf;
        json gp = jr.readFile("/gameData/default.json");
        gameProgress = gpf.create(renderer, gp);
        
        // restart music
        Mix_PlayMusic(main_theme_music, -1);
    } else if (gc == GameStateChange::NEXT) {
//        TUTORIAL_INDEX = 0;
//        doorHasNotBeenInteracted = true;
//        isDoorTutorial = false;
        printf("Moving to next level: ");
        std::string msg = std::to_string(gameProgress.level);
        msg += " -> ";
        msg += std::to_string(gameProgress.level + 1);
        std::cout << msg << std::endl;        
        if (gameProgress.level == 99) { // title screen is level 99 dont ask why
            gameProgress.level = 1;
        } else if (gameProgress.level == 11) {
            gameProgress.level = 101;
        } else if (gameProgress.level == 102) {
            gameProgress.level = 98;
        } else {
            gameProgress.level += 1;
        }
        save();
    } else if (gc == GameStateChange::GAMEOVER) {
        gameProgress.level = 100;
    } else {
        GameProgressFactory gpf;
        json gp = jr.readGameProgress();
        if (gp.empty()) {
            reset();
            return;
        }
        gameProgress = gpf.create(renderer, gp);
        if (gameProgress.level == 100) { // This is repeated code from above, probably some way to refactor this
            GameProgressFactory gpf;
            json gp = jr.readFile("/gameData/default.json");
            gameProgress = gpf.create(renderer, gp);
        }
        printf("Reloading from level: %i\n", gameProgress.level);
    }

    json level = jr.readLevel(gameProgress.level);

    findInitialFrameBufferSize();

    // Reset the game speed
    current_speed = 1.f;

    // Remove all entities that we created
    // All that have a motion, we could also iterate over all fish, turtles, ... but that would be more cumbersome
    while (registry.motions.entities.size() > 0)
        registry.remove_all_components_of(registry.motions.entities.back());

    // Debugging for memory/component leaks
    registry.list_all_components();
    debugging.in_debug_mode = false;

    vector<uint64_t> parallelJobs;

    // create a background
    parallelJobs.push_back(multithreading.addJob([&] {
        int bg = (level.contains("background")) ? level["background"].get<int>() : 12;
        background = createBackground(renderer, {600, 400}, (TEXTURE_ASSET_ID) bg);
    }));


    parallelJobs.push_back(multithreading.addJob([&] {
        if (level.contains("beds")) {
            BedFactory bedFactory;
            json beds = level["beds"];
            for (auto &bed : beds) {
                bedFactory.create(renderer, bed);
            }
        }
    }));

    parallelJobs.push_back(multithreading.addJob([&] {
        if (level.contains("lamps")) {
            LampFactory lampFactory;
            json lamps = level["lamps"];
            for (auto &lamp : lamps) {
                lampFactory.create(renderer, lamps);
            }
        }
    }));

    parallelJobs.push_back(multithreading.addJob([&] {
        if (level.contains("tables")) {
            TableFactory tableFactory;
            json tables = level["tables"];
            for (auto &table : tables) {
                tableFactory.create(renderer, table);
            }
        }
    }));

    parallelJobs.push_back(multithreading.addJob([&] {
        if (level.contains("doors")) {
            DoorFactory doorFactory;
            json doors = level["doors"];
            for (auto &door : doors) {
                doorFactory.create(renderer, door);
            }
        }
    }));

    parallelJobs.push_back(multithreading.addJob([&] {
        if (level.contains("healthPotions")) {
            HealthPotionFactory healthPotionFactory;
            json healthPotions = level["healthPotions"];
            for (auto &healthPotion : healthPotions) {
                // Check if we have already collected this item
                int item_id = healthPotion["world_id"].get<int>();
                if (std::find(gameProgress.all_collected_item_ids.begin(), gameProgress.all_collected_item_ids.end(),
                              item_id) != gameProgress.all_collected_item_ids.end()) {
                    // Do nothing
                } else {
                    healthPotionFactory.create(renderer, healthPotion);
                }
            }
        }
    }));

    parallelJobs.push_back(multithreading.addJob([&] {
        if (level.contains("mementos")) {
            MementoFactory mementoFactory;
            json mementos = level["mementos"];
            for (auto &memento : mementos) {
                int memento_id = memento["id"].get<int>();
                if (std::find(gameProgress.collected_memento_ids.begin(), gameProgress.collected_memento_ids.end(),
                              memento_id) == gameProgress.collected_memento_ids.end()) {
                    mementoFactory.create(renderer, memento);
                }
            }
        }
    }));

    parallelJobs.push_back(multithreading.addJob([&] {
        if (level.contains("enemies")) {
            EnemyFactory enemyFactory;
            json enemyData = level["enemies"];
            enemyFactory.create(renderer, this, enemyData);
        }
    }));

    parallelJobs.push_back(multithreading.addJob([&] {
        if (level.contains("walls")) {
            WallFactory wallFactory;
            json wallData = level["walls"];
            for (auto &wall : wallData) {
                wallFactory.create(renderer, wall);
            }
        }
    }));

    for(auto i: parallelJobs) {
        multithreading.join(i);
    }

    // doll creation
    PlayerFactory pf;
    if (gameProgress.level == 10 || gameProgress.level == 11) {
        for (int i = 0; i < 5; i++) {
            shadows[i] = createShadow(renderer, {0.0, 0.0}, i);
        }
    }
    player_doll = createDoll(renderer, gameProgress.motion.position, {fbWidth, fbHeight}, gameProgress.attacks,
                             gameProgress.magic, gameProgress.inventory_items, gameProgress.health.health,

                             gameProgress.health.healthDecrement, gameProgress.health.maxHP,
                             gameProgress.mana.mana, gameProgress.mana.maxMana);
    if (gameProgress.level == 10) {
        createFlora(renderer, { 500, 500 });
    }
    
    if (gameProgress.level == 11) {
         createAnimatronics(false);
    }

    prevDir = registry.motions.get(player_doll).dir;
    if (level.contains("player_doll")) {
        MotionFactory motionFactory;
        Motion copy = motionFactory.create(level["player_doll"]);
        Motion &m = registry.motions.get(player_doll);
        m.position = copy.position;
        m.dir = copy.dir;
        m.angle = copy.angle;

        // saving previous position of the doll
        playerPrevPosition = m.position;
//        ScrollableBackground& scrollableBackground = registry.scrollableBackground.get(background);
//        scrollableBackground.playerRelativePos = m.position;
    }


    // Create a new doll
    Motion &motion = registry.motions.get(player_doll);
    player_speed = 200.f;

    if (gc == GameStateChange::LOAD) {
        motion = gameProgress.motion;
        motion.velocity = {0, 0};
        registry.health.get(player_doll) = gameProgress.health;
        registry.mana.get(player_doll) = gameProgress.mana;
        registry.attackLists.get(player_doll).available_attacks = gameProgress.attacks;
        registry.inventories.get(player_doll).items = gameProgress.inventory_items;
        registry.inventories.get(player_doll).all_collected_item_ids = gameProgress.all_collected_item_ids;
        registry.mementoInventories.get(player_doll).collected_memento_ids = gameProgress.collected_memento_ids;
    } else if (gc == GameStateChange::NEXT) {
        registry.health.get(player_doll) = gameProgress.health;
        registry.mana.get(player_doll) = gameProgress.mana;
        registry.attackLists.get(player_doll).available_attacks = gameProgress.attacks;
        registry.inventories.get(player_doll).items = gameProgress.inventory_items;
        registry.inventories.get(player_doll).all_collected_item_ids = gameProgress.all_collected_item_ids;
        registry.mementoInventories.get(player_doll).collected_memento_ids = gameProgress.collected_memento_ids;
    }

    registry.colors.insert(player_doll, {1, 0.8f, 0.8f});

    // create a help screen
    if (gameProgress.level == 1) {
        helpScreen = createHelpWindow(renderer, {600, 130});
        HELPSCREEN_INDEX = 0;
    }
    // create a clickable menu button
    if (gameProgress.level < 98) {
        menuButton = createMenuButton(renderer, { 1150, 25 });
        Motion& menuButton_motion = registry.motions.get(menuButton);
        menuButton_motion.scale = menuButton_motion.scale * float(75);
    }

    // create a menu overlay
    menuOverlay = createMenuOverlay(renderer, {600, 400});
    Motion &overlayMotion = registry.motions.get(menuOverlay);
    overlayMotion.scale = overlayMotion.scale * float(300);

    // create tutorial screen at start of level
    if (gameProgress.level == 1 || gameProgress.level == 10 || gameProgress.level == 11) {
        tutorialScreen = createTutorial(renderer, {600, 600});

        switch(gameProgress.level) {
            case 1:
                registry.renderRequests.get(tutorialScreen).used_texture = atticTutorial[0];
                break;
            case 10:
                registry.renderRequests.get(tutorialScreen).used_texture = hallway[0];
                break;
            case 11:
                registry.renderRequests.get(tutorialScreen).used_texture = hallwayAgain[0];
            break;
        }
        // set state to tutorial
        if (!registry.tutorialTimer.get(tutorialScreen).tutorialCompleted) {
            Game &game = registry.game.get(player_doll);
            game.state = GameState::TUTORIAL;
        }
    }

    // examples of how to render team members
    if (gameProgress.level == 102) {
        Entity rosaline = createTeamMember(renderer, { 200, 380 }, "ROSALINE");
        Entity hannah = createTeamMember(renderer, { 600, 380 }, "HANNAH");
        Entity preet = createTeamMember(renderer, { 1000, 380 }, "PREET");
        Entity peter = createTeamMember(renderer, { 200, 650 }, "PETER");
        Entity liam = createTeamMember(renderer, { 600, 650 }, "LIAM");
        Entity naoreen = createTeamMember(renderer, { 1000, 650 }, "NAOREEN");
    }

    save();

}

Entity WorldSystem::createAnimatronics(bool battleScreen) {
    if (battleScreen) {
        left_leg_1 = createAnimatronic(renderer, TEXTURE_ASSET_ID::ANIMATRONIC_LEG_1, { 785, 400 });
        left_leg_2 = createAnimatronic(renderer, TEXTURE_ASSET_ID::ANIMATRONIC_LEG_2, { 785, 470 }, true);
        right_leg_1 = createAnimatronic(renderer, TEXTURE_ASSET_ID::ANIMATRONIC_LEG_1, { 815, 400 });
        right_leg_2 = createAnimatronic(renderer, TEXTURE_ASSET_ID::ANIMATRONIC_LEG_2, { 815, 470 }, true);
        torso = createAnimatronic(renderer, TEXTURE_ASSET_ID::ANIMATRONIC_TORSO, { 800, 300 });
        left_arm_1 = createAnimatronic(renderer, TEXTURE_ASSET_ID::ANIMATRONIC_ARM_1, { 770, 330 });
        left_arm_2 = createAnimatronic(renderer, TEXTURE_ASSET_ID::ANIMATRONIC_ARM_2, { 770, 380 });
        right_arm_1 = createAnimatronic(renderer, TEXTURE_ASSET_ID::ANIMATRONIC_ARM_1, { 830, 330 });
        right_arm_2 = createAnimatronic(renderer, TEXTURE_ASSET_ID::ANIMATRONIC_ARM_2, { 830, 380 });

        // Not sure what the positions in the below do... - LC
        registry.animatronics.insert(left_leg_1, { GoalPositionGroup::LEFT_FOOT, LimbGroup::LUL, {520, 530} });
        registry.animatronics.insert(left_leg_2, { GoalPositionGroup::LEFT_FOOT, LimbGroup::LLL, {520, 620} });
        registry.animatronics.insert(right_leg_1, { GoalPositionGroup::RIGHT_FOOT, LimbGroup::RUL, {550, 530} });
        registry.animatronics.insert(right_leg_2, { GoalPositionGroup::RIGHT_FOOT, LimbGroup::RLL, {550, 620} });
        registry.animatronics.insert(right_arm_1, { GoalPositionGroup::RIGHT_HAND, LimbGroup::RUA,{500, 440} });
        registry.animatronics.insert(right_arm_2, { GoalPositionGroup::RIGHT_HAND, LimbGroup::RLA, {500, 515} });
        registry.animatronics.insert(left_arm_1, { GoalPositionGroup::LEFT_HAND, LimbGroup::LUA, {435, 440} });
        registry.animatronics.insert(left_arm_2, { GoalPositionGroup::LEFT_HAND, LimbGroup::LLA, {435, 515} });

        registry.battleEnemies.emplace(left_leg_1);
        registry.battleEnemies.emplace(left_leg_2);
        registry.battleEnemies.emplace(right_leg_1);
        registry.battleEnemies.emplace(right_leg_2);
        registry.battleEnemies.emplace(torso);
        registry.battleEnemies.emplace(left_arm_1);
        registry.battleEnemies.emplace(left_arm_2);
        registry.battleEnemies.emplace(right_arm_1);
        registry.battleEnemies.emplace(right_arm_2);

        for (Entity part : registry.animatronics.entities) {
            Motion& motion = registry.motions.get(part);
            motion.scale *= 0.8;
        }

        Motion& torsoMotion = registry.motions.get(torso);
        torsoMotion.scale *= 0.8;
    }
    else {
        left_leg_1 = createAnimatronic(renderer, TEXTURE_ASSET_ID::ANIMATRONIC_LEG_1, { 580, 530 });
        left_leg_2 = createAnimatronic(renderer, TEXTURE_ASSET_ID::ANIMATRONIC_LEG_2, { 580, 620 }, true);
        right_leg_1 = createAnimatronic(renderer, TEXTURE_ASSET_ID::ANIMATRONIC_LEG_1, { 620, 530 });
        right_leg_2 = createAnimatronic(renderer, TEXTURE_ASSET_ID::ANIMATRONIC_LEG_2, { 620, 620 }, true);
        torso = createAnimatronic(renderer, TEXTURE_ASSET_ID::ANIMATRONIC_TORSO, { 600, 400 });
        left_arm_1 = createAnimatronic(renderer, TEXTURE_ASSET_ID::ANIMATRONIC_ARM_1, { 560, 440 });
        left_arm_2 = createAnimatronic(renderer, TEXTURE_ASSET_ID::ANIMATRONIC_ARM_2, { 560, 515 });
        right_arm_1 = createAnimatronic(renderer, TEXTURE_ASSET_ID::ANIMATRONIC_ARM_1, { 640, 440 });
        right_arm_2 = createAnimatronic(renderer, TEXTURE_ASSET_ID::ANIMATRONIC_ARM_2, { 640, 515 });

        registry.animatronics.insert(left_leg_1, { GoalPositionGroup::LEFT_FOOT, LimbGroup::LUL, {580, 530} });
        registry.animatronics.insert(left_leg_2, { GoalPositionGroup::LEFT_FOOT, LimbGroup::LLL, {580, 620} });
        registry.animatronics.insert(right_leg_1, { GoalPositionGroup::RIGHT_FOOT, LimbGroup::RUL, {620, 530} });
        registry.animatronics.insert(right_leg_2, { GoalPositionGroup::RIGHT_FOOT, LimbGroup::RLL, {620, 620} });
        registry.animatronics.insert(right_arm_1, { GoalPositionGroup::RIGHT_HAND, LimbGroup::RUA,{640, 440} });
        registry.animatronics.insert(right_arm_2, { GoalPositionGroup::RIGHT_HAND, LimbGroup::RLA, {640, 515} });
        registry.animatronics.insert(left_arm_1, { GoalPositionGroup::LEFT_HAND, LimbGroup::LUA, {560, 440} });
        registry.animatronics.insert(left_arm_2, { GoalPositionGroup::LEFT_HAND, LimbGroup::LLA, {560, 515} });
    }

    registry.animatronicMovementInfo.emplace(torso);
    map<GoalPositionGroup, vec2> goalPosition = {
            {GoalPositionGroup::RIGHT_HAND, {1,  1}},
            {GoalPositionGroup::LEFT_HAND,  {-1, -1}},
            {GoalPositionGroup::RIGHT_FOOT, {-1,  1}},
            {GoalPositionGroup::LEFT_FOOT,  {1,  -1}},
    };
    registry.animatronicMovementInfo.get(torso).goalPosition = goalPosition;
    return torso;
}

void WorldSystem::drawBattleWindow(EnemyName name) {
    selected_move_menu = BattleMenuItemType::NONE;
    battle_screen = createBattleWindow(renderer, {600.f, 400.f});
    generateParallaxWindows();
    battle_doll = createBattleDoll(renderer, {400, 400});
    if (name == EnemyName::DUST_BUNNY || name == EnemyName::TEDDY_BEAR || name == EnemyName::FLORA_1) {
        battle_enemy = createBattleEnemy(renderer, { 800, 370 }, name);
    }
    else {
        for (Entity part : registry.animatronics.entities) {
            registry.remove_all_components_of(part);
        }
        registry.remove_all_components_of(torso);

        createAnimatronics(true);
    }

    // create player healthbar
    if (registry.players.entities[0]) {
        healthbar = createHealthbar(renderer, {400.f, 200.f}, EnemyName::ENEMY_NAME_COUNT);
    }

    // create enemy healthbar
    if (registry.currentEnemies.entities[0]) {
        Entity curr = registry.currentEnemies.entities[0];
        healthbar = createHealthbar(renderer, {800.f, 200.f}, registry.enemies.get(curr).name);
    }

    const float SCALE = 800 / 160; // 160 px is the height of the Aseprite drawing of the battle screen

    // The numbers that the "POSITION" quantities are being multiplied by are the positions of the assets on the Aseprite drawing
    vec2 BUTTON_AREA_POSITION = {32 * SCALE, 132 * SCALE};
    Entity battle_menu_button_area = createBattleMenuItem(renderer, BUTTON_AREA_POSITION, BattleMenuItemType::BUTTON_AREA,
                                                   TEXTURE_ASSET_ID::BATTLE_MENU_BUTTON_AREA);
    scaleUIAsset(battle_menu_button_area, {60, 51}, SCALE);

    vec2 TEXT_AREA_POSITION = {151 * SCALE, 132 * SCALE};
    Entity battle_menu_text_area = createBattleMenuItem(renderer, TEXT_AREA_POSITION, BattleMenuItemType::TEXT_AREA,
                                                        TEXTURE_ASSET_ID::BATTLE_MENU_TEXT_AREA);
    scaleUIAsset(battle_menu_text_area, {173, 51}, SCALE);

    vec2 ATTACK_BUTTON_POSITION = {32 * SCALE, 132 * SCALE - 73};
    Entity battle_menu_button_attack = createBattleMenuItem(renderer, ATTACK_BUTTON_POSITION,
                                                            BattleMenuItemType::ATTACK_BUTTON,
                                                            TEXTURE_ASSET_ID::BATTLE_MENU_BUTTON_ATTACK);
    scaleUIAsset(battle_menu_button_attack, {52, 13}, SCALE);

    vec2 MAGIC_BUTTON_POSITION = {32 * SCALE, 132 * SCALE};
    Entity battle_menu_button_magic = createBattleMenuItem(renderer, MAGIC_BUTTON_POSITION,
                                                           BattleMenuItemType::MAGIC_BUTTON,
                                                           TEXTURE_ASSET_ID::BATTLE_MENU_BUTTON_MAGIC);
    scaleUIAsset(battle_menu_button_magic, {52, 13}, SCALE);

    vec2 ITEMS_BUTTON_POSITION = {32 * SCALE, 132 * SCALE + 73};
    Entity battle_menu_button_items = createBattleMenuItem(renderer, ITEMS_BUTTON_POSITION,
                                                           BattleMenuItemType::ITEMS_BUTTON,
                                                           TEXTURE_ASSET_ID::BATTLE_MENU_BUTTON_ITEMS);
    scaleUIAsset(battle_menu_button_items, {52, 13}, 5);
    // Commented these out for now - implementation of the buttons below is a TODO
    //vec2 LEARN_BUTTON_POSITION = { 224 * SCALE, 132 * SCALE };
    //battle_menu_button_learn = createBattleMenuItem(renderer, LEARN_BUTTON_POSITION, BattleMenuItemType::LEARN_BUTTON, TEXTURE_ASSET_ID::BATTLE_MENU_BUTTON_LEARN);
    //scaleUIAsset(battle_menu_button_learn, { 18, 13 }, 5);

    //vec2 GO_BUTTON_POSITION = { 224 * SCALE, 132 * SCALE + 73 };
    //battle_menu_button_go = createBattleMenuItem(renderer, GO_BUTTON_POSITION, BattleMenuItemType::GO_BUTTON, TEXTURE_ASSET_ID::BATTLE_MENU_BUTTON_GO);
    //scaleUIAsset(battle_menu_button_go, { 18, 13 }, SCALE);

    // pause the current music
    Mix_PauseMusic();
    // play the battle music on channel 5
    Mix_PlayChannel(5, battle_music, -1);
}

void WorldSystem::generateParallaxWindows() {
    parallax_background = createBattleParallaxWindow(renderer, {600.f, 400.f}, TEXTURE_ASSET_ID::PARALLAX_BACKGROUND);
    parallax_layer_0 = createBattleParallaxWindow(renderer, {600.f, 400.f}, TEXTURE_ASSET_ID::PARALLAX_LAYER0);
    second_parallax_layer_0 = createBattleParallaxWindow(renderer, {-600.f, 400.f}, TEXTURE_ASSET_ID::PARALLAX_LAYER0);
    parallax_layer_1 = createBattleParallaxWindow(renderer, {600.f, 400.f}, TEXTURE_ASSET_ID::PARALLAX_LAYER1);
    second_parallax_layer_1 = createBattleParallaxWindow(renderer, {-600.f, 400.f}, TEXTURE_ASSET_ID::PARALLAX_LAYER1);
    parallax_layer_2 = createBattleParallaxWindow(renderer, {600.f, 400.f}, TEXTURE_ASSET_ID::PARALLAX_LAYER2);
    second_parallax_layer_2 = createBattleParallaxWindow(renderer, {-600.f, 400.f}, TEXTURE_ASSET_ID::PARALLAX_LAYER2);
}


void WorldSystem::scaleUIAsset(Entity entity, vec2 originalDimensions, float scaleFactor) {
    assert(registry.motions.has(entity));
    Motion &motion = registry.motions.get(entity);
    motion.scale.x = originalDimensions[0] * scaleFactor;
    motion.scale.y = originalDimensions[1] * scaleFactor;
}

void WorldSystem::destroyBattleWindow() {
    clearBattlePlayerMoveButtons();
    registry.remove_all_components_of(battle_screen);
    registry.remove_all_components_of(battle_doll);
    registry.remove_all_components_of(battle_enemy);
    registry.remove_all_components_of(parallax_background);
    registry.remove_all_components_of(parallax_layer_0);
    registry.remove_all_components_of(parallax_layer_1);
    registry.remove_all_components_of(parallax_layer_2);
    registry.remove_all_components_of(second_parallax_layer_0);
    registry.remove_all_components_of(second_parallax_layer_1);
    registry.remove_all_components_of(second_parallax_layer_2);

    for (int i = registry.battleMenus.size() - 1; i >= 0; i--) {
        registry.remove_all_components_of(registry.battleMenus.entities[i]);
    }

    for (int i = registry.battleMenuButtons.size() - 1; i >= 0; i--) {
        registry.remove_all_components_of(registry.battleMenuButtons.entities[i]);
    }

    int numHealthbarItems = registry.healthbars.entities.size();
    for (int i = numHealthbarItems - 1; i >= 0; i--) {
        registry.remove_all_components_of(registry.healthbars.entities[i]);
    }

    // stop battle music
    Mix_HaltChannel(5);

    // switch music back to main theme
    Mix_ResumeMusic();
}

// Compute collisions between entities
void WorldSystem::handle_collisions() {

    // Loop over all collisions detected by the physics system
    auto &collisionsRegistry = registry.collisions;
    for (uint i = 0; i < collisionsRegistry.components.size(); i++) {
        // The entity and its collider
        Entity entity = collisionsRegistry.entities[i];
        Entity entity_other = collisionsRegistry.components[i].other;

        // For now, we are only interested in collisions that involve the player doll
        if (registry.players.has(entity)) {
            Player &player = registry.players.get(entity);
            if (registry.enemies.has(entity_other) && !registry.bosses.has(entity_other)) {
                Game &game = registry.game.get(player_doll);
                game.state = GameState::BATTLE;

                // All other chasers will now lose interest upon battling
                auto &behaviourRegistry = registry.behaviours;
                for (uint i = 0; i < behaviourRegistry.components.size(); i++) {
                    Entity &enemy = behaviourRegistry.entities[i];
                    behaviourRegistry.components[i] = Behaviour();
                    if (registry.roaming.has(enemy)) {
                        registry.roaming.remove(enemy);
                    }
                }

                if (!registry.currentEnemies.has(entity_other)) {
                    registry.currentEnemies.emplace(entity_other);

                    drawBattleWindow(registry.enemies.get(entity_other).name);
                }
            } else if (registry.doors.has(entity_other)) {
                if (!(gameProgress.level == 1 && enemiesKilled < MAX_ENEMY_COUNT) && hasMementoForCurrentLevel(player_doll, gameProgress.level)) {
                    // registry.game.get(player_doll).state = GameState::PLAYING;
                    registry.game.get(player_doll).state = GameState::TRANSITION;
                    registry.game.get(player_doll).timer = transitionDelay;
                    registry.collisions.clear();
                    handleTransition();
                    // restart_game(GameStateChange::NEXT);// TODO: follow other todo that talks about handling delay
                    return;
                } else if (doorHasNotBeenInteracted && gameProgress.level < 10) {
                    doorHasNotBeenInteracted = false;
                    tutorialScreen = createDoorLockTutorial(renderer);
                    Game &game = registry.game.get(player_doll);
                    isDoorTutorial = true;
                    TUTORIAL_INDEX = 0;
                    game.state = GameState::TUTORIAL;
                }
            }
            if (registry.gameItems.has(entity_other) && registry.game.get(entity).state == GameState::PLAYING) {
                // Add item to inventory
                Inventory &player_inventory = registry.inventories.get(player_doll);

                if (player_inventory.items.size() < player_inventory.MAX_ITEMS) {
                    GameItem item = registry.gameItems.get(entity_other);
                    player_inventory.addItem(item);

                    // Destroy item from world
                    registry.remove_all_components_of(entity_other);

                    // Play sound effect
                    Mix_PlayChannel(-1, get_item_sound, 0);
                } else {
                    // Block player from walking into potion
                    PhysicsSystem physicsSystem;
                    physicsSystem.handleWallCollision(entity, entity_other);
                }
            }

            if (registry.mementos.has(entity_other) && registry.game.get(entity).state == GameState::PLAYING) {
                MementoName &memento = registry.mementos.get(entity_other);
                Game &game = registry.game.get(player_doll);
                // Add item to inventory
                registry.mementoInventories.get(player_doll).addItem(memento);

                // Destroy item from world
                registry.remove_all_components_of(entity_other);

                // Show message
                createMementoText(renderer, { 600, 400 }, memento);
                game.state = GameState::POPUP;
            }

            if ((registry.bosses.has(entity_other) || registry.floras.has(entity_other))&& registry.game.get(entity).state == GameState::PLAYING) {
                Game& game = registry.game.get(player_doll);
                if (registry.floras.has(entity_other)) {
                    createBoss(renderer, 1);
                }
                else {
                    createBoss(renderer, 2);
                }
                game.state = GameState::GOINGTOBOSS;
            }
        }
    }
    // Remove all collisions from this simulation step
    registry.collisions.clear();
}

// Should the game be over ?
bool WorldSystem::is_over() const {
    return bool(glfwWindowShouldClose(window));
}

// On key callback
void WorldSystem::on_key(int key, int, int action, int mod) {
    // press spacebar to close tutorial screen
    if (action == GLFW_PRESS && key == GLFW_KEY_R) {
        reset();
        return;
    }
    Game &game = registry.game.get(player_doll);
    // press slash to activate debugging mode, release to deactivate
    if (game.state == GameState::PLAYING && key == GLFW_KEY_SLASH) {
        if (action == GLFW_PRESS) {
            debugging.in_debug_mode = !debugging.in_debug_mode;
        }
    }

    if (gameProgress.level == 100 && key == GLFW_KEY_Y) {
        restart_game(GameStateChange::RESET);
    }

    if ((gameProgress.level == 101 || gameProgress.level == 102) && key == GLFW_KEY_ENTER && action == GLFW_PRESS) {
        restart_game(GameStateChange::NEXT);
    }

    else if (gameProgress.level == 100 && key == GLFW_KEY_N) {
        glfwSetWindowShouldClose(window, 1);
    }

    // press spacebar to close tutorial screen
    if (game.state == GameState::TUTORIAL && key == GLFW_KEY_SPACE && action == GLFW_PRESS) {
        assert(registry.tutorialTimer.has(tutorialScreen));
        TutorialTimer &tutorialTimer = registry.tutorialTimer.get(tutorialScreen);
        escapeTutorial();
        game.state = GameState::PLAYING;
    }
    // press return key to progress tutorial faster
    if (game.state == GameState::TUTORIAL && key == GLFW_KEY_ENTER && action == GLFW_PRESS) {
        assert(registry.tutorialTimer.has(tutorialScreen));
        RenderRequest &rr = registry.renderRequests.get(tutorialScreen);
        TutorialTimer &tutorialTimer = registry.tutorialTimer.get(tutorialScreen);
        float &time = tutorialTimer.timePerSprite;
        int &index = tutorialTimer.tutorialIndex;
        bool &isComplete = tutorialTimer.tutorialCompleted;
        int maxIndex = 0;
        switch(gameProgress.level) {
            case 1: maxIndex = 5;
            break;
            case 10: maxIndex = hallway.size();
            break;
            case 11:
                if (isPreBossTutorial) {
                    maxIndex = preBossBattle.size();
                } else {
                    maxIndex = hallwayAgain.size();
                }
            break;
            default:
                break;
        }
        if (TUTORIAL_INDEX == maxIndex - 1) {
            TUTORIAL_INDEX = 0;
            escapeTutorial();
        } else {
            time = 3000.f;
            switch(gameProgress.level) {
                case 1:
                    if (isDoorTutorial) {
                        progressDoorLockWarningTutorial();
                    } else {
                        rr.used_texture = atticTutorial[TUTORIAL_INDEX];
                    }
                break;
                case 10: rr.used_texture = hallway[TUTORIAL_INDEX];
                break;
                case 11:
                    if (isPreBossTutorial) {
                        rr.used_texture = preBossBattle[TUTORIAL_INDEX];
                    } else {
                        rr.used_texture = hallwayAgain[TUTORIAL_INDEX];
                    }
                break;
                default:
                    progressDoorLockWarningTutorial();
                    break;
            }
            TUTORIAL_INDEX++;
        }
    }

    if (game.state == GameState::POPUP && key == GLFW_KEY_ENTER && action == GLFW_PRESS) {
        Entity& popup = registry.popups.entities[0];
        registry.remove_all_components_of(popup);
        registry.game.get(player_doll).state = GameState::PLAYING;
    }

    const float finalHelpIndex = helpScreenOptions.size();

    // change the helpscreen display, n to move left and m to move right
    if (registry.renderRequests.has(helpScreen)) {
        RenderRequest &hs_rr = registry.renderRequests.get(helpScreen);
        if (key == GLFW_KEY_M && action == GLFW_PRESS && HELPSCREEN_INDEX > 0) {
            hs_rr.used_texture = helpScreenOptions[HELPSCREEN_INDEX - 1];
            HELPSCREEN_INDEX--;
        } else if (key == GLFW_KEY_N && action == GLFW_PRESS && HELPSCREEN_INDEX < finalHelpIndex - 1) {
            hs_rr.used_texture = helpScreenOptions[HELPSCREEN_INDEX + 1];
            HELPSCREEN_INDEX++;
        }
    }

    // Dialogue for room 1
//    if (game.state != GameState::TUTORIAL && registry.popups.has(room1Dialogue) && gameProgress.level == 1) {
//        PopUp &dialogue = registry.popups.get(room1Dialogue);
//        RenderRequest &dialogue_rr = registry.renderRequests.get(room1Dialogue);
//        if ((key == GLFW_KEY_ENTER || key == GLFW_KEY_SPACE) && action == GLFW_PRESS) {
//            if (dialogue.order < room1Popups.size()) {
//                dialogue_rr.used_texture = room1Popups[dialogue.order];
//                dialogue.order++;
//            } else if (dialogue.order == (room1Popups.size())) {
//                escapeDialogue(room1Dialogue);
//            }
//        }
//    }

    // Resetting game
    if (action == GLFW_RELEASE && key == GLFW_KEY_R) {
        int w, h;
        glfwGetWindowSize(window, &w, &h);

        restart_game();
    }
    //
    auto gamestate = registry.game.get(player_doll).state;
    if (action == GLFW_PRESS && key == GLFW_KEY_ESCAPE) {
        gamestate = GameState::PLAYING;
    }
    /*
    *
    // Temporary: Testing for blur/background
    Background &bg_motion = registry.backgrounds.get(background);
    if (action == GLFW_PRESS) {
        // Blur
        if (key == GLFW_KEY_P) {
            bg_motion.blur_state = 1;
        }
            // Regular
        else if (key == GLFW_KEY_O) {
            bg_motion.blur_state = 0;
        }
            // Tweaking
        else if (key == GLFW_KEY_I) {
            bg_motion.blur_state = 2;
        }
    }


    */


    Motion &motion = registry.motions.get(player_doll);
    if (action == GLFW_PRESS) {
        if (key == GLFW_KEY_W) {
            motion.velocity = vec2{0, -player_speed};
            motion.dir = Direction::UP;
        } else if (key == GLFW_KEY_S) {
            motion.velocity = vec2{0, +player_speed};
            motion.dir = Direction::DOWN;
        } else if (key == GLFW_KEY_A) {
            motion.velocity = vec2{-player_speed, 0};
            motion.dir = Direction::LEFT;
        } else if (key == GLFW_KEY_D) {
            motion.velocity = vec2{+player_speed, 0};
            motion.dir = Direction::RIGHT;
        }
    } else if (action == GLFW_RELEASE) {
        if (key == GLFW_KEY_W) {
            motion.velocity = vec2{motion.velocity[0], 0};
        } else if (key == GLFW_KEY_S) {
            motion.velocity = vec2{motion.velocity[0], 0};
        } else if (key == GLFW_KEY_A) {
            motion.velocity = vec2{0, motion.velocity[1]};
        } else if (key == GLFW_KEY_D) {
            motion.velocity = vec2{0, motion.velocity[1]};
        }
    }
    if (prevDir != motion.dir) {
        this->setRenderRequests();
        prevDir = motion.dir;
    }

    // Control the current speed with `<` `>`
    if (action == GLFW_RELEASE && (mod & GLFW_MOD_SHIFT) && key == GLFW_KEY_COMMA) {
        current_speed -= 0.1f;
        printf("Current speed = %f\n", current_speed);
    }
    if (action == GLFW_RELEASE && (mod & GLFW_MOD_SHIFT) && key == GLFW_KEY_PERIOD) {
        current_speed += 0.1f;
        printf("Current speed = %f\n", current_speed);
    }
    current_speed = fmax(0.f, current_speed);

    // Increase or decrease the volume
    if (key == GLFW_KEY_UP && action == GLFW_PRESS) {
        // unpause if currently paused
        if (Mix_PausedMusic() && !inBattleOrBossState()) {
            Mix_ResumeMusic();
        }
        else {
            int currentVolume = Mix_VolumeMusic(-1); // -1 does not set the volume, but returns the current volume setting
            Mix_VolumeMusic(currentVolume + 20); // sets backround music volume
            Mix_Volume(-1, currentVolume + 20); // sets sound effects volume
        }
    }
    else if (key == GLFW_KEY_DOWN && action == GLFW_PRESS) {
        int currentVolume = Mix_VolumeMusic(-1);
        Mix_VolumeMusic(currentVolume - 20);
        Mix_Volume(-1, currentVolume - 20);

        // pause music
        if (currentVolume - 20 <= 0) {
            Mix_PauseMusic();
            Mix_Volume(-1, 1);
        }
    }
}

void WorldSystem::on_mouse_click(int button, int action, int mods) {
    double xPos, yPos;
    auto t = Clock::now();

    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
        glfwGetCursorPos(window, &xPos, &yPos);
        old_xPos = xPos;
        old_yPos = yPos;
        Game &game = registry.game.get(player_doll);
        // clickevent happening on menu box
        if (isClickInRegion(xPos, yPos, {1115, 10}, 40, 75)) {
            if (game.state == GameState::MENUOVERLAY) {
                Mix_PlayChannel(-1, menu_click_sound, 0);
                closeMenuOverlayScreen();
            } else if (game.state == GameState::PLAYING) {
                Mix_PlayChannel(-1, menu_click_sound, 0);
                openMenuOverlayScreen();
            }
        }
        if (game.state == GameState::MENUOVERLAY) {
            if (isClickInRegion(xPos, yPos, {460, 320}, 40, 300)) {
                Mix_PlayChannel(-1, menu_click_sound, 0);
                closeMenuOverlayScreen();
                restart_game();
            }
            // REPLAY_TUTORIAL
            // changed to exit game
            if (isClickInRegion(xPos, yPos, {460, 420}, 40, 300)) {
                Mix_PlayChannel(-1, menu_click_sound, 0);
                glfwSetWindowShouldClose(window, 1);

//                closeMenuOverlayScreen();
//                tutorialScreen = createTutorial(renderer, {600, 400});
//                Motion &tutorialMotion = registry.motions.get(tutorialScreen);
//                TutorialTimer &tutorial = registry.tutorialTimer.get(tutorialScreen);
//                tutorial.tutorialCompleted = false;
//                tutorial.tutorialIndex = 0;
//                Game &game = registry.game.get(player_doll);
//                game.state = GameState::TUTORIAL;
            }
        }
        if (game.state == GameState::BATTLE || game.state == GameState::BOSS) {
            detectAndHandleBattleScreenClick(xPos, yPos);
        }
    }
    // Speed up with mouse drag in direction of movement
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE) {
        glfwGetCursorPos(window, &xPos, &yPos);
        double x_diff = xPos - old_xPos;
        double y_diff = yPos - old_yPos;
        auto now = Clock::now();
        std::chrono::duration<double> elapsed_seconds = now - t;

        // arbitrary speed units (easier to look at)
        double speed_of_click_x = (abs(x_diff / 1000) / elapsed_seconds.count()) / 100;
        double speed_of_click_y = (abs(y_diff / 1000) / elapsed_seconds.count()) / 100;
        auto player_motion = registry.motions.get(player_doll).dir;

        // speed up, right
        if (x_diff > 0 && (-50 < y_diff) && (y_diff < 50) && player_motion == Direction::RIGHT) {
            if (speed_of_click_x > 700) {
                registry.motions.get(player_doll).velocity *= 3;
            } else {
                registry.motions.get(player_doll).velocity *= 2;
            }
        }
        // speed up, left
        if (x_diff < 0 && (-50 < y_diff) && (y_diff < 50) && player_motion == Direction::LEFT) {
            if (speed_of_click_x > 700) {
                registry.motions.get(player_doll).velocity *= 3;
            } else {
                registry.motions.get(player_doll).velocity *= 2;
            }
        }
        // speed up, down
        if (y_diff > 0 && (-50 < x_diff) && (x_diff < 50) && player_motion == Direction::DOWN) {
            if (speed_of_click_y > 700) {
                registry.motions.get(player_doll).velocity *= 3;
            } else {
                registry.motions.get(player_doll).velocity *= 2;
            }
        }
        // speed up, up
        if (y_diff < 0 && (-50 < x_diff) && (x_diff < 50) && player_motion == Direction::UP) {
            if (speed_of_click_y > 700) {
                registry.motions.get(player_doll).velocity *= 3;
            } else {
                registry.motions.get(player_doll).velocity *= 2;
            }
        }

    }
}

void WorldSystem::openMenuOverlayScreen() {
    Game &game = registry.game.get(player_doll);
    game.state = GameState::MENUOVERLAY;
    Background &bg_motion = registry.backgrounds.get(background);
    bg_motion.blur_state = 1;
    registry.renderRequests.insert(
            menuOverlay,
            {TEXTURE_ASSET_ID::MENU_OVERLAY,
             EFFECT_ASSET_ID::HELP_SCREEN,
             GEOMETRY_BUFFER_ID::SPRITE});
}

void WorldSystem::closeMenuOverlayScreen() {
    Background &bg_motion = registry.backgrounds.get(background);
    bg_motion.blur_state = 0;
    registry.renderRequests.remove(menuOverlay);
    Game &game = registry.game.get(player_doll);
    game.state = GameState::PLAYING;
}

void WorldSystem::attach(std::function<void(Entity, bool)> fn) {
    callbacks.push_back(fn);
}

void WorldSystem::on_mouse_move(vec2 mouse_position) {
    (vec2) mouse_position; // dummy to avoid compiler warning
}

void WorldSystem::setSpriteAnimationFrame(float elapsed_time_ms, Entity entity) {
    assert(registry.animatedSprites.has(entity));
    AnimatedSprite &animated_sprite = registry.animatedSprites.get(entity);
    animated_sprite.ms_since_last_update += elapsed_time_ms;
    if (animated_sprite.paused == true) {
        animated_sprite.current_frame = 0;
    } else if (animated_sprite.ms_since_last_update > animated_sprite.animation_speed_ms) {
        int num_frames = animated_sprite.num_frames_per_type[animated_sprite.current_type];
        animated_sprite.current_frame = (animated_sprite.current_frame + 1) % num_frames;
        animated_sprite.ms_since_last_update = 0;
    }
}

void WorldSystem::setSpriteAnimationPauseState(Entity entity) {
    assert(registry.animatedSprites.has(entity));
    AnimatedSprite &animated_sprite = registry.animatedSprites.get(entity);

    if (registry.game.get(player_doll).state == GameState::POPUP ||
        registry.game.get(player_doll).state == GameState::TUTORIAL) {
        animated_sprite.paused = true;
        return;
    }

    // pause/go logic for player
    if (registry.players.has(entity)) {
        Motion &player_motion = registry.motions.get(entity);
        animated_sprite.paused = player_motion.velocity == vec2{0, 0};
    }
}

void WorldSystem::setRenderRequests() {
    // TODO: This function name isn't really relevant anymore, maybe we can change it -Naoreen
    if (registry.game.get(player_doll).state == GameState::PLAYING
        || registry.game.get(player_doll).state == GameState::TUTORIAL) {
        Direction dir = registry.motions.get(player_doll).dir;
        AnimatedSprite &animated_sprite = registry.animatedSprites.get(player_doll);
        if (dir == Direction::UP) {
            animated_sprite.current_type = 2;
        } else if (dir == Direction::RIGHT) {
            animated_sprite.current_type = 1;
        } else if (dir == Direction::DOWN) {
            animated_sprite.current_type = 0;
        } else if (dir == Direction::LEFT) {
            animated_sprite.current_type = 3;
        }
    }
}

void WorldSystem::handleBattleScreenButtonClick(Entity button_clicked) {
    assert(inBattleOrBossState());
    if (registry.battleMenuButtons.has(button_clicked)) {
        Mix_PlayChannel(-1, menu_click_sound, 0);
        switchBattleMenu(button_clicked);
        clearBattleMoveUsed();
    } else if (
            registry.battleMenuPlayerMoves.has(button_clicked) &&
            registry.game.get(player_doll).battle_state == BattleState::PLAYER_TURN &&
            !registry.turns.has(player_doll)) {
        resetPlayerMoveDefaultTextures();
        // Make clicked button look selected (highlighted)
        BattleMenuPlayerMove &chosen_move = registry.battleMenuPlayerMoves.get(button_clicked);
        RenderRequest &rr = registry.renderRequests.get(button_clicked);
        rr.used_texture = chosen_move.selected_texture;

        // Register a Turn with the correct move name
        if (selected_move_menu == BattleMenuItemType::ATTACK_BUTTON) {
            registry.turns.insert(player_doll, Turn(chosen_move.attack_name));
        }
        if (selected_move_menu == BattleMenuItemType::MAGIC_BUTTON) {
            registry.turns.insert(player_doll, Turn(chosen_move.magic_name));
        }
        if (selected_move_menu == BattleMenuItemType::ITEMS_BUTTON) {
            registry.turns.insert(player_doll, Turn(chosen_move.item_name, chosen_move.item_id));
        }
    }
}


void WorldSystem::switchBattleMenu(Entity menu_button) {
    assert(inBattleOrBossState());
    assert(registry.battleMenuButtons.has(menu_button));
    BattleMenuItemType menu_button_type = registry.battleMenuButtons.get(menu_button).button_type;

    if (selected_move_menu == menu_button_type) {
        return;
    }

    selected_move_menu = menu_button_type;
    clearBattlePlayerMoveButtons();

    if (menu_button_type == BattleMenuItemType::ATTACK_BUTTON) {
        renderPlayerAttackOptions();
    } else if (menu_button_type == BattleMenuItemType::MAGIC_BUTTON) {
        renderPlayerMagicOptions();
    } else if (menu_button_type == BattleMenuItemType::ITEMS_BUTTON) {
        renderPlayerItemOptions();
    }
}

void WorldSystem::updateBattleScreenItemOptions() {
    clearBattlePlayerMoveButtons();
    renderPlayerItemOptions();
}

void WorldSystem::renderPlayerAttackOptions() {
    assert(inBattleOrBossState());
    assert(selected_move_menu == BattleMenuItemType::ATTACK_BUTTON);

    AttackList& player_attacks = registry.attackLists.get(player_doll);
    assert(player_attacks.available_attacks.size() <= 6);

    for (int i = 0; i < player_attacks.available_attacks.size(); i++) {
        AttackName attack_name = player_attacks.available_attacks.at(i).name;
        Entity attack_move_button = createBattleMenuPlayerMove(renderer, ATTACK_OPTION_POSITIONS[i], MoveType::ATTACK,
                                                               attack_name, MagicName::NONE, GameItemName::NONE);
        scaleUIAsset(attack_move_button, attackOptionOriginalDimensions[attack_name], SCALE);
    }
}

void WorldSystem::renderPlayerMagicOptions() {
    assert(inBattleOrBossState());
    assert(selected_move_menu == BattleMenuItemType::MAGIC_BUTTON);

    // Create a list combining all the player's available magic
    MagicList player_magic = registry.magicLists.get(player_doll);
    vector<Magic> all_magic = player_magic.available_magic_attacks;
    all_magic.insert(
            all_magic.end(),
            player_magic.available_magic_defenses.begin(),
            player_magic.available_magic_defenses.end());

    all_magic.insert(
            all_magic.end(),
            player_magic.available_magic_effects.begin(),
            player_magic.available_magic_effects.end());

    assert(all_magic.size() <= 6);

    // Loop through and render each option as player move button
    for (int i = 0; i < all_magic.size(); i++) {
        MagicName magic_name = all_magic.at(i).name;
        vec2 render_position = {MAGIC_OPTION_POSITIONS[i].x + magicOptionOriginalDimensions[magic_name].x,
                                MAGIC_OPTION_POSITIONS[i].y};
        Entity magic_button = createBattleMenuPlayerMove(renderer, render_position, MoveType::MAGIC, AttackName::NONE,
                                                         magic_name, GameItemName::NONE);
        scaleUIAsset(magic_button, magicOptionOriginalDimensions[magic_name], SCALE);
    }
}

void WorldSystem::renderPlayerItemOptions() {
    assert(inBattleOrBossState());
    assert(selected_move_menu == BattleMenuItemType::ITEMS_BUTTON);
    // TODO: Render number beside the inventory item
    Inventory &player_inventory = registry.inventories.get(player_doll);

    for (int i = 0; i < player_inventory.items.size(); i++) {
        GameItem game_item = player_inventory.items.at(i);
        Entity item_button = createBattleMenuPlayerMove(renderer, ITEM_OPTION_POSITIONS[i], MoveType::ITEM, AttackName::NONE, MagicName::NONE, game_item.item_name);
        BattleMenuPlayerMove& player_move = registry.battleMenuPlayerMoves.get(item_button);
        player_move.item_id = game_item.world_id;
        scaleUIAsset(item_button, itemOptionOriginalDimensions[game_item.item_name], SCALE);
    }
}

void WorldSystem::resetPlayerMoveDefaultTextures() {
    assert(inBattleOrBossState());
    for (Entity entity : registry.battleMenuPlayerMoves.entities) {
        BattleMenuPlayerMove player_move = registry.battleMenuPlayerMoves.get(entity);
        RenderRequest &rr = registry.renderRequests.get(entity);
        rr.used_texture = player_move.default_texture;
    }
}

void WorldSystem::clearBattlePlayerMoveButtons() {
    assert(inBattleOrBossState());
    for (Entity entity : registry.battleMenuPlayerMoves.entities) {
        registry.remove_all_components_of(entity);
    }
}

void WorldSystem::detectAndHandleBattleScreenClick(double x, double y) {
    assert(inBattleOrBossState());
    // Get all button entities that are on screen (menu options and the player move options)
    vector <Entity> buttons_on_screen = registry.battleMenuButtons.entities;
    buttons_on_screen.insert(
            buttons_on_screen.end(),
            registry.battleMenuPlayerMoves.entities.begin(),
            registry.battleMenuPlayerMoves.entities.end());

    for (Entity button : buttons_on_screen) {
        Motion &motion = registry.motions.get(button);
        vec2 top_left_coords = {motion.position.x - motion.scale.x / 2, motion.position.y - motion.scale.y / 2};
        if (isClickInRegion(x, y, top_left_coords, motion.scale.y, motion.scale.x)) {
            handleBattleScreenButtonClick(button);
            break;
        }
    }
}

bool WorldSystem::isClickInRegion(double x, double y, vec2 top_left_coords, double height, double width) {
    int w, h;
    int originalHeight = registry.players.get(player_doll).fBHeight;
    int originalWidth = registry.players.get(player_doll).fBWidth;
    glfwGetFramebufferSize(window, &w, &h);
    int scaledX = x * originalWidth / w;
    int scaledY = y * originalHeight / h;
    return scaledX >= top_left_coords[0] &&
           scaledX <= top_left_coords[0] + width &&
           scaledY >= top_left_coords[1] &&
           scaledY <= top_left_coords[1] + height;
}

void WorldSystem::clearEnemy() {
    enemyMap.clear();
    enemyTypes.clear();
}

void WorldSystem::addEnemy(std::string type, int frequency) {
    enemyTypes.push_back(type);
    enemyMap.insert({type, frequency});
}

void WorldSystem::save() {
    gameProgress.health = registry.health.get(player_doll);
    gameProgress.mana = registry.mana.get(player_doll);
    gameProgress.attacks = registry.attackLists.get(player_doll).available_attacks;
    gameProgress.motion = registry.motions.get(player_doll);
    gameProgress.inventory_items = registry.inventories.get(player_doll).items;
    gameProgress.all_collected_item_ids = registry.inventories.get(player_doll).all_collected_item_ids;
    gameProgress.collected_memento_ids = registry.mementoInventories.get(player_doll).collected_memento_ids;
    json save;
    save["health"] = json();
    save["health"]["health"] = gameProgress.health.health;
    save["health"]["healthDecrement"] = gameProgress.health.healthDecrement;
    save["health"]["maxHP"] = gameProgress.health.maxHP;
    save["mana"]["mana"] = gameProgress.mana.mana;
    save["mana"]["maxMana"] = gameProgress.mana.maxMana;
    save["level"] = gameProgress.level;
    save["attacks"] = json::parse(createAttackString(gameProgress.attacks));
    save["magic"] = json::parse(createMagicString(gameProgress.magic));
    save["motion"] = json::parse(createMotionString(gameProgress.motion));
    save["inventory_items"] = json::parse(createItemListString(gameProgress.inventory_items));
    save["all_collected_item_ids"] = json::parse(createItemIdString(gameProgress.all_collected_item_ids));
    save["collected_memento_ids"] = json::parse(createItemIdString(gameProgress.collected_memento_ids));
    std::string obj = save.dump();
    std::ofstream myfile;
    std::string filePath = data_path();
    filePath += "/gameData/gameProgress.json";
    myfile.open(filePath);
    myfile << obj;
    myfile.close();
}

void WorldSystem::reset() {
    restart_game(GameStateChange::RESET);
}

void WorldSystem::incrementEnemiesKilled() {
    enemiesKilled++;
}

bool WorldSystem::inBattleOrBossState() {
    return registry.game.get(player_doll).state == GameState::BATTLE ||
           registry.game.get(player_doll).state == GameState::BOSS ||
           registry.game.get(player_doll).state == GameState::GAMEOVER;
}

void WorldSystem::playMoveSoundEffect(BattleMove move) {

    if (move.moveType == MoveType::ATTACK) {
        Mix_PlayChannel(-1, battle_hit_sound, 0);
    } else if (move.moveType == MoveType::ITEM) {
        Mix_PlayChannel(-1, heal_sound, 0);
    } else if (move.moveType == MoveType::MAGIC) {
        if (move.nameAsString == "LIGHTNING_STRIKE") {
            Mix_PlayChannel(-1, lightning_sound, 0);
        } else if (move.nameAsString == "FIREBALL") {
            Mix_PlayChannel(-1, fireball_sound, 0);
        } else if (move.nameAsString == "SPLASH") {
            Mix_PlayChannel(-1, splash_sound, 0);
        } else if (move.nameAsString == "SHIELD") {
            Mix_PlayChannel(-1, shield_sound, 0);
        } else if (
                move.nameAsString == "ARMOR_UP" ||
                move.nameAsString == "MAGIC_RESIST_UP" ||
                move.nameAsString == "MIND_AND_BODY" ||
                move.nameAsString == "QUICK_STEP") {
            Mix_PlayChannel(-1, power_up_sound, 0);
        } else if (move.nameAsString == "TAUNT") {
            Mix_PlayChannel(-1, taunt_sound, 0);
        } else if (move.nameAsString == "DARKNESS") {
            Mix_PlayChannel(-1, darkness_sound, 0);
        } else if (move.nameAsString == "THORNS") {
            Mix_PlayChannel(-1, thorns_sound, 0);
        } else if (move.nameAsString == "POISON") {
            Mix_PlayChannel(-1, poison_sound, 0);
        }
    }
}

RenderSystem *WorldSystem::getRenderer() {
    return renderer;
}
void WorldSystem::clearBattleMoveUsed() {
    for (Entity entity : registry.battleUsedMoves.entities) {
        registry.remove_all_components_of(entity);
    }
}

bool WorldSystem::hasMementoForCurrentLevel(Entity player_doll, int level) {
    if (level == 1 || level > 90) {
        return true;
    }
    else {
        MementoInventory inv = registry.mementoInventories.get(player_doll);
        return std::find(inv.collected_memento_ids.begin(), inv.collected_memento_ids.end(), level - 1) != inv.collected_memento_ids.end();
    }
}
