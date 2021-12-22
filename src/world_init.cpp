#include "world_init.hpp"
#include "tiny_ecs_registry.hpp"

Entity createStar(RenderSystem *renderer){
    auto entity = Entity();

    // Store a reference to the potentially re-used mesh object
    Mesh &mesh = renderer->getMesh(GEOMETRY_BUFFER_ID::SPRITE);
    registry.meshPtrs.emplace(entity, &mesh);
    registry.glowingObject.emplace(entity);

    // Setting initial motion values
    Motion &motion = registry.motions.emplace(entity);
    motion.dir = Direction::DOWN;
    motion.position = {600, 400};
    motion.angle = 0.f;
    motion.velocity = {0.f, 0.f};
    motion.collision_proof = 1;
    motion.scale.y = 800; // fit to screen
    motion.scale.x = 1200; // fit to screen

    registry.renderRequests.insert(
            entity,
            {TEXTURE_ASSET_ID::STAR,
             EFFECT_ASSET_ID::TEXTURED,
             GEOMETRY_BUFFER_ID::SPRITE});

    return entity;
}

Entity createBackground(RenderSystem *renderer, vec2 pos, TEXTURE_ASSET_ID background)
{
    auto entity = Entity();

    // Store a reference to the potentially re-used mesh object
    Mesh &mesh = renderer->getMesh(GEOMETRY_BUFFER_ID::SPRITE);
    registry.meshPtrs.emplace(entity, &mesh);

    // Setting initial motion values
    Motion &motion = registry.motions.emplace(entity);
    motion.dir = Direction::DOWN;
    motion.position = vec2(600, 400);
    motion.angle = 0.f;
    motion.velocity = {0.f, 0.f};
    motion.collision_proof = 1;
    motion.scale.y = (pos.y * 2) / mesh.original_size.y; // fit to screen
    motion.scale.x = (pos.x * 2) / mesh.original_size.x; // fit to screen
    registry.backgrounds.emplace(entity);


    if (background == (TEXTURE_ASSET_ID) 47){
        motion.scale *= 2;
        motion.position = vec2(1040, 400);
        registry.renderRequests.insert(
                entity,
                {(TEXTURE_ASSET_ID) background,
                 EFFECT_ASSET_ID::TEXTURED,
                 GEOMETRY_BUFFER_ID::HELP_SCREEN});
    } else {
        registry.renderRequests.insert(
                entity,
                {(TEXTURE_ASSET_ID) background,
                 EFFECT_ASSET_ID::TEXTURED,
                 GEOMETRY_BUFFER_ID::SPRITE});
    }

    return entity;
}

// this entity can be blurred when blur activates
Entity createHelpWindow(RenderSystem *renderer, vec2 pos)
{
    auto entity = Entity();
    // width: height = 2 : 1
    Mesh &mesh = renderer->getMesh(GEOMETRY_BUFFER_ID::HELP_SCREEN);
    registry.meshPtrs.emplace(entity, &mesh);
    Motion &motion = registry.motions.emplace(entity);
    motion.dir = Direction::DOWN;
    motion.position = pos;
    motion.angle = 0.f;
    motion.velocity = {0.f, 0.f};
    motion.scale = mesh.original_size * float(220);
    motion.collision_proof = 1;
    registry.renderRequests.insert(
        entity,
        {TEXTURE_ASSET_ID::HELP_1,
         EFFECT_ASSET_ID::HELP_SCREEN,
         GEOMETRY_BUFFER_ID::HELP_SCREEN});
    return entity;
}

Entity createMenuButton(RenderSystem *renderer, vec2 pos)
{
    auto entity = Entity();
    Mesh &mesh = renderer->getMesh(GEOMETRY_BUFFER_ID::HELP_SCREEN);
    registry.meshPtrs.emplace(entity, &mesh);
    Motion &motion = registry.motions.emplace(entity);
    motion.dir = Direction::DOWN;
    motion.position = pos;
    motion.angle = 0.f;
    motion.velocity = {0.f, 0.f};
    motion.scale = mesh.original_size;
    motion.collision_proof = 1;
    registry.helpScreens.emplace(entity);
    registry.renderRequests.insert(
        entity,
        {TEXTURE_ASSET_ID::MENU_BUTTON,
         EFFECT_ASSET_ID::HELP_SCREEN,
         GEOMETRY_BUFFER_ID::HELP_SCREEN});
    return entity;
}

Entity createMenuOverlay(RenderSystem *renderer, vec2 pos)
{
    auto entity = Entity();
    Mesh &mesh = renderer->getMesh(GEOMETRY_BUFFER_ID::HELP_SCREEN);
    registry.meshPtrs.emplace(entity, &mesh);
    Motion &motion = registry.motions.emplace(entity);
    motion.dir = Direction::DOWN;
    motion.position = pos;
    motion.angle = 0.f;
    motion.velocity = {0.f, 0.f};
    motion.scale = mesh.original_size;
    motion.collision_proof = 1;
    registry.helpScreens.emplace(entity);
    //    registry.renderRequests.insert(
    //            entity,
    //            { TEXTURE_ASSET_ID::MENU_OVERLAY,
    //              EFFECT_ASSET_ID::HELP_SCREEN,
    //              GEOMETRY_BUFFER_ID::SPRITE});
    return entity;
}

Entity createTutorial(RenderSystem *renderer, vec2 pos)
{
    auto entity = Entity();
    Mesh &mesh = renderer->getMesh(GEOMETRY_BUFFER_ID::HELP_SCREEN);
    registry.meshPtrs.emplace(entity, &mesh);
    Motion &motion = registry.motions.emplace(entity);
    motion.dir = Direction::DOWN;
    motion.position = pos;
    motion.angle = 0.f;
    motion.velocity = {0.f, 0.f};
    motion.scale = {640, 200};
    motion.collision_proof = 1;
    registry.tutorialTimer.emplace(entity);
    registry.helpScreens.emplace(entity);
    registry.renderRequests.insert(
        entity,
        {TEXTURE_ASSET_ID::TUTORIAL_ONE,
         EFFECT_ASSET_ID::HELP_SCREEN,
         GEOMETRY_BUFFER_ID::SPRITE});
    return entity;
}

Entity createDoorLockTutorial(RenderSystem *renderer)
{
    auto entity = Entity();
    Mesh &mesh = renderer->getMesh(GEOMETRY_BUFFER_ID::HELP_SCREEN);
    registry.meshPtrs.emplace(entity, &mesh);
    Motion &motion = registry.motions.emplace(entity);
    motion.dir = Direction::DOWN;
    motion.position = {600, 600};
    motion.angle = 0.f;
    motion.velocity = {0.f, 0.f};
    motion.scale = {640, 200};
    motion.collision_proof = 1;
    registry.tutorialTimer.emplace(entity);
    registry.helpScreens.emplace(entity);
    registry.renderRequests.insert(
            entity,
            {TEXTURE_ASSET_ID::TUTORIAL_ATTIC_LOCK_ANNA,
             EFFECT_ASSET_ID::HELP_SCREEN,
             GEOMETRY_BUFFER_ID::SPRITE});
    return entity;
}

Entity createPreBossTutorial(RenderSystem *renderer)
{
    auto entity = Entity();
    Mesh &mesh = renderer->getMesh(GEOMETRY_BUFFER_ID::HELP_SCREEN);
    registry.meshPtrs.emplace(entity, &mesh);
    Motion &motion = registry.motions.emplace(entity);
    motion.dir = Direction::DOWN;
    motion.position = {600, 600};
    motion.angle = 0.f;
    motion.velocity = {0.f, 0.f};
    motion.scale = {640, 200};
    motion.collision_proof = 1;
    registry.tutorialTimer.emplace(entity);
    registry.helpScreens.emplace(entity);
    registry.renderRequests.insert(
            entity,
            {TEXTURE_ASSET_ID::TUTORIAL_PREBOSS_1,
             EFFECT_ASSET_ID::HELP_SCREEN,
             GEOMETRY_BUFFER_ID::SPRITE});
    return entity;
}

Entity createPopUpWindow(RenderSystem *renderer, vec2 pos)
{
    auto entity = Entity();
    Mesh &mesh = renderer->getMesh(GEOMETRY_BUFFER_ID::SPRITE);
    registry.meshPtrs.emplace(entity, &mesh);
    Motion &motion = registry.motions.emplace(entity);
    motion.dir = Direction::DOWN;
    motion.position = pos;
    motion.angle = 0.f;
    motion.velocity = {0.f, 0.f};
    //    motion.scale = mesh.original_size;
    motion.scale = {1000.f, 175.f};
    motion.collision_proof = 1;
    //    registry.popUpTimers.emplace(entity);
    registry.popups.emplace(entity);
    registry.renderRequests.insert(
        entity,
        {TEXTURE_ASSET_ID::ROOM1_SPEECH1,
         EFFECT_ASSET_ID::HELP_SCREEN,
         GEOMETRY_BUFFER_ID::SPRITE});
    return entity;
}



