#pragma once

#include <array>
#include <utility>

#include "common.hpp"
#include "components.hpp"
#include "tiny_ecs.hpp"

// System responsible for setting up OpenGL and for rendering all the
// visual entities in the game
class RenderSystem {
	/**
	 * The following arrays store the assets the game will use. They are loaded
	 * at initialization and are assumed to not be modified by the render loop.
	 *
	 * Whenever possible, add to these lists instead of creating dynamic state
	 * it is easier to debug and faster to execute for the computer.
	 */
	std::array<GLuint, texture_count> texture_gl_handles;
	std::array<ivec2, texture_count> texture_dimensions;

	// Make sure these paths remain in sync with the associated enumerators.
	// Associated id with .obj path
	const std::vector < std::pair<GEOMETRY_BUFFER_ID, std::string>> mesh_paths =
	{
		  std::pair<GEOMETRY_BUFFER_ID, std::string>(GEOMETRY_BUFFER_ID::SHADOW, mesh_path("shadow.obj"))
		  // specify meshes of other assets here
	};

	// Make sure these paths remain in sync with the associated enumerators.
	const std::array<std::string, texture_count> texture_paths = {
			textures_path("enemies_spriteSheet.png"),
			textures_path("doll_spriteSheet.png"),
			textures_path("turtle.png"),
			textures_path("fish.png"),
			textures_path("turtle.png"),
			textures_path("fish.png"),
			textures_path("dresser_1.png"),
			textures_path("fish.png"),
			textures_path("bed_1.png"),
			textures_path("fish.png"),
			textures_path("turtle.png"),
			textures_path("fish.png"),
			textures_path("attic.png"),
			textures_path("helpscreen_1.png"),
			textures_path("helpscreen_2.png"),
			textures_path("helpscreen_3.png"),
			textures_path("helpscreen_4.png"),
			textures_path("helpscreen_5.png"),
			textures_path("MenuButton.png"),
			textures_path("MenuOverlay.png"),
			textures_path("Tutorial_1.png"),
			textures_path("Tutorial_2.png"),
			textures_path("Tutorial_3.png"),
			textures_path("Tutorial_4.png"),
			textures_path("Tutorial5.png"),
			textures_path("battle_screen_1.png"),
			textures_path("battle_menu_button_area.png"),
			textures_path("battle_menu_text_area.png"),
			textures_path("battle_menu_button_attack.png"),
			textures_path("battle_menu_button_magic.png"),
			textures_path("battle_menu_button_items.png"),
			textures_path("battle_menu_button_learn.png"),
			textures_path("battle_menu_button_go.png"),
			textures_path("battle_menu_attackOptions_PUNCH.png"),
			textures_path("battle_menu_attackOptions_PUNCH_selected.png"),
			textures_path("player_temp.png"),
			textures_path("enemy_temp.png"),
			textures_path("room1_diarypage.png"),
			textures_path("room1_speech1.png"),
			textures_path("room1_speech2.png"),
			textures_path("room1_speech3.png"),
			textures_path("room1_diary1.png"),
			textures_path("room1_diary2.png"),
			textures_path("room1_explore.png"),
			textures_path("level_1_toyRoom.png"),
			textures_path("level_2_kitchen_backup.png"),
			textures_path("level_3_bathroom.png"),
			textures_path("level_4_hallway.png"),
			textures_path("level_4_livingroom.png"),
			textures_path("level_5_garage.png"),
			textures_path("level_6_garden.png"),
			textures_path("level_7_studyroom.png"),
			textures_path("level_8_sistersRoom.png"),

			textures_path("health_potion_1.png"),
			textures_path("health_potion_2.png"),
			textures_path("door_attic.png"),
			textures_path("door_green.png"),
			textures_path("door_white.png"),
			textures_path("door_brown.png"),
			textures_path("door_brown_wreath.png"),
			textures_path("door_garden.png"),
			textures_path("star.png"),
			textures_path("parallax_background.png"),
			textures_path("parallax_layer0.png"),
			textures_path("parallax_layer1.png"),
			textures_path("parallax_layer2.png"),
			textures_path("healthbar_anna.png"),
			textures_path("healthbar_dustbunny.png"),
			textures_path("healthbar_teddybear.png"),
			textures_path("enemy_sneeze.png"),
			textures_path("battle_menu_itemOptions_MINI_POTION.png"),
			textures_path("battle_menu_itemOptions_MINI_POTION_selected.png"),
			textures_path("battle_menu_itemOptions_MEGA_POTION.png"),
			textures_path("battle_menu_itemOptions_MEGA_POTION_selected.png"),
			textures_path("battle_menu_magicOptions_SHIELD.png"),
			textures_path("battle_menu_magicOptions_SHIELD_selected.png"),
			textures_path("battle_menu_magicOptions_LIGHTNING.png"),
			textures_path("battle_menu_magicOptions_LIGHTNING_selected.png"),
			textures_path("battle_menu_magicOptions_SPLASH.png"),
			textures_path("battle_menu_magicOptions_SPLASH_selected.png"),
			textures_path("battle_menu_magicOptions_POISON.png"),
			textures_path("battle_menu_magicOptions_POISON_selected.png"),
			textures_path("battle_menu_magicOptions_TAUNT.png"),
			textures_path("battle_menu_magicOptions_TAUNT_selected.png"),
			textures_path("battle_menu_attackOptions_TRIP.png"),
			textures_path("battle_menu_attackOptions_TRIP_selected.png"),
			textures_path("battle_menu_magicOptions_ARMORUP.png"),
			textures_path("battle_menu_magicOptions_ARMORUP_selected.png"),
			textures_path("battle_menu_magicOptions_MINDANDBODY.png"),
			textures_path("battle_menu_magicOptions_MINDANDBODY_selected.png"),
			textures_path("battle_menu_magicOptions_RESISTUP.png"),
			textures_path("battle_menu_magicOptions_RESISTUP_selected.png"),
			textures_path("battle_menu_magicOptions_QUICKSTEP.png"),
			textures_path("battle_menu_magicOptions_QUICKSTEP_selected.png"),
			textures_path("battle_menu_magicOptions_THORNS.png"),
			textures_path("battle_menu_magicOptions_THORNS_selected.png"),
			textures_path("battle_menu_magicOptions_FIREBALL.png"),
			textures_path("battle_menu_magicOptions_FIREBALL_selected.png"),
			textures_path("battle_menu_magicOptions_DARKNESS.png"),
			textures_path("battle_menu_magicOptions_DARKNESS_selected.png"),
			textures_path("Memento_Object_1.png"),
			textures_path("Memento_Object_2.png"),
			textures_path("Memento_Object_3.png"),
			textures_path("Memento_Object_4.png"),
			textures_path("Memento_Object_5.png"),
			textures_path("Memento_Object_6.png"),
			textures_path("Memento_Object_7.png"),
			textures_path("Memento_Object_8.png"),
			textures_path("Memento_Text_1.png"),
			textures_path("Memento_Text_2.png"),
			textures_path("Memento_Text_3.png"),
			textures_path("Memento_Text_4.png"),
			textures_path("Memento_Text_5.png"),
			textures_path("Memento_Text_6.png"),
			textures_path("Memento_Text_7.png"),
			textures_path("Memento_Text_8.png"),
            textures_path("enemy_armorup.png"),
            textures_path("enemy_darkness.png"),
            textures_path("enemy_fireball.png"),
            textures_path("enemy_lightningstrike.png"),
            textures_path("enemy_magicresistup.png"),
            textures_path("enemy_mindandbody.png"),
            textures_path("enemy_poison.png"),
            textures_path("enemy_quickstep.png"),
            textures_path("enemy_scratch.png"),
            textures_path("enemy_shield.png"),
            textures_path("enemy_splash.png"),
            textures_path("enemy_taunt.png"),
            textures_path("enemy_thorns.png"),
            textures_path("enemy_trip.png"),
            textures_path("player_armorup.png"),
            textures_path("player_darkness.png"),
            textures_path("player_fireball.png"),
            textures_path("player_lightningstrike.png"),
            textures_path("player_magicresistup.png"),
            textures_path("player_mindandbody.png"),
            textures_path("player_poison.png"),
            textures_path("player_quickstep.png"),
            textures_path("player_scratch.png"),
            textures_path("player_shield.png"),
            textures_path("player_splash.png"),
            textures_path("player_taunt.png"),
            textures_path("player_thorns.png"),
            textures_path("player_trip.png"),
            textures_path("player_sneeze.png"),
            textures_path("enemy_healthpotion.png"),
            textures_path("player_healthpotion.png"),
            textures_path("enemy_punch.png"),
            textures_path("player_punch.png"),
            textures_path("not_enough_mana.png"),
			textures_path("splash_screen.png"),
			textures_path("title_screen.png"),
			textures_path("gameover.png"),
			textures_path("dollhouse_door.png"),
			textures_path("flora.png"),
			textures_path("animatronic_torso.png"),
			textures_path("animatronic_arm1.png"),
			textures_path("animatronic_arm2.png"),
			textures_path("animatronic_leg1.png"),
			textures_path("animatronic_leg2.png"),
			textures_path("healthbar_flora2.png"),
			textures_path("team_spritesheet_1.png"),
			textures_path("team_spritesheet_2.png"),
			textures_path("credits.png"),
			textures_path("youwin.png"),
			textures_path("healthbar_flora.png"),
            textures_path("TUTORIAL_ATTIC_1.png"),
            textures_path("TUTORIAL_ATTIC_2.png"),
            textures_path("TUTORIAL_ATTIC_3.png"),
            textures_path("TUTORIAL_ATTIC_4.png"),
            textures_path("TUTORIAL_ATTIC_LOCK_ANNA.png"),
            textures_path("TUTORIAL_ATTIC_LOCK_WARNING.png"),
            textures_path("TUTORIAL_DOOR_LOCK_WARNING.png"),
            textures_path("TUTORIAL_HALLWAY_1.png"),
            textures_path("TUTORIAL_HALLWAY_2.png"),
            textures_path("TUTORIAL_HALLWAY_3.png"),
            textures_path("TUTORIAL_HALLWAY_4.png"),
            textures_path("TUTORIAL_HALLWAY_5.png"),
            textures_path("TUTORIAL_HALLWAY_AGAIN.png"),
            textures_path("TUTORIAL_PREBOSS_1.png"),
            textures_path("TUTORIAL_PREBOSS_2.png"),
            textures_path("TUTORIAL_PREBOSS_3.png"),
            textures_path("TUTORIAL_PREBOSS_4.png"),
            textures_path("TUTORIAL_PREBOSS_5.png"),
            textures_path("TUTORIAL_PREBOSS_6.png"),
            textures_path("TUTORIAL_PREBOSS_7.png"),
            textures_path("TUTORIAL_PREBOSS_8.png"),
            textures_path("TUTORIAL_PREBOSS_9.png"),
    };

