#pragma once

#include "common.hpp"
#include "tiny_ecs.hpp"
#include "render_system.hpp"

// These are ahrd coded to the dimensions of the entity texture
const float DOLL_BB_WIDTH = 0.4f * 64.f;
const float DOLL_BB_HEIGHT = 0.4f * 64.f;
const float ENEMY_BB_WIDTH = 0.4f * 64.f;
const float ENEMY_BB_HEIGHT = 0.4f * 64.f;

Entity createBackground(RenderSystem *renderer, vec2 pos, TEXTURE_ASSET_ID background);

Entity createHelpWindow(RenderSystem *renderer, vec2 pos);

Entity createBattleWindow(RenderSystem *renderer, vec2 pos);

Entity createHealthbar(RenderSystem* renderer, vec2 pos, EnemyName name);

Entity createBattleParallaxWindow(RenderSystem *renderer, vec2 pos, TEXTURE_ASSET_ID textureAssetId);

Entity createBattleMenuItem(RenderSystem *renderer, vec2 pos, BattleMenuItemType item_type, TEXTURE_ASSET_ID texture_id);

Entity createBattleMenuPlayerMove(RenderSystem* renderer, vec2 pos, MoveType move_type, AttackName attack_name, MagicName magic_name, GameItemName item_name);

TEXTURE_ASSET_ID getBattleMenuPlayerMoveDefaultTexture(MoveType move_type, AttackName attack_name, MagicName magic_name, GameItemName item_name);

Entity createEnemy(RenderSystem *renderer, vec2 pos, EnemyName name);

Entity createBattleMoveUsed(RenderSystem *renderer, BattleMove move, BattleState battle_state);

Entity createTestEnemy(vec2 pos);

Entity createBattleDoll(RenderSystem *renderer, vec2 pos);

Entity createBattleEnemy(RenderSystem *renderer, vec2 pos, EnemyName name);

Entity createHelpWindow(RenderSystem *renderer, vec2 pos);

Entity createPopUpWindow(RenderSystem *renderer, vec2 pos);

Entity createMementoText(RenderSystem* renderer, vec2 pos, MementoName id);

Entity createMenuButton(RenderSystem *renderer, vec2 pos);

Entity createMenuOverlay(RenderSystem *renderer, vec2 pos);

Entity createTutorial(RenderSystem *renderer, vec2 pos);

Entity createStar(RenderSystem *renderer);

//Entity createHelpWindow(RenderSystem *renderer, vec2 pos);

Entity createWallBlock(vec2 pos, vec2 scale);

Entity createBed(RenderSystem *renderer, vec2 pos, Direction dir = Direction::DOWN, float angle = 0);

Entity createTable(RenderSystem *renderer, vec2 pos, Direction dir = Direction::DOWN, float angle = 0);

Entity createLamp(RenderSystem *renderer, vec2 pos, Direction dir = Direction::DOWN, float angle = 0);

Entity createDoor(RenderSystem* renderer, vec2 pos, vec2 scale, TEXTURE_ASSET_ID asset_id);

Entity createHealthPotion(RenderSystem* renderer, int world_id, vec2 pos, GameItemName potion_name);

Attack createAttack(AttackName name, AttackType type, int damage);

GameItem createGameItem(GameItemName name, bool enemyRepel, int health, int mana, int speed, bool timed, float timer, int world_id = 0);

Entity createMemento(RenderSystem* renderer, MementoName id, vec2 pos);

Entity createAnimatronic(RenderSystem* renderer, TEXTURE_ASSET_ID id, vec2 pos, bool collidable = false);

Entity createFlora(RenderSystem* renderer, vec2 pos);

GameProgress createGameProgress(int level, Health health, Mana mana, Motion motion, std::vector<Attack> attack, std::vector<Magic> magic, std::vector<GameItem> inventory_items, std::vector<int> all_collected_item_ids, std::vector<int> collected_memento_ids);

Health createHealth(int health = 100, int healthDecrement = 0, int maxHP = 100);

Mana createMana(int mana = 100,  int maxMana = 100);

Motion createMotion(vec2 scale, vec2 position, vec2 velocity, float angle, Direction dir, bool collision_proof);

//Entity createClickableArea(vec2 leftTop, int width, int height);

Entity createLine(vec2 position, vec2 scale);

void createBox(vec2 pos, vec2 scale);

Entity createDoll(RenderSystem *renderer, vec2 pos, vec2 frameBufferSize, const std::vector<Attack>& attackList, std::vector<Magic> magicList, std::vector<GameItem> itemList, int hp, int hd, int maxHP, int mp, int maxMana);

Entity createBoss(RenderSystem *renderer, int flora);

Magic createMagic(MagicName mn, MagicType mt, AttackType at, float amount, float physical_defense_boost, float magic_defense_boost, int timer, int countdown, bool isTemporary);

Entity createShadow(RenderSystem* renderer, vec2 pos, int num);

Entity createTeamMember(RenderSystem* renderer, vec2 pos, std::string memberName);

Entity createDoorLockTutorial(RenderSystem *renderer);

Entity createPreBossTutorial(RenderSystem *renderer);