Entity createMementoText(RenderSystem* renderer, vec2 pos, MementoName id)
{
    auto entity = Entity();
    Mesh& mesh = renderer->getMesh(GEOMETRY_BUFFER_ID::SPRITE);
    registry.meshPtrs.emplace(entity, &mesh);
    Motion& motion = registry.motions.emplace(entity);
    motion.dir = Direction::DOWN;
    motion.position = pos;
    motion.angle = 0.f;
    motion.velocity = { 0.f, 0.f };
    motion.scale = { 350.f, 350.f };
    motion.collision_proof = 1;

    TEXTURE_ASSET_ID asset_id = TEXTURE_ASSET_ID::TEXTURE_COUNT;

    if (id == MementoName::PIG) {
        asset_id = TEXTURE_ASSET_ID::MEMENTO_TEXT_1;
    } else if (id == MementoName::CUPCAKES) {
        asset_id = TEXTURE_ASSET_ID::MEMENTO_TEXT_2;
    } else if (id == MementoName::BRUSH) {
        asset_id = TEXTURE_ASSET_ID::MEMENTO_TEXT_3;
    } else if (id == MementoName::REMOTE) {
        asset_id = TEXTURE_ASSET_ID::MEMENTO_TEXT_4;
    } else if (id == MementoName::BIKE) {
        asset_id = TEXTURE_ASSET_ID::MEMENTO_TEXT_5;
    } else if (id == MementoName::FLOWER) {
        asset_id = TEXTURE_ASSET_ID::MEMENTO_TEXT_6;
    } else if (id == MementoName::TEA) {
        asset_id = TEXTURE_ASSET_ID::MEMENTO_TEXT_7;
    } else if (id == MementoName::DRAWING) {
        asset_id = TEXTURE_ASSET_ID::MEMENTO_TEXT_8;
    }

    registry.popups.emplace(entity);
    registry.renderRequests.insert(
        entity,
        { asset_id,
         EFFECT_ASSET_ID::HELP_SCREEN,
         GEOMETRY_BUFFER_ID::SPRITE });
    return entity;
}

Entity createBattleWindow(RenderSystem *renderer, vec2 pos)
{
    auto entity = Entity();
    Mesh &mesh = renderer->getMesh(GEOMETRY_BUFFER_ID::SPRITE);
    registry.meshPtrs.emplace(entity, &mesh);
    Motion &motion = registry.motions.emplace(entity);
    motion.dir = Direction::DOWN;
    motion.position = pos;
    motion.angle = 0.f;
    motion.velocity = {0.f, 0.f};
    motion.scale.y = (pos.y * 2) / mesh.original_size.y; // fit to screen
    motion.scale.x = (pos.x * 2) / mesh.original_size.x; // fit to screen
    motion.collision_proof = 1;
    registry.battleScreens.emplace(entity);
    registry.renderRequests.insert(
        entity,
        {TEXTURE_ASSET_ID::BATTLE_BACKGROUND_1,
         // help screen effect just renders it like the help screen
         EFFECT_ASSET_ID::HELP_SCREEN,
         GEOMETRY_BUFFER_ID::SPRITE});
    return entity;
}

Entity createBattleParallaxWindow(RenderSystem *renderer, vec2 pos, TEXTURE_ASSET_ID textureAssetId) {
    auto entity = Entity();
    Mesh &mesh = renderer->getMesh(GEOMETRY_BUFFER_ID::SPRITE);
    registry.meshPtrs.emplace(entity, &mesh);
    Motion &motion = registry.motions.emplace(entity);
    vec2 velocity = vec2();
    if (textureAssetId == TEXTURE_ASSET_ID::PARALLAX_BACKGROUND) {
        velocity = vec2();
    } else if (textureAssetId == TEXTURE_ASSET_ID::PARALLAX_LAYER0) {
        velocity = vec2(10, 0);
    } else if (textureAssetId == TEXTURE_ASSET_ID::PARALLAX_LAYER1) {
        velocity = vec2(20, 0);
    } else if (textureAssetId == TEXTURE_ASSET_ID::PARALLAX_LAYER2) {
        velocity = vec2(30, 0);
    }
    velocity *= 5;
    motion.dir = Direction::DOWN;
    motion.position = pos;
    motion.angle = 0.f;
    motion.velocity = velocity;
    motion.scale.y = (pos.y * 2) / mesh.original_size.y; // fit to screen
    motion.scale.x = (pos.x * 2) / mesh.original_size.x; // fit to screen
    motion.collision_proof = 1;
    registry.battleScreens.emplace(entity);
    registry.parallaxBackground.emplace(entity);
    registry.renderRequests.insert(
            entity,
            {textureAssetId,
                    // help screen effect just renders it like the help screen
             EFFECT_ASSET_ID::HELP_SCREEN,
             GEOMETRY_BUFFER_ID::SPRITE});
    return entity;
}

Entity createBattleMenuItem(RenderSystem *renderer, vec2 pos, BattleMenuItemType item_type, TEXTURE_ASSET_ID texture_id)
{
    auto entity = Entity();
    Mesh &mesh = renderer->getMesh(GEOMETRY_BUFFER_ID::SPRITE);
    registry.meshPtrs.emplace(entity, &mesh);

    Motion &motion = registry.motions.emplace(entity);
    motion.dir = Direction::DOWN;
    motion.position = pos;
    motion.angle = 0.f;
    motion.velocity = {0.f, 0.f};
    motion.scale = mesh.original_size;
    motion.collision_proof = 1;

    if (item_type == BattleMenuItemType::BUTTON_AREA || item_type == BattleMenuItemType::TEXT_AREA)
    {
        BattleMenu &battle_menu = registry.battleMenus.emplace(entity);
        battle_menu.menu_type = item_type;
    }
    else if (
        item_type == BattleMenuItemType::ATTACK_BUTTON ||
        item_type == BattleMenuItemType::MAGIC_BUTTON ||
        item_type == BattleMenuItemType::ITEMS_BUTTON ||
        item_type == BattleMenuItemType::LEARN_BUTTON ||
        item_type == BattleMenuItemType::GO_BUTTON)
    {
        BattleMenuButton &battle_menu_button = registry.battleMenuButtons.emplace(entity);
        battle_menu_button.button_type = item_type;
    }

    registry.renderRequests.insert(
        entity,
        {texture_id,
         EFFECT_ASSET_ID::HELP_SCREEN,
         GEOMETRY_BUFFER_ID::SPRITE});
    return entity;
}

Entity createBattleMenuPlayerMove(RenderSystem* renderer, vec2 pos, MoveType move_type, AttackName attack_name, MagicName magic_name, GameItemName item_name) {
    auto entity = Entity();
    Mesh& mesh = renderer->getMesh(GEOMETRY_BUFFER_ID::SPRITE);
    registry.meshPtrs.emplace(entity, &mesh);

    Motion& motion = registry.motions.emplace(entity);
    motion.dir = Direction::DOWN;
    motion.position = pos;
    motion.angle = 0.f;
    motion.velocity = { 0.f, 0.f };
    motion.scale = mesh.original_size;
    motion.collision_proof = 1;

    BattleMenuPlayerMove& player_move = registry.battleMenuPlayerMoves.emplace(entity);
    //player_move.move_type = move_type;
    player_move.default_texture = getBattleMenuPlayerMoveDefaultTexture(move_type, attack_name, magic_name, item_name);
    player_move.selected_texture = (TEXTURE_ASSET_ID)((int)player_move.default_texture + 1);
    player_move.attack_name = attack_name;
    player_move.magic_name = magic_name;
    player_move.item_name = item_name;

    registry.renderRequests.insert(
        entity,
        { player_move.default_texture,
         EFFECT_ASSET_ID::HELP_SCREEN,
         GEOMETRY_BUFFER_ID::SPRITE });

    return entity;
}

// helper function for createBattleMenuPlayerMove
TEXTURE_ASSET_ID getBattleMenuPlayerMoveDefaultTexture(MoveType move_type, AttackName attack_name, MagicName magic_name, GameItemName item_name) {
    if (move_type == MoveType::ATTACK) {
        switch (attack_name)
        {
            case AttackName::PUNCH: return TEXTURE_ASSET_ID::ATTACK_OPTIONS_PUNCH;
            case AttackName::TRIP: return TEXTURE_ASSET_ID::ATTACK_OPTIONS_TRIP;
            default: return TEXTURE_ASSET_ID::ATTACK_OPTIONS_PUNCH; // we really shouldn't end up in the default case ever
        }
    }
    else if (move_type == MoveType::MAGIC) {
        switch (magic_name)
        {
            case MagicName::LIGHTNING_STRIKE: return TEXTURE_ASSET_ID::MAGIC_OPTIONS_LIGHTNING;
            case MagicName::SHIELD: return TEXTURE_ASSET_ID::MAGIC_OPTIONS_SHIELD;
            case MagicName::SPLASH: return TEXTURE_ASSET_ID::MAGIC_OPTIONS_SPLASH;
            case MagicName::POISON_CLOUD: return TEXTURE_ASSET_ID::MAGIC_OPTIONS_POISON;
            case MagicName::TAUNT: return TEXTURE_ASSET_ID::MAGIC_OPTIONS_TAUNT;
            case MagicName::ARMOR_UP: return TEXTURE_ASSET_ID::MAGIC_OPTIONS_ARMOR;
            case MagicName::MAGIC_RESIST_UP: return TEXTURE_ASSET_ID::MAGIC_OPTIONS_RESIST_UP;
            case MagicName::QUICK_STEP: return TEXTURE_ASSET_ID::MAGIC_OPTIONS_QUICKSTEP;
            case MagicName::MIND_AND_BODY: return TEXTURE_ASSET_ID::MAGIC_OPTIONS_MIND_AND_BODY;
            case MagicName::DARKNESS: return TEXTURE_ASSET_ID::MAGIC_OPTIONS_DARKNESS;
            case MagicName::THORNS: return TEXTURE_ASSET_ID::MAGIC_OPTIONS_THORNS;
            case MagicName::FIREBALL: return TEXTURE_ASSET_ID::MAGIC_OPTIONS_FIREBALL;
            default: return TEXTURE_ASSET_ID::ATTACK_OPTIONS_PUNCH;
        }
    }
    else if (move_type == MoveType::ITEM) {
        switch (item_name)
        {
            case GameItemName::HEALING_POTION: return TEXTURE_ASSET_ID::ITEM_OPTIONS_POTION_MINI;
            case GameItemName::HEALING_POTION_2: return TEXTURE_ASSET_ID::ITEM_OPTIONS_POTION_MEGA;
            default: return TEXTURE_ASSET_ID::ATTACK_OPTIONS_PUNCH;
        }
    }
    else {
        return TEXTURE_ASSET_ID::ATTACK_OPTIONS_PUNCH;
    }
}