	std::array<GLuint, effect_count> effects;
	// Make sure these paths remain in sync with the associated enumerators.
	const std::array<std::string, effect_count> effect_paths = {
		shader_path("coloured"),
		shader_path("shadow"),
		shader_path("textured"),
		shader_path("texturedAnimation"),
		shader_path("verticalBlur"),
		shader_path("horizontalBlur"),
        shader_path("helpScreen"),
        shader_path("pebble"),
        shader_path("drawToScreen"),
		shader_path("glowingTexture"),
		shader_path("healthbar_player"),
		shader_path("healthbar_enemy")
	};

	std::array<GLuint, geometry_count> vertex_buffers;
	std::array<GLuint, geometry_count> index_buffers;
	std::array<Mesh, geometry_count> meshes;

public:
    // Initialize the window
    bool init(int width, int height, GLFWwindow *window);

    template<class T>
    void bindVBOandIBO(GEOMETRY_BUFFER_ID gid, std::vector<T> vertices, std::vector<uint16_t> indices);

    void initializeGlTextures();

    void initializeGlEffects();

    void initializeGlMeshes();

    Mesh &getMesh(GEOMETRY_BUFFER_ID id) { return meshes[(int) id]; };

    void initializeGlGeometryBuffers();

    // Initialize the screen texture used as intermediate render target
    // The draw loop first renders to this texture, then it is used for the water
    // shader
    bool initScreenTexture();