Entity createBattleMoveUsed(RenderSystem* renderer, BattleMove move, BattleState battle_state)
{
    auto entity = Entity();
    Mesh &mesh = renderer->getMesh(GEOMETRY_BUFFER_ID::SPRITE);
    registry.meshPtrs.emplace(entity, &mesh);
    Motion &motion = registry.motions.emplace(entity);
    motion.dir = Direction::DOWN;
    motion.position = {600.f, 500.f};
    motion.angle = 0.f;
    motion.velocity = {0.f, 0.f};
    motion.scale.x = 750.f*mesh.original_size.x;
    motion.scale.y = 60.f*mesh.original_size.y;
    motion.collision_proof = 1;
    registry.battleUsedMoves.emplace(entity);

    if (battle_state == BattleState::PLAYER_TURN) {
        if (move.moveType == MoveType::ATTACK || move.moveType == MoveType::MAGIC) {
            if (move.nameAsString == "ARMOR_UP") { 
                registry.renderRequests.insert(
                        entity,
                        {TEXTURE_ASSET_ID::PLAYER_ARMORUP,
                         EFFECT_ASSET_ID::HELP_SCREEN,
                         GEOMETRY_BUFFER_ID::SPRITE});
            } else if (move.nameAsString == "DARKNESS") {
                registry.renderRequests.insert(
                        entity,
                        {TEXTURE_ASSET_ID::PLAYER_DARKNESS,
                         EFFECT_ASSET_ID::HELP_SCREEN,
                         GEOMETRY_BUFFER_ID::SPRITE});
            } else if (move.nameAsString == "FIREBALL") {
                registry.renderRequests.insert(
                        entity,
                        {TEXTURE_ASSET_ID::PLAYER_FIREBALL,
                         EFFECT_ASSET_ID::HELP_SCREEN,
                         GEOMETRY_BUFFER_ID::SPRITE});
            } else if (move.nameAsString == "LIGHTNING_STRIKE") {
                registry.renderRequests.insert(
                        entity,
                        {TEXTURE_ASSET_ID::PLAYER_LIGHTNINGSTRIKE,
                         EFFECT_ASSET_ID::HELP_SCREEN,
                         GEOMETRY_BUFFER_ID::SPRITE});
            } else if (move.nameAsString == "MAGIC_RESIST_UP") {
                registry.renderRequests.insert(
                        entity,
                        {TEXTURE_ASSET_ID::PLAYER_MAGICRESISTUP,
                         EFFECT_ASSET_ID::HELP_SCREEN,
                         GEOMETRY_BUFFER_ID::SPRITE});
            } else if (move.nameAsString == "MIND_AND_BODY") {
                registry.renderRequests.insert(
                        entity,
                        {TEXTURE_ASSET_ID::PLAYER_MINDANDBODY,
                         EFFECT_ASSET_ID::HELP_SCREEN,
                         GEOMETRY_BUFFER_ID::SPRITE});
            } else if (move.nameAsString == "POISON") {
                registry.renderRequests.insert(
                        entity,
                        {TEXTURE_ASSET_ID::PLAYER_POISON,
                         EFFECT_ASSET_ID::HELP_SCREEN,
                         GEOMETRY_BUFFER_ID::SPRITE});
            } else if (move.nameAsString == "PUNCH") {
                registry.renderRequests.insert(
                        entity,
                        {TEXTURE_ASSET_ID::PLAYER_PUNCH,
                         EFFECT_ASSET_ID::HELP_SCREEN,
                         GEOMETRY_BUFFER_ID::SPRITE});
            } else if (move.nameAsString == "QUICKSTEP") {
                registry.renderRequests.insert(
                        entity,
                        {TEXTURE_ASSET_ID::PLAYER_QUICKSTEP,
                         EFFECT_ASSET_ID::HELP_SCREEN,
                         GEOMETRY_BUFFER_ID::SPRITE});
            } else if (move.nameAsString == "SCRATCH") {
                registry.renderRequests.insert(
                        entity,
                        {TEXTURE_ASSET_ID::PLAYER_SCRATCH,
                         EFFECT_ASSET_ID::HELP_SCREEN,
                         GEOMETRY_BUFFER_ID::SPRITE});
            } else if (move.nameAsString == "SHIELD") {
                registry.renderRequests.insert(
                        entity,
                        {TEXTURE_ASSET_ID::PLAYER_SHIELD,
                         EFFECT_ASSET_ID::HELP_SCREEN,
                         GEOMETRY_BUFFER_ID::SPRITE});
            } else if (move.nameAsString == "SNEEZE") {
                registry.renderRequests.insert(
                        entity,
                        {TEXTURE_ASSET_ID::PLAYER_SNEEZE,
                         EFFECT_ASSET_ID::HELP_SCREEN,
                         GEOMETRY_BUFFER_ID::SPRITE});
            } else if (move.nameAsString == "SPLASH") {
                registry.renderRequests.insert(
                        entity,
                        {TEXTURE_ASSET_ID::PLAYER_SPLASH,
                         EFFECT_ASSET_ID::HELP_SCREEN,
                         GEOMETRY_BUFFER_ID::SPRITE});
            } else if (move.nameAsString == "TAUNT") {
                registry.renderRequests.insert(
                        entity,
                        {TEXTURE_ASSET_ID::PLAYER_TAUNT,
                         EFFECT_ASSET_ID::HELP_SCREEN,
                         GEOMETRY_BUFFER_ID::SPRITE});
            } else if (move.nameAsString == "THORNS") {
                registry.renderRequests.insert(
                        entity,
                        {TEXTURE_ASSET_ID::PLAYER_THORNS,
                         EFFECT_ASSET_ID::HELP_SCREEN,
                         GEOMETRY_BUFFER_ID::SPRITE});
            } else if (move.nameAsString == "TRIP") {
                registry.renderRequests.insert(
                        entity,
                        {TEXTURE_ASSET_ID::PLAYER_TRIP,
                         EFFECT_ASSET_ID::HELP_SCREEN,
                         GEOMETRY_BUFFER_ID::SPRITE});
            } else if (move.nameAsString == "NOT_ENOUGH_MANA") {
                registry.renderRequests.insert(
                        entity,
                        {TEXTURE_ASSET_ID::NOT_ENOUGH_MANA,
                         EFFECT_ASSET_ID::HELP_SCREEN,
                         GEOMETRY_BUFFER_ID::SPRITE});
            }

        } else if (move.moveType == MoveType::ITEM) {
            registry.renderRequests.insert(
                    entity,
                    {TEXTURE_ASSET_ID::PLAYER_HEALTHPOTION,
                     EFFECT_ASSET_ID::HELP_SCREEN,
                     GEOMETRY_BUFFER_ID::SPRITE});
        }
    } else if (battle_state == BattleState::ENEMY_TURN) {
        if (move.moveType == MoveType::ATTACK || move.moveType == MoveType::MAGIC) {
            if (move.nameAsString == "ARMOR_UP") {
                registry.renderRequests.insert(
                        entity,
                        {TEXTURE_ASSET_ID::ENEMY_ARMORUP,
                         EFFECT_ASSET_ID::HELP_SCREEN,
                         GEOMETRY_BUFFER_ID::SPRITE});
            } else if (move.nameAsString == "DARKNESS") {
                registry.renderRequests.insert(
                        entity,
                        {TEXTURE_ASSET_ID::ENEMY_DARKNESS,
                         EFFECT_ASSET_ID::HELP_SCREEN,
                         GEOMETRY_BUFFER_ID::SPRITE});
            } else if (move.nameAsString == "FIREBALL") {
                registry.renderRequests.insert(
                        entity,
                        {TEXTURE_ASSET_ID::ENEMY_FIREBALL,
                         EFFECT_ASSET_ID::HELP_SCREEN,
                         GEOMETRY_BUFFER_ID::SPRITE});
            } else if (move.nameAsString == "LIGHTNING_STRIKE") {
                registry.renderRequests.insert(
                        entity,
                        {TEXTURE_ASSET_ID::ENEMY_LIGHTNINGSTRIKE,
                         EFFECT_ASSET_ID::HELP_SCREEN,
                         GEOMETRY_BUFFER_ID::SPRITE});
            } else if (move.nameAsString == "MAGIC_RESIST_UP") {
                registry.renderRequests.insert(
                        entity,
                        {TEXTURE_ASSET_ID::ENEMY_MAGICRESISTUP,
                         EFFECT_ASSET_ID::HELP_SCREEN,
                         GEOMETRY_BUFFER_ID::SPRITE});
            } else if (move.nameAsString == "MIND_AND_BODY") {
                registry.renderRequests.insert(
                        entity,
                        {TEXTURE_ASSET_ID::ENEMY_MINDANDBODY,
                         EFFECT_ASSET_ID::HELP_SCREEN,
                         GEOMETRY_BUFFER_ID::SPRITE});
            } else if (move.nameAsString == "POISON") {
                registry.renderRequests.insert(
                        entity,
                        {TEXTURE_ASSET_ID::ENEMY_POISON,
                         EFFECT_ASSET_ID::HELP_SCREEN,
                         GEOMETRY_BUFFER_ID::SPRITE});
            } else if (move.nameAsString == "PUNCH") {
                registry.renderRequests.insert(
                        entity,
                        {TEXTURE_ASSET_ID::ENEMY_PUNCH,
                         EFFECT_ASSET_ID::HELP_SCREEN,
                         GEOMETRY_BUFFER_ID::SPRITE});
            } else if (move.nameAsString == "QUICKSTEP") {
                registry.renderRequests.insert(
                        entity,
                        {TEXTURE_ASSET_ID::ENEMY_QUICKSTEP,
                         EFFECT_ASSET_ID::HELP_SCREEN,
                         GEOMETRY_BUFFER_ID::SPRITE});
            } else if (move.nameAsString == "SCRATCH") {
                registry.renderRequests.insert(
                        entity,
                        {TEXTURE_ASSET_ID::ENEMY_SCRATCH,
                         EFFECT_ASSET_ID::HELP_SCREEN,
                         GEOMETRY_BUFFER_ID::SPRITE});
            } else if (move.nameAsString == "SHIELD") {
                registry.renderRequests.insert(
                        entity,
                        {TEXTURE_ASSET_ID::ENEMY_SHIELD,
                         EFFECT_ASSET_ID::HELP_SCREEN,
                         GEOMETRY_BUFFER_ID::SPRITE});
            } else if (move.nameAsString == "SNEEZE") {
                registry.renderRequests.insert(
                        entity,
                        {TEXTURE_ASSET_ID::ENEMY_SNEEZE,
                         EFFECT_ASSET_ID::HELP_SCREEN,
                         GEOMETRY_BUFFER_ID::SPRITE});
            } else if (move.nameAsString == "SPLASH") {
                    registry.renderRequests.insert(
                            entity,
                            {TEXTURE_ASSET_ID::ENEMY_SPLASH,
                             EFFECT_ASSET_ID::HELP_SCREEN,
                             GEOMETRY_BUFFER_ID::SPRITE});
            } else if (move.nameAsString == "TAUNT") {
                registry.renderRequests.insert(
                        entity,
                        {TEXTURE_ASSET_ID::ENEMY_TAUNT,
                         EFFECT_ASSET_ID::HELP_SCREEN,
                         GEOMETRY_BUFFER_ID::SPRITE});
            } else if (move.nameAsString == "THORNS") {
                registry.renderRequests.insert(
                        entity,
                        {TEXTURE_ASSET_ID::ENEMY_THORNS,
                         EFFECT_ASSET_ID::HELP_SCREEN,
                         GEOMETRY_BUFFER_ID::SPRITE});
            } else if (move.nameAsString == "TRIP") {
                registry.renderRequests.insert(
                        entity,
                        {TEXTURE_ASSET_ID::ENEMY_TRIP,
                         EFFECT_ASSET_ID::HELP_SCREEN,
                         GEOMETRY_BUFFER_ID::SPRITE});
            }

        } else if (move.moveType == MoveType::ITEM) {
            registry.renderRequests.insert(
                    entity,
                    {TEXTURE_ASSET_ID::ENEMY_HEALTHPOTION,
                     EFFECT_ASSET_ID::HELP_SCREEN,
                     GEOMETRY_BUFFER_ID::SPRITE});
        }
    }
    else {
        registry.renderRequests.insert(
                entity,
                {TEXTURE_ASSET_ID::ENEMY_DARKNESS,
                 EFFECT_ASSET_ID::HELP_SCREEN,
                 GEOMETRY_BUFFER_ID::SPRITE});
    }
    return entity;
}

Entity createHealthbar(RenderSystem* renderer, vec2 pos, EnemyName name) {
    auto exterior = Entity();
    auto hp = Entity();
    Mesh& mesh = renderer->getMesh(GEOMETRY_BUFFER_ID::SPRITE);
    registry.meshPtrs.emplace(exterior, &mesh);
    registry.meshPtrs.emplace(hp, &mesh);
    Motion& exterior_motion = registry.motions.emplace(exterior);
    exterior_motion.dir = Direction::DOWN;
    exterior_motion.position = pos ;
    exterior_motion.scale = mesh.original_size * 400.f;
    exterior_motion.collision_proof = 1;
    Motion& hp_motion = registry.motions.emplace(hp);
    hp_motion.dir = Direction::DOWN;
    hp_motion.position = { pos.x + 13, pos.y - 60 };
    hp_motion.scale = mesh.original_size * 100.f;
    hp_motion.scale = { hp_motion.scale.x * 2.2, hp_motion.scale.y * 0.3 };
#ifdef __APPLE__
    hp_motion.position = { pos.x - 96, pos.y - 80 };
#endif
    hp_motion.collision_proof = 1;
    registry.healthbars.emplace(exterior);
    registry.healthbars.emplace(hp);

    if (name == EnemyName::ENEMY_NAME_COUNT) {
        // This is the players healthbar so we do some additional calculations for mana
        hp_motion.position = { hp_motion.position.x, hp_motion.position.y + 25 };
#ifdef __APPLE__
        hp_motion.position = { hp_motion.position.x - 5, hp_motion.position.y - 20};
#endif
        hp_motion.scale = { hp_motion.scale.x * 1.03, hp_motion.scale.y * 2.5 };

        registry.renderRequests.insert(
            hp,
            { TEXTURE_ASSET_ID::TEXTURE_COUNT,
             EFFECT_ASSET_ID::HEALTHBAR_PLAYER,
             GEOMETRY_BUFFER_ID::SPRITE });
        registry.renderRequests.insert(
            exterior,
            { TEXTURE_ASSET_ID::HEALTHBAR_ANNA,
             EFFECT_ASSET_ID::HELP_SCREEN,
             GEOMETRY_BUFFER_ID::SPRITE });
    }
    else if (name == EnemyName::DUST_BUNNY) {
        registry.renderRequests.insert(
            hp,
            { TEXTURE_ASSET_ID::TEXTURE_COUNT,
             EFFECT_ASSET_ID::HEALTHBAR_ENEMY,
             GEOMETRY_BUFFER_ID::SPRITE });

        registry.renderRequests.insert(
            exterior,
            { TEXTURE_ASSET_ID::HEALTHBAR_DUSTBUNNY,
             EFFECT_ASSET_ID::HELP_SCREEN,
             GEOMETRY_BUFFER_ID::SPRITE });
    }
    else if (name == EnemyName::TEDDY_BEAR) {
        registry.renderRequests.insert(
            hp,
            { TEXTURE_ASSET_ID::TEXTURE_COUNT,
             EFFECT_ASSET_ID::HEALTHBAR_ENEMY,
             GEOMETRY_BUFFER_ID::SPRITE });

        registry.renderRequests.insert(
            exterior,
            { TEXTURE_ASSET_ID::HEALTHBAR_TEDDYBEAR,
             EFFECT_ASSET_ID::HELP_SCREEN,
             GEOMETRY_BUFFER_ID::SPRITE });
    }
    else if (name == EnemyName::FLORA_1) {
        registry.renderRequests.insert(
            hp,
            { TEXTURE_ASSET_ID::TEXTURE_COUNT,
             EFFECT_ASSET_ID::HEALTHBAR_ENEMY,
             GEOMETRY_BUFFER_ID::SPRITE });

        registry.renderRequests.insert(
            exterior,
            { TEXTURE_ASSET_ID::HEALTHBAR_FLORA1,
             EFFECT_ASSET_ID::HELP_SCREEN,
             GEOMETRY_BUFFER_ID::SPRITE });
    }
    else if (name == EnemyName::FLORA_2) {
        registry.renderRequests.insert(
            hp,
            { TEXTURE_ASSET_ID::TEXTURE_COUNT,
             EFFECT_ASSET_ID::HEALTHBAR_ENEMY,
             GEOMETRY_BUFFER_ID::SPRITE });

        registry.renderRequests.insert(
            exterior,
            { TEXTURE_ASSET_ID::HEALTHBAR_FLORA2,
             EFFECT_ASSET_ID::HELP_SCREEN,
             GEOMETRY_BUFFER_ID::SPRITE });
    }

    return exterior;
}