    // Destroy resources associated to one or all entities created by the system
    ~RenderSystem();

    // Draw all entities
    void draw();

    mat3 createProjectionMatrix();

    mat3 specialProjectionMatrix();

private:
    void findVar();

    // Internal drawing functions for each entity type
    void drawTexturedMesh(Entity entity, const mat3 &projection);

    void drawLightAndGlowEffect();

    void drawToScreen();

    void verticalBlur();

    void horizontalBlur();

    void drawAnimatedSprite(Entity entity);

    void drawOverlayWindow(Entity entity,
                           const mat3 &projection);

    bool isInOverlayWindow1(Entity entity);

    bool isInOverlayWindow2(Entity entity);

    bool isInOverlayWindow3(Entity entity);

    bool isInOverlayWindow4(Entity entity);

    bool isInOverlayWindow5(Entity entity);

    // Window handle
    GLFWwindow *window;
    float screen_scale;  // Screen to pixel coordinates scale factor (for apple
    // retina display?)

    // Screen texture handles
    GLuint frame_buffer;
    GLuint off_screen_render_buffer_color;
    GLuint off_screen_render_buffer_depth;

    Entity screen_state_entity;
};

bool loadEffectFromFile(
        const std::string &vs_path, const std::string &fs_path, GLuint &out_program);