Entity createDoll(RenderSystem* renderer, vec2 pos, vec2 frameBufferSize, const std::vector<Attack>& attackList, std::vector<Magic> magicList, std::vector<GameItem> itemList, int hp, int hd, int maxHP, int mp, int maxMana)
{
    auto entity = Entity();

    // Store a reference to the potentially re-used mesh object
    Mesh &mesh = renderer->getMesh(GEOMETRY_BUFFER_ID::SPRITE);
    registry.meshPtrs.emplace(entity, &mesh);

    // Setting initial motion values
    Motion &motion = registry.motions.emplace(entity);
    motion.dir = Direction::DOWN;
    motion.position = pos;
    motion.angle = 0.f;
    motion.velocity = {0.f, 0.f};
    motion.scale = mesh.original_size * float(120);

    Health &health = registry.health.emplace(entity);
    health.health = hp;
    health.healthDecrement = hd;
    health.maxHP = maxHP;

    Mana& mana = registry.mana.emplace(entity);
    mana.mana = mp;
    mana.maxMana = maxMana;

    // Give the player an attacks list, magic list, and add some default moves
    AttackList &player_attacks = registry.attackLists.emplace(entity);
    for (Attack a : attackList)
    {
        player_attacks.addAttack(a);
    }

    MagicList &player_magic = registry.magicLists.emplace(entity);
    for (Magic m : magicList)
    {
         player_magic.addMagic(m);
    }

    // TODO: Add new attacks and magic gradually
    //player_attacks.addAttack(AttackName::TRIP, AttackType::NORMAL, 0.05);

    // Testing can remove or gradually given as game progresses - PL
    player_magic.addMagicAttack(MagicName::LIGHTNING_STRIKE, AttackType::NORMAL, 30, 10);
    player_magic.addMagicEffect(MagicName::SHIELD, true, 2, 20, 0);
    player_magic.addMagicAttack(MagicName::SPLASH, AttackType::NORMAL, 20, 5);
    //player_magic.addMagicEffect(MagicName::POISON_CLOUD, true, 3, 5);
    //player_magic.addMagicEffect(MagicName::TAUNT, true, 2, 0.3);
    player_magic.addMagicAttack(MagicName::FIREBALL, AttackType::NORMAL, 15, 5);
    player_magic.addMagicAttack(MagicName::DARKNESS, AttackType::NOTNORMAL, 0.08, 5);
    //player_magic.addMagicEffect(MagicName::THORNS, true, 2, 0.3, 18);
    player_magic.addMagicDefense(MagicName::ARMOR_UP, 15, 0, 6, 0, 2);
    //player_magic.addMagicDefense(MagicName::MAGIC_RESIST_UP, 0, 15, 6, 0);
    //player_magic.addMagicDefense(MagicName::MIND_AND_BODY, 10, 10, 6, 0);
    //player_magic.addMagicDefense(MagicName::QUICK_STEP, 0, 0, 3, 20);


    Game &game = registry.game.emplace(entity);
    game.state = GameState::PLAYING;
    registry.players.insert(entity, {(int)frameBufferSize.x, (int)frameBufferSize.y});
    AnimatedSprite &animated_sprite = registry.animatedSprites.emplace(entity);
    animated_sprite.paused = true;
    animated_sprite.num_frames_per_type = {{0, 4}, {1, 4}, {2, 4}, {3, 4}};

    Inventory& player_inventory = registry.inventories.emplace(entity);
    player_inventory.MAX_ITEMS = 6;
    registry.mementoInventories.emplace(entity);
    assert(itemList.size() <= player_inventory.MAX_ITEMS);
    for (GameItem item : itemList) {
        player_inventory.items.push_back(item);
    }

    registry.statusEffects.emplace(entity);
    registry.renderRequests.insert(
        entity,
        {TEXTURE_ASSET_ID::DOLL,
         EFFECT_ASSET_ID::TEXTURED_ANIMATION,
         GEOMETRY_BUFFER_ID::SPRITE_ANIMATION});

    return entity;
}

Entity createBattleDoll(RenderSystem *renderer, vec2 pos)
{
    auto entity = Entity();
    Mesh &mesh = renderer->getMesh(GEOMETRY_BUFFER_ID::SPRITE_ANIMATION);
    registry.meshPtrs.emplace(entity, &mesh);
    Motion &motion = registry.motions.emplace(entity);
    motion.dir = Direction::DOWN;
    motion.position = pos;
    motion.angle = 0.f;
    motion.velocity = {0.f, 0.f};
    motion.scale = {100, 100};
    motion.collision_proof = 1;
    registry.battleDolls.emplace(entity);
    AnimatedSprite &animated_sprite = registry.animatedSprites.emplace(entity);
    animated_sprite.paused = false;
    animated_sprite.num_frames_per_type = {{0, 2}};
    animated_sprite.animation_speed_ms = 750;

    registry.renderRequests.insert(
        entity,
        {TEXTURE_ASSET_ID::DOLL,
         EFFECT_ASSET_ID::TEXTURED_ANIMATION,
         GEOMETRY_BUFFER_ID::SPRITE_ANIMATION});
    return entity;
}

Entity createEnemy(RenderSystem *renderer, vec2 pos, EnemyName name)
{
    auto entity = Entity();

    // Store a reference to the potentially re-used mesh object
    Mesh &mesh = renderer->getMesh(GEOMETRY_BUFFER_ID::SPRITE);
    registry.meshPtrs.emplace(entity, &mesh);

    // Setting initial motion values
    Motion &motion = registry.motions.emplace(entity);
    motion.dir = Direction::DOWN;
    motion.position = pos;
    motion.angle = 0.f;
    motion.velocity = {0.f, 0.f};
    motion.scale = mesh.original_size;
    motion.scale.x *= -1; // point front to the right

    Health &health = registry.health.emplace(entity);
    health.health = 80.f;
    health.maxHP = 80.f;
    health.healthDecrement = 0;

    Mana& mana = registry.mana.emplace(entity);
    mana.mana = 70.f;
    mana.maxMana = 70.f;

    // Give it a status
    registry.statusEffects.emplace(entity);

    AttackList &enemy_attacks = registry.attackLists.emplace(entity);
    GameItem& item = registry.gameItems.emplace(entity);
    MagicList& magic = registry.magicLists.emplace(entity);

    // Create an enemy
    Enemy& enemy = registry.enemies.emplace(entity);
    registry.AIEntities.emplace(entity);

    // Animation Handle
    AnimatedSprite& animated_sprite = registry.animatedSprites.emplace(entity);
    animated_sprite.paused = false;
    animated_sprite.num_frames_per_type = { {0, 2}, {1, 4} };
    animated_sprite.animation_speed_ms = 750;


    if (name == EnemyName::DUST_BUNNY) {
        enemy.name = name;
        animated_sprite.current_type = 0;
        registry.bunnies.emplace(entity);

        health.health = 50.f;
        health.maxHP = 50.f;

        mana.mana = 50.f;
        mana.maxMana = 70.f;

        // One attack move: Sneeze
        enemy_attacks.addAttack(AttackName::SNEEZE, AttackType::NORMAL, 5);

        // One Healing Potion
        item.item_name = GameItemName::HEALING_POTION;
        item.health = 0.25 * health.health;

        // One status effect debuff
        magic.addMagicEffect(MagicName::TAUNT, true, 2, 0.3, 10);

        // One defense effect buff
        magic.addMagicEffect(MagicName::SHIELD, true, 2, 16, 0);



        // IN ORDER TO ADD THE MOVES BELOW WE NEED TO ASSIGN THEM MAGIC COSTS (last param to .addMagic___) I JUST DONT KNOW WHAT THEY ARE RN - Liam

       /* enemy_attacks.addAttack(AttackName::SCRATCH, AttackType::NORMAL, 8);
        enemy_attacks.addAttack(AttackName::TRIP, AttackType::NORMAL, 0.05);
        magic.addMagicAttack(MagicName::LIGHTNING_STRIKE, AttackType::NORMAL, 30);
        magic.addMagicAttack(MagicName::FIREBALL, AttackType::NORMAL, 15);
        magic.addMagicAttack(MagicName::SPLASH, AttackType::NORMAL, 7);
        magic.addMagicAttack(MagicName::DARKNESS, AttackType::NOTNORMAL, 0.08);
        magic.addMagicEffect(MagicName::THORNS, true, 2, 0.3);
        magic.addMagicEffect(MagicName::POISON_CLOUD, TRUE, 3, 5);
        magic.addMagicDefense(MagicName::ARMOR_UP, 15, 0, 6, 0);
        magic.addMagicDefense(MagicName::MAGIC_RESIST_UP, 0, 15, 6, 0);
        magic.addMagicDefense(MagicName::MIND_AND_BODY, 10, 10, 6, 0);
         magic.addMagicDefense(MagicName::QUICK_STEP, 0, 0, 3, 20);
       */

    }
    if (name == EnemyName::TEDDY_BEAR) {
        enemy.name = name;
        registry.bears.emplace(entity);

        health.health = 70.f;
        health.maxHP = 70.f;

        mana.mana = 70.f;
        mana.maxMana = 80.f;

        animated_sprite.current_type = 1;
        enemy_attacks.addAttack(AttackName::SCRATCH, AttackType::NORMAL, 8);
        item.item_name = GameItemName::HEALING_POTION;
        item.health = 0.25 * health.health;
        magic.addMagicEffect(MagicName::SHIELD, true, 2, 16, 0);
        magic.addMagicEffect(MagicName::THORNS, true, 2, 0.3, 18);
        magic.addMagicAttack(MagicName::FIREBALL, AttackType::NORMAL, 15, 25);
        magic.addMagicAttack(MagicName::DARKNESS, AttackType::NOTNORMAL, 0.08, 7);
        magic.addMagicDefense(MagicName::ARMOR_UP, 15, 0, 6, 0, 12);
        magic.addMagicDefense(MagicName::MAGIC_RESIST_UP, 0, 15, 6, 0, 12);
        magic.addMagicDefense(MagicName::QUICK_STEP, 0, 0, 3, 20, 15);
    }

    Behaviour& node = registry.behaviours.emplace(entity);

    registry.renderRequests.insert(
        entity,
        {TEXTURE_ASSET_ID::ENEMIES,
         EFFECT_ASSET_ID::TEXTURED_ANIMATION,
         GEOMETRY_BUFFER_ID::SPRITE_ANIMATION});

    return entity;
}

Entity createBoss(RenderSystem *renderer, int flora) {
    auto entity = Entity();

    // Store a reference to the potentially re-used mesh object
    Mesh &mesh = renderer->getMesh(GEOMETRY_BUFFER_ID::SPRITE);
    registry.meshPtrs.emplace(entity, &mesh);

    // Setting initial motion values
    Motion &motion = registry.motions.emplace(entity);
    motion.dir = Direction::DOWN;
    motion.position = {0.f,0.f};
    motion.angle = 0.f;
    motion.velocity = {0.f, 0.f};
    motion.scale = mesh.original_size;
    motion.scale.x *= -1; // point front to the right
    // Give it a status
    registry.statusEffects.emplace(entity);

    Health& health = registry.health.emplace(entity);
    health.health = 150;
    health.maxHP = 150;

    Mana& mana = registry.mana.emplace(entity);
    mana.mana = 100;
    mana.maxMana = 100;

    AttackList &enemy_attacks = registry.attackLists.emplace(entity);
    Inventory &enemy_inventory = registry.inventories.emplace(entity);
    MagicList &magic = registry.magicLists.emplace(entity);

    // Manually add boss attacks/magic/inventory below instead of in JSON
    enemy_attacks.addAttack(createAttack((AttackName)1, (AttackType)0, 10));
    enemy_attacks.addAttack(createAttack((AttackName)3, (AttackType)0, 10));
    magic.addMagic(createMagic((MagicName)1, (MagicType)0, (AttackType)0, 15, 0, 0, 0, 0, false));
    magic.addMagic(createMagic((MagicName)7, (MagicType)0, (AttackType)0, 10, 0, 0, 0, 0, false));
    magic.addMagic(createMagic((MagicName)3, (MagicType)2, (AttackType)0, 0, 0, 0, 3, 3, true));
    enemy_inventory.addItem(createGameItem((GameItemName)1, false, 25, 50, 0, false, 0, 0));

    Enemy& enemy = registry.enemies.emplace(entity);

    // Create an enemy
    if (flora == 1) {
        enemy.name = EnemyName::FLORA_1;
        registry.bosses.emplace(entity);
    }
    else {
        enemy.name = EnemyName::FLORA_2;
    }

    registry.currentEnemies.emplace(entity);
    // Animation Handle
    AnimatedSprite &animated_sprite = registry.animatedSprites.emplace(entity);
    animated_sprite.paused = false;
    animated_sprite.num_frames_per_type = {{0, 2},
                                           {1, 4}};
    animated_sprite.animation_speed_ms = 750;

    return entity;
}

Entity createTestEnemy(vec2 pos)
{
    auto entity = Entity();

    Motion& motion = registry.motions.emplace(entity);
    motion.position = pos;
    motion.scale = { 64.f, 64.f };

    return entity;
}


Entity createBattleEnemy(RenderSystem *renderer, vec2 pos, EnemyName name)
{
    auto entity = Entity();
    // Store a reference to the potentially re-used mesh object
    Mesh &mesh = renderer->getMesh(GEOMETRY_BUFFER_ID::SPRITE_ANIMATION);
    registry.meshPtrs.emplace(entity, &mesh);

    // Setting initial motion values
    Motion &motion = registry.motions.emplace(entity);
    motion.dir = Direction::DOWN;
    motion.position = pos + vec2(0, 25);
    motion.angle = 0.f;
    motion.velocity = { 0.f, 0.f };
    motion.collision_proof = 1;
    //    motion.scale.x *= -1; // point front to the right
    registry.battleEnemies.emplace(entity);

    if (name == EnemyName::DUST_BUNNY || name == EnemyName::TEDDY_BEAR) {
        AnimatedSprite& animated_sprite = registry.animatedSprites.emplace(entity);
        animated_sprite.paused = false;
        animated_sprite.num_frames_per_type = { {0, 2}, {1, 4} };
        animated_sprite.animation_speed_ms = 750;


        if (name == EnemyName::DUST_BUNNY) {
            animated_sprite.current_type = 0;
            motion.scale = { 150, 150 };
        }
        else if (name == EnemyName::TEDDY_BEAR) {
            animated_sprite.current_type = 1;
            motion.scale = { 50, 50 };
        }

        registry.renderRequests.insert(
            entity,
            { TEXTURE_ASSET_ID::ENEMIES,
             EFFECT_ASSET_ID::TEXTURED_ANIMATION,
             GEOMETRY_BUFFER_ID::SPRITE_ANIMATION });
    }

    if (name == EnemyName::FLORA_1) {
        motion.scale = { 100, 100 };

        registry.renderRequests.insert(
            entity,
            { TEXTURE_ASSET_ID::FLORA_NORMAL,
             EFFECT_ASSET_ID::TEXTURED,
             GEOMETRY_BUFFER_ID::SPRITE });
    }

    return entity;
}

Entity createWallBlock(vec2 pos, vec2 scale)
{
    auto entity = Entity();

    // Setting initial motion values
    Motion &motion = registry.motions.emplace(entity);
    motion.position = pos;
    motion.scale = scale;
    motion.velocity = {0.f, 0.f};

    registry.walls.emplace(entity);
    return entity;
}

Entity createBed(RenderSystem *renderer, vec2 pos, Direction dir, float angle)
{
    auto entity = Entity();


    // Store a reference to the potentially re-used mesh object
    Mesh &mesh = renderer->getMesh(GEOMETRY_BUFFER_ID::SPRITE);
    registry.meshPtrs.emplace(entity, &mesh);

    // Setting initial motion values
    Motion &motion = registry.motions.emplace(entity);
    motion.dir = dir;
    motion.position = pos;
    motion.angle = angle;
    motion.velocity = {0.f, 0.f};
    motion.scale = mesh.original_size;

    registry.renderRequests.insert(
        entity,
        {TEXTURE_ASSET_ID::BED,
         EFFECT_ASSET_ID::TEXTURED,
         GEOMETRY_BUFFER_ID::SPRITE});

    return entity;
}

Entity createTable(RenderSystem *renderer, vec2 pos, Direction dir, float angle)
{
    auto entity = Entity();

    // Store a reference to the potentially re-used mesh object
    Mesh &mesh = renderer->getMesh(GEOMETRY_BUFFER_ID::SPRITE);
    registry.meshPtrs.emplace(entity, &mesh);

    // Setting initial motion values
    Motion &motion = registry.motions.emplace(entity);
    motion.dir = dir;
    motion.position = pos;
    motion.angle = angle;
    motion.velocity = {0.f, 0.f};
    motion.scale = {118.0f, 74.0f};
    motion.collision_proof = false;
    registry.walls.emplace(entity);

    registry.renderRequests.insert(
        entity,
        {TEXTURE_ASSET_ID::TABLE,
         EFFECT_ASSET_ID::TEXTURED,
         GEOMETRY_BUFFER_ID::SPRITE});

    return entity;
}

Entity createHouse(RenderSystem *renderer, vec2 pos, Direction dir, float angle)
{
    auto entity = Entity();

    // Store a reference to the potentially re-used mesh object
    Mesh &mesh = renderer->getMesh(GEOMETRY_BUFFER_ID::SPRITE);
    registry.meshPtrs.emplace(entity, &mesh);

    // Setting initial motion values
    Motion &motion = registry.motions.emplace(entity);
    motion.dir = dir;
    motion.position = pos;
    motion.angle = angle;
    motion.velocity = {0.f, 0.f};
    motion.scale = {118.0f, 74.0f};
    motion.collision_proof = false;
    registry.walls.emplace(entity);

    registry.renderRequests.insert(
            entity,
            {TEXTURE_ASSET_ID::TABLE, // TODO: change id make house
             EFFECT_ASSET_ID::TEXTURED,
             GEOMETRY_BUFFER_ID::SPRITE});

    return entity;
}

Entity createLamp(RenderSystem *renderer, vec2 pos, Direction dir, float angle)
{
    auto entity = Entity();

    // TODO: fix rendering
    // Store a reference to the potentially re-used mesh object
    Mesh &mesh = renderer->getMesh(GEOMETRY_BUFFER_ID::SPRITE);
    registry.meshPtrs.emplace(entity, &mesh);

    // Setting initial motion values
    Motion &motion = registry.motions.emplace(entity);
    motion.dir = dir;
    motion.position = pos;
    motion.angle = angle;
    motion.velocity = {0.f, 0.f};
    motion.scale = mesh.original_size;

    registry.renderRequests.insert(
        entity,
        {TEXTURE_ASSET_ID::DOLL, // TODO: Change this
         EFFECT_ASSET_ID::TEXTURED,
         GEOMETRY_BUFFER_ID::SPRITE});

    return entity;
}

Entity createDoor(RenderSystem* renderer, vec2 pos, vec2 scale, TEXTURE_ASSET_ID asset_id) {
    auto entity = Entity();

    Mesh& mesh = renderer->getMesh(GEOMETRY_BUFFER_ID::SPRITE);
    registry.meshPtrs.emplace(entity, &mesh);

    Motion& motion = registry.motions.emplace(entity);
    motion.dir = Direction::DOWN;
    motion.position = pos;
    motion.velocity = { 0.f, 0.f };
    motion.angle = 0;
    motion.scale = scale;
    motion.collision_proof = false;

    registry.doors.emplace(entity);
    // registry.walls.emplace(entity); // Might want to handle its collision separately or on top of this
    registry.renderRequests.insert(
        entity,
        {
            asset_id,
            EFFECT_ASSET_ID::TEXTURED,
            GEOMETRY_BUFFER_ID::SPRITE,
        }
    );

    return entity;
}

Entity createHealthPotion(RenderSystem* renderer, int world_id, vec2 pos, GameItemName potion_name) {
    assert(potion_name == GameItemName::HEALING_POTION || potion_name == GameItemName::HEALING_POTION_2);
    auto entity = Entity();

    Mesh& mesh = renderer->getMesh(GEOMETRY_BUFFER_ID::SPRITE);
    registry.meshPtrs.emplace(entity, &mesh);

    Motion& motion = registry.motions.emplace(entity);
    motion.dir = Direction::DOWN;
    motion.position = pos;
    motion.velocity = { 0.f, 0.f };
    motion.angle = 0;
    motion.scale = { 54.0f , 62.0f };
    motion.collision_proof = false;

    GameItem& game_item = registry.gameItems.emplace(entity);
    game_item.world_id = world_id;
    game_item.item_name = potion_name;
    game_item.health = potion_name == GameItemName::HEALING_POTION ? 50 : 75;
    game_item.mana = potion_name == GameItemName::HEALING_POTION ? 0 : 50;

    TEXTURE_ASSET_ID asset_id =
        potion_name == GameItemName::HEALING_POTION ?
        TEXTURE_ASSET_ID::HEALTH_POTION_MINI :
        TEXTURE_ASSET_ID::HEALTH_POTION_MEGA;

    registry.renderRequests.insert(
        entity,
        {
            asset_id,
            EFFECT_ASSET_ID::TEXTURED,
            GEOMETRY_BUFFER_ID::SPRITE
        }
    );

    return entity;
}

Entity createMemento(RenderSystem* renderer, MementoName id, vec2 pos) {
    auto entity = Entity();

    Mesh& mesh = renderer->getMesh(GEOMETRY_BUFFER_ID::SPRITE);
    registry.meshPtrs.emplace(entity, &mesh);

    Motion& motion = registry.motions.emplace(entity);
    motion.dir = Direction::DOWN;
    motion.position = pos;
    motion.velocity = { 0.f, 0.f };
    motion.angle = 0;
    motion.scale = { 100.0f , 100.0f };
    motion.collision_proof = false;

    MementoName& memento = registry.mementos.emplace(entity);
    memento = id;

    TEXTURE_ASSET_ID asset_id = TEXTURE_ASSET_ID::TEXTURE_COUNT;

    if (memento == MementoName::PIG) {
        asset_id = TEXTURE_ASSET_ID::MEMENTO_OBJECT_1;
    } else if (memento == MementoName::CUPCAKES) {
        asset_id = TEXTURE_ASSET_ID::MEMENTO_OBJECT_2;
    } else if (memento == MementoName::BRUSH) {
        asset_id = TEXTURE_ASSET_ID::MEMENTO_OBJECT_3;
    } else if (memento == MementoName::REMOTE) {
        asset_id = TEXTURE_ASSET_ID::MEMENTO_OBJECT_4;
    } else if (memento == MementoName::BIKE) {
        asset_id = TEXTURE_ASSET_ID::MEMENTO_OBJECT_5;
    } else if (memento == MementoName::FLOWER) {
        asset_id = TEXTURE_ASSET_ID::MEMENTO_OBJECT_6;
    } else if (memento == MementoName::TEA) {
        asset_id = TEXTURE_ASSET_ID::MEMENTO_OBJECT_7;
    } else if (memento == MementoName::DRAWING) {
        asset_id = TEXTURE_ASSET_ID::MEMENTO_OBJECT_8;
    }

    registry.renderRequests.insert(
        entity,
        {
            asset_id,
            EFFECT_ASSET_ID::TEXTURED,
            GEOMETRY_BUFFER_ID::SPRITE
        }
    );

    return entity;
}

Entity createFlora(RenderSystem* renderer, vec2 pos) {
    auto entity = Entity();

    Mesh& mesh = renderer->getMesh(GEOMETRY_BUFFER_ID::SPRITE);
    registry.meshPtrs.emplace(entity, &mesh);

    Motion& motion = registry.motions.emplace(entity);
    motion.dir = Direction::DOWN;
    motion.position = pos;
    motion.velocity = { 0.f, 0.f };
    motion.scale = { 100.f, 100.f };
    motion.angle = 0;
    motion.collision_proof = false;


    registry.floras.emplace(entity);
    registry.renderRequests.insert(
            entity,
            {
                    TEXTURE_ASSET_ID::FLORA_NORMAL,
                    EFFECT_ASSET_ID::TEXTURED,
                    GEOMETRY_BUFFER_ID::SPRITE
            }
    );

    return entity;
}

Entity createAnimatronic(RenderSystem* renderer, TEXTURE_ASSET_ID id, vec2 pos, bool collidable) {
    auto animatronicPart = Entity();

    Mesh& mesh = renderer->getMesh(GEOMETRY_BUFFER_ID::SPRITE);
    registry.meshPtrs.emplace(animatronicPart, &mesh);

    Motion& motion = registry.motions.emplace(animatronicPart);
    motion.dir = Direction::DOWN;
    motion.position = pos;
    motion.velocity = { 0.f, 0.f };
    motion.angle = 0;

    if (id == TEXTURE_ASSET_ID::ANIMATRONIC_TORSO) {
        motion.scale = { 200.0f , 200.0f };
    }
    else {
        motion.scale = { 100.0f , 100.0f };
    }
    motion.collision_proof = false;

    if (collidable) { // only adds legs to bosses registry
        Boss& boss = registry.bosses.emplace(animatronicPart);
        Enemy& enemy = registry.enemies.emplace(animatronicPart);
        enemy.name = EnemyName::FLORA_2;
    }

    registry.renderRequests.insert(
        animatronicPart,
        {
                id,
                EFFECT_ASSET_ID::TEXTURED,
                GEOMETRY_BUFFER_ID::SPRITE
        }
    );

    return animatronicPart;
}

Attack createAttack(AttackName name, AttackType type, int damage)
{
    Attack a = {};
    a.name = name;
    a.type = type;
    a.damage = damage;
    return a;
}

Magic createMagic(MagicName mn, MagicType mt, AttackType at, float amount, float physical_defense_boost, float magic_defense_boost, int timer, int countdown, bool isTemporary) {
    Magic m = {};
    m.name = mn;
    m.magicType = mt;
    m.timer = timer;
    m.isTemporary = isTemporary;
    m.magic_defense_boost = magic_defense_boost;
    m.physical_defense_boost = physical_defense_boost;
    m.attack_type = at;
    m.amount = amount;
    m.countdown = countdown;
    return m;
}

GameItem createGameItem(GameItemName name, bool enemyRepel, int health, int mana, int speed, bool timed, float timer, int world_id) {
    GameItem item = {};
    item.item_name = name;
    item.enemyRepel = enemyRepel;
    item.health = health;
    item.mana = mana;
    item.speed = speed;
    item.timed = timed;
    item.timer = timer;
    item.world_id = world_id;

    return item;
}

Motion createMotion(vec2 scale, vec2 position, vec2 velocity, float angle, Direction dir, bool collision_proof)
{
    Motion m;
    m.scale = scale;
    m.position = position;
    m.velocity = velocity;
    m.angle = angle;
    m.dir = dir;
    m.collision_proof = collision_proof;
    return m;
}

Health createHealth(int health, int healthDecrement, int maxHP)
{
    Health h;
    h.health = health;
    h.healthDecrement = healthDecrement;
    h.maxHP = maxHP;
    return h;
}

Mana createMana(int mana, int maxMana) {
    Mana m;
    m.mana = mana;
    m.maxMana = maxMana;
    return m;
}

GameProgress createGameProgress(int level, Health health, Mana mana, Motion motion, std::vector<Attack> attacks, std::vector<Magic> magic, std::vector<GameItem> inventory_items, std::vector<int> all_collected_item_ids, std::vector<int> collected_memento_ids)
{
    GameProgress g;
    g.level = level;
    g.health = health;
    g.mana = mana;
    g.motion = motion;
    g.attacks = attacks;
    g.magic = magic;
    g.attacks = attacks;
    g.inventory_items = inventory_items;
    g.all_collected_item_ids = all_collected_item_ids;
    g.collected_memento_ids = collected_memento_ids;
    return g;
}
//Entity createClickableArea(vec2 topLeft, int width, int height) {
//    auto entity = Entity();
//    ClickableArea& ca = registry.clickableArea.emplace(entity);
//    ca.topLeft = topLeft;
//    ca.width = width;
//    ca.height = height;
//    return entity;
//}

Entity createLine(vec2 position, vec2 scale)
{
    Entity entity = Entity();

    // Store a reference to the potentially re-used mesh object (the value is stored in the resource cache)
    registry.renderRequests.insert(
        entity,
        {TEXTURE_ASSET_ID::TEXTURE_COUNT,
         EFFECT_ASSET_ID::PEBBLE,
         GEOMETRY_BUFFER_ID::DEBUG_LINE});
    // Create motion
    Motion &motion = registry.motions.emplace(entity);
    motion.angle = 0.f;
    motion.velocity = {0, 0};
    motion.position = position;
    motion.scale = scale;
    registry.debugComponents.emplace(entity);
    return entity;
}

void createBox(vec2 pos, vec2 scale)
{
    createLine({ pos.x - (scale.x / 2.f), pos.y }, { 5.f, scale.y });
    createLine({ pos.x + (scale.x / 2.f), pos.y }, { 5.f, scale.y });
    createLine({ pos.x, pos.y - (scale.y / 2.f) }, { scale.x, 5.f });
    createLine({ pos.x, pos.y + (scale.y / 2.f) }, { scale.x, 5.f });
}

Entity createShadow(RenderSystem* renderer, vec2 pos, int num)
{
    auto entity = Entity();

    // Store a reference to the potentially re-used mesh object
    Mesh& mesh = renderer->getMesh(GEOMETRY_BUFFER_ID::SHADOW);
    registry.meshPtrs.emplace(entity, &mesh);

    // Setting initial motion values
    Motion& motion = registry.motions.emplace(entity);
    motion.position = pos;
    motion.angle = 0.f;
    motion.velocity = { 0.f, 0.f };
    motion.scale = mesh.original_size * 75.f;
    
    registry.colors.insert(entity, { 1.0f, 1.0f, 1.0f });
    Shadow& shadow = registry.shadow.emplace(entity);
    shadow.num = num;


    registry.renderRequests.insert(
        entity,
        { TEXTURE_ASSET_ID::TEXTURE_COUNT, // TEXTURE_COUNT indicates that no txture is needed
            EFFECT_ASSET_ID::SHADOW,
            GEOMETRY_BUFFER_ID::SHADOW });

    return entity;
}

// memberName is one of: "ROSALINE" "PETER" "HANNAH" "NAOREEN" "LIAM" "PREET"
Entity createTeamMember(RenderSystem* renderer, vec2 pos, std::string memberName)
{
    auto entity = Entity();
    Mesh& mesh = renderer->getMesh(GEOMETRY_BUFFER_ID::SPRITE_ANIMATION);
    registry.meshPtrs.emplace(entity, &mesh);
    Motion& motion = registry.motions.emplace(entity);
    motion.dir = Direction::DOWN;
    motion.position = pos;
    motion.angle = 0.f;
    motion.velocity = { 0.f, 0.f };
    motion.scale = { 150, 150 };
    motion.collision_proof = 1;
    AnimatedSprite& animated_sprite = registry.animatedSprites.emplace(entity);
    animated_sprite.paused = false;
    animated_sprite.num_frames_per_type = { {0, 4}, {1,4}, {2,4} };
    animated_sprite.animation_speed_ms = 500;

    TEXTURE_ASSET_ID spritesheet_asset_id =
        (memberName == "ROSALINE" || memberName == "HANNAH" || memberName == "NAOREEN") ?
        TEXTURE_ASSET_ID::TEAM_SPRITESHEET_1 :
        TEXTURE_ASSET_ID::TEAM_SPRITESHEET_2;

    animated_sprite.current_type =
        (memberName == "ROSALINE" || memberName == "PETER") ? 0 :
        (memberName == "HANNAH" || memberName == "PREET") ? 1 :
        2; // NAOREEN or LIAM
            

    registry.renderRequests.insert(
        entity,
        { spritesheet_asset_id,
         EFFECT_ASSET_ID::TEXTURED_ANIMATION,
         GEOMETRY_BUFFER_ID::SPRITE_ANIMATION });
    return entity;
}



/*
 Adding Attacks to entities: Example List to copy from and edit - PL
 ----------------------------------------------------------------------
 ----------------------------------------------------------------------

 AttackList &enemy_attacks = registry.attackLists.emplace(entity);
 GameItem& item = registry.gameItems.emplace(entity);
 MagicList& magic = registry.magicLists.emplace(entity);

 ----------------------------------------------------------------------
 Attack moves: 	void addAttack(AttackName name, AttackType attack_type, int damage, int manaCost)
 ----------------------------------------------------------------------

    SNEEZE : Dust Bunny's Special Base Move
 enemy_attacks.addAttack(AttackName::SNEEZE, AttackType::NORMAL, 5);

    SCRATCH : Teddy's Special Base Move
 enemy_attacks.addAttack(AttackName::SCRATCH, AttackType::NORMAL, 8);

    PUNCH : Player's Special Base Move
 player_attacks.addAttack(AttackName::SCRATCH, AttackType::NORMAL, 10);

    TRIP : You trip them, does a percentage amount of damage based on maximum HP
 enemy_attacks.addAttack(AttackName::TRIP, AttackType::NORMAL, 0.05);


 ----------------------------------------------------------------------
 ----------------------------------------------------------------------

 Game Items:
 ----------------------------------------------------------------------

    Healing Potion: Heals a percentage of your HP
 item.item_name = GameItemName::HEALING_POTION;
 item.health = 0.3 * health.health;

 ----------------------------------------------------------------------
 ----------------------------------------------------------------------
 Magic: Attacks : void addMagicAttack(MagicName name, AttackType attack_type, int damage, int manaCost)
 ----------------------------------------------------------------------
    LIGHTNING_STRIKE : Strikes three times, double damage against WET enemies, you will not shock yourself, removes WETNESS
 magic.addMagicAttack(MagicName::LIGHTNING_STRIKE, AttackType::NORMAL, 30, 25);

    FIREBALL : Has a chance to ignite, this will remove WETNESS for both parties, will do 30% less damage against wet
 magic.addMagicAttack(MagicName::FIREBALL, AttackType::NORMAL, 15, 20);

    SPLASH : Angry, you splash them with a cup of tea you materialize, this will remove IGNITE for both parties, this will apply WETNESS to the opponent
 magic.addMagicAttack(MagicName::SPLASH, AttackType::NORMAL, 7, 15);

     DARKNESS : You turn off the lights lowering their sanity, and do current health damage
 magic.addMagicAttack(MagicName::DARKNESS, AttackType::NOTNORMAL, 0.08, 15);


 ----------------------------------------------------------------------
 ----------------------------------------------------------------------
 Magic: Status effects: void addMagicEffect(MagicName name, bool isTemporary, float timer, int amount, int manaCost)
 ----------------------------------------------------------------------

    TAUNT : Will force the enemy to attacks using their base move, temporary
 magic.addMagicEffect(MagicName::TAUNT, true, 2, 0, 10);

    SHIELD : Gains a shield that blocks damage, temporary
 magic.addMagicEffect(MagicName::SHIELD, true, 2, 20, 15);

    THORNS : Gains a thorny exterior which damages enemies when they attacks you for a percentage of damage dealt to them, wears away when attacked
 magic.addMagicEffect(MagicName::THORNS, true, 2, 0.3, 10);

    POISON_CLOUD : Releases a cloud of poison to poison enemies, they take damage over time, temporary
 player_magic.addMagicEffect(MagicName::POISON_CLOUD, TRUE, 3, 5, 15);

 ----------------------------------------------------------------------
 ----------------------------------------------------------------------
 Magic: Defense effects: void addMagicDefense(MagicName name, int physical_defense_boost, int magic_defense_boost, int timer, int amount, int manaCost)
 ----------------------------------------------------------------------

    ARMOR_UP : Increases armor for self,  up to a maximum of 90, timer only set when 0, timer cannot stack/reset, armor can stack, temporary
 magic.addMagicDefense(MagicName::ARMOR_UP, 15, 0, 6, 0, 15);

     MAGIC_RESIST_UP : Increases magic resistance for self, up to a maximum of 90, timer only set when 0, timer cannot stack/reset, magic resistance can stack, temporary
 magic.addMagicDefense(MagicName::MAGIC_RESIST_UP, 0, 15, 6, 0, 15);

    MIND_AND_BODY : Increases both armor and magic resistance up to 90 each, timer only set when 0, timer cannot stack/reset, armor/magic resistance can stack, temporary
 magic.addMagicDefense(MagicName::MIND_AND_BODY, 10, 10, 6, 0, 30);
    
    QUICK_STEP : Increase one's speed incorporating a skip in one's step, making it harder to hit
 magic.addMagicDefense(MagicName::QUICK_STEP, 0, 0, 3, 20, 15);


*/

