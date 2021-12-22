
#include "battle_system.hpp"
#include "world_init.hpp"
#include "world_system.hpp"
#include "job_system.h"

#include <iostream>
#include <chrono>

const int max_depth = 5;
float enemyMoveDelay = 1000.f;
float enemyBestMovePercent = 70; // percent of time the enemy picks the best move

void BattleSystem::init(WorldSystem *world_system_arg) {
    this->world_system = world_system_arg;
};

void resetStatus(StatusEffect &s) {
    s.TAUNTED = false;
    s.tauntTimer = 0;

    s.POISONED = false;
    s.poisonTimer = 0;
    s.poisonDamage = 0;

    s.SHIELDED = false;
    s.shield_amount = 0;
    s.shieldTimer = 0;

    s.THORNS = false;
    s.thorns_amount = 0;
    s.thornTimer = 0;

    s.IGNITED = false;
    s.ignite_damage = 0;
    s.ignitedTimer = 0;

    s.EVASION = 0;
    s.evasionTimer = 0;

    s.physical_defense = 0;
    s.physical_reduction = 0;
    s.physTimer = 0;

    s.magical_defense = 0;
    s.magical_reduction = 0;
    s.magiTimer = 0;

    s.WET = false;
}

void BattleSystem::handle_battle(float elapsed_ms) {
    Entity &player_doll = registry.players.entities[0];
    Game &game = registry.game.get(player_doll);
    if ((game.state != GameState::BATTLE && game.state != GameState::BOSS) ||
        registry.currentEnemies.entities.empty()) {
        return;
    }

    if (game.battle_state == BattleState::NONE) {
        // New battle was initiated so let's initialize relevant variables
        Entity &current_enemies = registry.currentEnemies.entities[0];
        Health &player_health = registry.health.get(player_doll);
        Mana& player_mana = registry.mana.get(player_doll);
        player_health.healthDecrement = 0;
        player_health.maxHP = 200;
        player_mana.maxMana = 100;
        StatusEffect &player_status = registry.statusEffects.get(player_doll);
        resetStatus(player_status);
        game.battle_state = BattleState::START;
        printf("=====Initialized battle=====\n");
    } else if (game.battle_state == BattleState::START) {
        game.battle_state = BattleState::PLAYER_TURN;
        if (game.state == GameState::BOSS) {
            advancedDecisionTree = getRoot(player_doll, registry.currentEnemies.entities[0]);
        }
    }

    else if (game.battle_state == BattleState::PLAYER_TURN && registry.turns.has(player_doll)) {
        Turn& player_turn = registry.turns.get(player_doll);
        if (player_turn.move_type == MoveType::MAGIC) {
            MagicList& player_attacks = registry.magicLists.get(player_doll);
            Magic chosen_attack = player_attacks.getAttack(player_turn.magic_name);
            Mana player_mana = registry.mana.get(player_doll);
            if (chosen_attack.manaCost > player_mana.mana) {
                printf("Not enough Mana for that!\n");
                BattleMove no_mana;
                no_mana.moveType = MoveType::MAGIC;
                no_mana.nameAsString = "NOT_ENOUGH_MANA";
                createBattleMoveUsed(world_system->getRenderer(), no_mana, BattleState::PLAYER_TURN);
                registry.turns.remove(player_doll);
                return;
            }
        }

        BattleMove chosen_move = process_player_turn(registry.turns.get(player_doll));
        if (chosen_move.nameAsString == "PUNCH" && game.state != GameState::BOSS && !registry.squished.has(registry.battleEnemies.entities[0])) {
            // add 'squish' component to enemy
            registry.squished.emplace(registry.battleEnemies.entities[0]);
        }

        Health& enemy_health = registry.health.get(registry.currentEnemies.entities[0]);
        Health& player_health = registry.health.get(player_doll);
        Mana& player_mana = registry.mana.get(player_doll);

        world_system->playMoveSoundEffect(chosen_move);

        if (chosen_move.moveType == MoveType::ATTACK || chosen_move.moveType == MoveType::MAGIC) {
            printf("You used the move: %s", chosen_move.nameAsString.c_str());
            printf("!, Enemy health : %d", enemy_health.health);
            printf(", Your Mana : %d\n", player_mana.mana);
            createBattleMoveUsed(world_system->getRenderer(), chosen_move, BattleState::PLAYER_TURN);

        }

        else if (chosen_move.moveType == MoveType::ITEM) {
            printf("You used the item: %s! \n", chosen_move.nameAsString.c_str());
            printf("Your health : %d", player_health.health);
            printf("    Enemy health : %d\n\n", enemy_health.health);
            createBattleMoveUsed(world_system->getRenderer(), chosen_move, BattleState::PLAYER_TURN);
        }
        registry.turns.remove(player_doll);
        game.battle_state = enemy_health.health < 1 ? BattleState::WON : BattleState::ENEMY_TURN;
    } 
    else if (game.battle_state == BattleState::ENEMY_TURN && registry.turns.has(registry.currentEnemies.entities[0])) {
        //ai.AISystem::step(500);
        if (enemyMoveDelay < 0) {
            Entity& enemy = registry.currentEnemies.entities[0];
            Turn& turn = registry.turns.get(enemy);
            BattleMove enemy_move = process_enemy_turn(turn, enemy);
            Health& player_health = registry.health.get(player_doll);
            Health& enemy_health = registry.health.get(registry.currentEnemies.entities[0]);

            world_system->playMoveSoundEffect(enemy_move);

            if (enemy_move.moveType == MoveType::ATTACK || enemy_move.moveType == MoveType::MAGIC) {
                printf("The enemy used the move: %s", enemy_move.nameAsString.c_str());
                printf("!, Your health : %d\n\n", player_health.health);
                createBattleMoveUsed(world_system->getRenderer(), enemy_move, BattleState::ENEMY_TURN);
            }

            else if (enemy_move.moveType == MoveType::ITEM) {
                printf("The enemy used the item: %s! \n", enemy_move.nameAsString.c_str());
                printf("Your health : %d", player_health.health);
                printf("    Enemy health : %d\n\n", enemy_health.health);
                createBattleMoveUsed(world_system->getRenderer(), enemy_move, BattleState::ENEMY_TURN);
            }

            registry.turns.remove(registry.currentEnemies.entities[0]);
            game.battle_state = BattleState::PLAYER_TURN;
            game.battle_state = player_health.health < 1 ? BattleState::LOST : BattleState::PLAYER_TURN;
//            enemyMoveDelay = game.state == GameState::BOSS ? 0.f : 1000.f;
            enemyMoveDelay = 1000.f;
        } else {
            enemyMoveDelay -= elapsed_ms;
        }
    } else if (game.battle_state == BattleState::WON) {
        printf("You win! :D \n");
        // TODO: additional battle winning logic like adding XP
        StatusEffect &player_status = registry.statusEffects.get(player_doll);
        resetStatus(player_status);
        end_battle(game, registry.currentEnemies.entities);
    } else if (game.battle_state == BattleState::LOST) {
        registry.game.get(player_doll).state = GameState::GAMEOVER;
        world_system->restart_game(GameStateChange::GAMEOVER);
        printf("You lose! :( \n");
        end_battle(game, registry.currentEnemies.entities);
    }
};

// Opponent's status timers are reduced, poison/ignite hits opponent, all status checks, self status check of taunted
void handleStatuses(Entity &attacker, Entity &target, Health &attackerHealth, Health &targetHealth, Turn &turn) {
    StatusEffect &target_status = registry.statusEffects.get(target);
    StatusEffect &self_status = registry.statusEffects.get(attacker);

    // THORNS
    if (target_status.THORNS) {
        if (target_status.thornTimer <= 0) {
            target_status.thornTimer = 0;
            target_status.THORNS = false;
            target_status.thorns_amount = 0;
        }
    }

    //Armor and Magic Res. timer update
    if (target_status.magiTimer > 0) {
        target_status.magiTimer -= 1;
    }
    if (target_status.magiTimer == 0) {
        target_status.magical_reduction = 0;
        target_status.magical_defense = 0;
    }
    if (target_status.physTimer > 0) {
        target_status.physTimer -= 1;
    }
    if (target_status.physTimer == 0) {
        target_status.physical_defense = 0;
        target_status.physical_reduction = 0;
    }


    // POISON CHECK
    if (target_status.POISONED) {
        if (target_status.poisonTimer > 0) {
            targetHealth.healthDecrement += target_status.poisonDamage;
            target_status.poisonTimer -= 1;
        }
    }
    if (target_status.poisonTimer <= 0) {
        target_status.POISONED = false;
    }

    // IGNITE CHECK
    if (target_status.IGNITED) {
        if (target_status.ignitedTimer > 0) {
            targetHealth.healthDecrement += target_status.ignite_damage;
            target_status.ignitedTimer -= 1;
        }
    }
    if (target_status.ignitedTimer <= 0) {
        target_status.IGNITED = false;
    }

    // EVASION CHECK
    if (target_status.EVASION > 0) {
        if (target_status.evasionTimer > 0) {
            target_status.evasionTimer -= 1;

            // See if attacks misses
            unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
            std::minstd_rand0 generator(seed);
            float chance = generator() % 100;

            if (chance < target_status.EVASION) {
                // TODO: Move print statement up or show battle feedback some other way - NK
                //printf("Attack was evaded!");
                targetHealth.healthDecrement = 0;
            }
        }
    }
    if (target_status.evasionTimer <= 0) {
        target_status.EVASION = 0;
        target_status.evasionTimer = 0;
    }

    // SELF TAUNT CHECK
    if (self_status.TAUNTED) {
        if (self_status.tauntTimer > 0) {
            self_status.tauntTimer -= 1;
            targetHealth.healthDecrement *= 0.85;
        }
    }
    if (self_status.tauntTimer <= 0) {
        self_status.TAUNTED = false;
    }

    // SHIELD CHECK
    if (target_status.SHIELDED) {
        if (target_status.shieldTimer > 0) {
            target_status.shieldTimer -= 1;
            if (target_status.shield_amount > 0) {
                if (targetHealth.healthDecrement > target_status.shield_amount) {
                    targetHealth.healthDecrement -= target_status.shield_amount;
                    // Shield break
                    target_status.SHIELDED = false;
                    target_status.shield_amount = 0;
                    target_status.shieldTimer = 0;
                }
                else {
                    target_status.shield_amount -= targetHealth.healthDecrement;
                    targetHealth.healthDecrement = 0;
                }
            }
        }
    }
    if (target_status.shieldTimer <= 0) {
        target_status.SHIELDED = false;
        target_status.shield_amount = 0;
        target_status.shieldTimer = 0;
    }


}

// Handles all attacks (non magical ones)
void handleAttacks(Entity &attacker, Entity &target, Health &targetHealth, Health &selfHealth, Turn &turn) {
    AttackList &attacks = registry.attackLists.get(attacker);
    Attack chosen_attack = attacks.getAttack(turn.attack_name);
    StatusEffect &target_status = registry.statusEffects.get(target);

    if (turn.attack_name == AttackName::TRIP) {
        targetHealth.healthDecrement = targetHealth.maxHP * chosen_attack.damage;
    } else {
        targetHealth.healthDecrement = chosen_attack.damage;
    }

    // apply damage reduction
    if (target_status.physical_defense > 0) {
        targetHealth.healthDecrement = targetHealth.healthDecrement * (1 - target_status.physical_reduction);
    }

    //Thorns only active if attacker attacks (attempts to deal damage before status effects)
    if (target_status.THORNS && targetHealth.healthDecrement > 0) {
        selfHealth.health -= targetHealth.healthDecrement * target_status.thorns_amount *
                             (1 - registry.statusEffects.get(attacker).physical_reduction);
        target_status.thornTimer -= 1;
    }

}

// Handles Magic Attacks
void handleMagicAttacks(Entity &attacker, Entity &target, Health &targetHealth, Health &selfHealth, Turn &turn) {
    MagicList &magic = registry.magicLists.get(attacker);
    MagicName magicName = turn.magic_name;
    Magic chosen_magic = magic.getAttack(magicName);
    StatusEffect &target_status = registry.statusEffects.get(target);
    StatusEffect &attacker_status = registry.statusEffects.get(attacker);
    Mana& attacker_mana = registry.mana.get(attacker);
    attacker_mana.mana -= chosen_magic.manaCost;

    if (magicName == MagicName::LIGHTNING_STRIKE) {
        targetHealth.healthDecrement = chosen_magic.amount;
        if (target_status.THORNS && targetHealth.healthDecrement > 0) {
            selfHealth.health -= 2 * targetHealth.healthDecrement * target_status.thorns_amount *
                                 (1 - registry.statusEffects.get(attacker).physical_reduction);
            target_status.thornTimer -= 2;
        }
        if (target_status.thornTimer <= 0) {
            target_status.THORNS = false;
            target_status.thornTimer = 0;
            target_status.thorns_amount = 0;
        }

        // Double damage if wet
        if (target_status.WET) {
            targetHealth.healthDecrement *= 2;
            target_status.WET = false;
        }
    } else if (magicName == MagicName::FIREBALL) {
        targetHealth.healthDecrement = chosen_magic.amount;
        unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
        std::minstd_rand0 generator(seed);
        float chance = generator() % 100;
        if (chance < chosen_magic.amount) {
            target_status.IGNITED = true;
        }
        // 30% less damage if wet
        if (target_status.WET) {
            targetHealth.healthDecrement *= 0.7;
            target_status.WET = false;
        }
        attacker_status.WET = false;
    } else if (magicName == MagicName::SPLASH) {
        targetHealth.healthDecrement = chosen_magic.amount;
        // Opponent is wet
        target_status.WET = true;

        // Removes ignite from both parties
        target_status.IGNITED = false;
        target_status.ignitedTimer = 0;
        target_status.ignite_damage = 0;

        attacker_status.IGNITED = false;
        attacker_status.ignitedTimer = 0;
        attacker_status.ignite_damage = 0;
    } else if (magicName == MagicName::DARKNESS) {
        targetHealth.healthDecrement = chosen_magic.amount * targetHealth.health;

    }

    // Thorns application
    if (target_status.THORNS && targetHealth.healthDecrement > 0) {
        selfHealth.health -= targetHealth.healthDecrement * target_status.thorns_amount *
                             (1 - registry.statusEffects.get(attacker).magical_reduction);
        target_status.thornTimer -= 1;
    }

    // apply damage reduction
    if (target_status.magical_reduction > 0) {
        targetHealth.healthDecrement = targetHealth.healthDecrement * (1 - target_status.magical_reduction);
    }
}

// Handles Magic Defense
void handleMagicDefense(Entity &attacker, Health &selfHealth, Turn &turn) {
    MagicList &magic = registry.magicLists.get(attacker);
    MagicName magicName = turn.magic_name;
    Magic chosen_magic = magic.getAttack(magicName);
    StatusEffect &attacker_status = registry.statusEffects.get(attacker);
    Mana& attacker_mana = registry.mana.get(attacker);
    attacker_mana.mana -= chosen_magic.manaCost;

    if (magicName == MagicName::ARMOR_UP) {
        if (attacker_status.physTimer == 0) {
            attacker_status.physTimer = chosen_magic.timer;
        }
        attacker_status.physical_defense += chosen_magic.physical_defense_boost;
        if (attacker_status.physical_defense > 90) {
            attacker_status.physical_defense = 90;
        }
        float x = attacker_status.physical_defense;
        attacker_status.physical_reduction = (x * log10(x + 50) - x / 2) / 100;

    } else if (magicName == MagicName::MAGIC_RESIST_UP) {
        if (attacker_status.magiTimer == 0) {
            attacker_status.magiTimer = chosen_magic.timer;
        }
        attacker_status.magical_defense += chosen_magic.magic_defense_boost;
        if (attacker_status.magical_defense > 90) {
            attacker_status.magical_defense = 90;
        }
        float x = attacker_status.magical_defense;
        attacker_status.magical_reduction = (x * log10(x + 50) - x / 2) / 100;
    } else if (magicName == MagicName::MIND_AND_BODY) {
        if (attacker_status.physTimer == 0) {
            attacker_status.physTimer = chosen_magic.timer;
        }
        if (attacker_status.magiTimer == 0) {
            attacker_status.magiTimer = chosen_magic.timer;
        }

        attacker_status.magical_defense += chosen_magic.magic_defense_boost;
        attacker_status.physical_defense += chosen_magic.physical_defense_boost;
        if (attacker_status.physical_defense > 90) {
            attacker_status.physical_defense = 90;
        }
        if (attacker_status.magical_defense > 90) {
            attacker_status.magical_defense = 90;
        }
        float x = attacker_status.magical_defense;
        attacker_status.magical_reduction = (x * log10(x + 50) - x / 2) / 100;
        x = attacker_status.physical_defense;
        attacker_status.physical_reduction = (x * log10(x + 50) - x / 2) / 100;

    } else if (magicName == MagicName::QUICK_STEP) {
        attacker_status.EVASION += chosen_magic.amount;
        attacker_status.evasionTimer = chosen_magic.timer;
    }
}

// Handles Magic Status Effects Moves
void handleMagicEffects(Entity &attacker, Entity &target, Health &targetHealth, Health &selfHealth, Turn &turn) {
    MagicList &magic = registry.magicLists.get(attacker);
    MagicName magicName = turn.magic_name;
    Magic chosen_magic = magic.getAttack(magicName);
    StatusEffect &target_status = registry.statusEffects.get(target);
    StatusEffect &attacker_status = registry.statusEffects.get(attacker);
    Mana& attacker_mana = registry.mana.get(attacker);
    attacker_mana.mana -= chosen_magic.manaCost;

    if (magicName == MagicName::TAUNT) {
        target_status.TAUNTED = true;
        target_status.tauntTimer = chosen_magic.timer;
    } else if (magicName == MagicName::SHIELD) {
        attacker_status.SHIELDED = true;
        attacker_status.shieldTimer = chosen_magic.timer;
        attacker_status.shield_amount = chosen_magic.amount;

    } else if (magicName == MagicName::THORNS) {
        attacker_status.THORNS = true;
        attacker_status.thorns_amount = chosen_magic.amount;
        attacker_status.thornTimer = chosen_magic.timer;

    } else if (magicName == MagicName::POISON_CLOUD) {
        target_status.POISONED = true;
        target_status.poisonDamage = chosen_magic.amount;
        target_status.poisonTimer = chosen_magic.timer;
    }

}

BattleMove BattleSystem::process_player_turn(Turn& player_turn) {
    BattleMove chosen_move;
    chosen_move.moveType = player_turn.move_type;
    Entity& player_doll = registry.players.entities[0];
    Entity& enemy = registry.currentEnemies.entities[0];
    Health& enemy_health = registry.health.get(enemy);
    Health& player_health = registry.health.get(player_doll);
    Mana& player_mana = registry.mana.get(player_doll);
    enemy_health.healthDecrement = 0;
    player_health.healthDecrement = 0;

    if (player_turn.move_type == MoveType::ATTACK || registry.statusEffects.get(player_doll).TAUNTED) {
        AttackList &player_attacks = registry.attackLists.get(player_doll);
        Attack chosen_attack = player_attacks.getAttack(player_turn.attack_name);
        if (registry.statusEffects.get(player_doll).TAUNTED) {
            if (registry.attackLists.get(player_doll).available_attacks.size() > 0)
                chosen_attack = player_attacks.available_attacks[0];
        }
       

        handleAttacks(player_doll, enemy, enemy_health, player_health, player_turn);
        handleStatuses(player_doll, enemy, player_health, enemy_health, player_turn);

        enemy_health.health -= enemy_health.healthDecrement;
        chosen_move.nameAsString = chosen_attack.nameAsString();
    }
    else if (player_turn.move_type == MoveType::MAGIC) {
        MagicList& player_attacks = registry.magicLists.get(player_doll);
        Magic chosen_attack = player_attacks.getAttack(player_turn.magic_name);
       
        if (chosen_attack.magicType == MagicType::ATTACK) {
            handleMagicAttacks(player_doll, enemy, enemy_health, player_health, player_turn);
        } else if (chosen_attack.magicType == MagicType::DEFENSE) {
            handleMagicDefense(player_doll, player_health, player_turn);
        } else if (chosen_attack.magicType == MagicType::STATUS_EFFECT) {
            handleMagicEffects(player_doll, enemy, enemy_health, player_health, player_turn);
        }

        handleStatuses(player_doll, enemy, player_health, enemy_health, player_turn);
        enemy_health.health -= enemy_health.healthDecrement;
        chosen_move.nameAsString = chosen_attack.nameAsString();
    }
    else if (player_turn.move_type == MoveType::ITEM) {
        Inventory& player_inventory = registry.inventories.get(player_doll);
        GameItem chosen_item = player_inventory.getItem(player_turn.item_id);
        if (chosen_item.item_name == GameItemName::HEALING_POTION ||
            chosen_item.item_name == GameItemName::HEALING_POTION_2) {
            player_health.health =
                    player_health.health + chosen_item.health > player_health.maxHP ?
                    player_health.maxHP :
                    player_health.health + chosen_item.health;
            player_mana.mana =
                player_mana.mana + chosen_item.mana > player_mana.maxMana ?
                player_mana.maxMana :
                player_mana.mana + chosen_item.mana;
            handleStatuses(enemy, player_doll, enemy_health, player_health, player_turn);
        }
        chosen_move.nameAsString = chosen_item.nameAsString();
        player_inventory.removeItem(player_turn.item_id);
        world_system->updateBattleScreenItemOptions();
    }
    return chosen_move;
}


BattleMove BattleSystem::process_enemy_turn(Turn& enemy_turn, Entity& enemy) {
    BattleMove enemy_chosen_move;
    enemy_chosen_move.moveType = enemy_turn.move_type;
    Entity& player_doll = registry.players.entities[0];
    Health& player_health = registry.health.get(player_doll);
    Health& enemy_health = registry.health.get(enemy);
    enemy_health.healthDecrement = 0;
    player_health.healthDecrement = 0;
    if (enemy_turn.move_type == MoveType::ATTACK) {
        AttackList& enemy_attacks = registry.attackLists.get(enemy);
        Attack chosen_attack = enemy_attacks.getAttack(enemy_turn.attack_name);

        if (registry.statusEffects.get(enemy).TAUNTED) {
            if (registry.attackLists.get(enemy).available_attacks.size() > 0)
                chosen_attack = enemy_attacks.available_attacks[0];
        }
        handleAttacks(enemy, player_doll, player_health, enemy_health, enemy_turn);
        handleStatuses(enemy, player_doll, enemy_health, player_health, enemy_turn);

        player_health.health -= player_health.healthDecrement;
        enemy_chosen_move.nameAsString = chosen_attack.nameAsString();
    }
    else if (enemy_turn.move_type == MoveType::MAGIC) {
        MagicList& enemy_attacks = registry.magicLists.get(enemy);
        Magic chosen_attack = enemy_attacks.getAttack(enemy_turn.magic_name);

        if (chosen_attack.magicType == MagicType::ATTACK) {
            handleMagicAttacks(enemy, player_doll, player_health, enemy_health, enemy_turn);
        } else if (chosen_attack.magicType == MagicType::DEFENSE) {
            handleMagicDefense(enemy, enemy_health, enemy_turn);
        } else if (chosen_attack.magicType == MagicType::STATUS_EFFECT) {
            handleMagicEffects(enemy, player_doll, player_health, enemy_health, enemy_turn);
        }

        handleStatuses(enemy, player_doll, enemy_health, player_health, enemy_turn);
        player_health.health -= player_health.healthDecrement;
        enemy_chosen_move.nameAsString = chosen_attack.nameAsString();
    }
    else if (enemy_turn.move_type == MoveType::ITEM) {
        Health& enemy_health = registry.health.get(registry.currentEnemies.entities[0]);
        GameItem& enemy_item = registry.gameItems.get(enemy);
        if (enemy_item.item_name == GameItemName::HEALING_POTION) {
            if (enemy_health.health + enemy_item.health > enemy_health.maxHP) {
                enemy_health.health = enemy_health.maxHP;
            } else {
                enemy_health.health += enemy_item.health;
            }

            registry.gameItems.remove(enemy);
            handleStatuses(enemy, player_doll, enemy_health, player_health, enemy_turn);
            enemy_chosen_move.nameAsString = "HEALTH POTION";
        }
    }
    return enemy_chosen_move;
}

void BattleSystem::end_battle(Game &game, std::vector<Entity> current_enemies) {
    for (int i = 0; i < current_enemies.size(); i++) {
        Entity enemy = current_enemies[0];
        registry.remove_all_components_of(enemy);
        world_system->clearBattleMoveUsed();
        world_system->destroyBattleWindow();
    }
    if (advancedDecisionTree != nullptr) {
        freeTree(advancedDecisionTree);
        advancedDecisionTree = nullptr;
    }
    game.battle_state = BattleState::NONE;
    if (game.state == GameState::BOSS) {
        game.state = GameState::ENDINGBOSS;
    } else if (game.state != GameState::GAMEOVER || game.state != GameState::TRANSITION) {
        game.state = GameState::PLAYING;
    }


    printf("=====Battle ended=====\n");

    world_system->incrementEnemiesKilled();

}

TreeNode *BattleSystem::getRoot(Entity &player, Entity &enemy) {
    TreeNode *root = new TreeNode();
    root->moveType = MoveType::NONE;
    root->parent = nullptr;
    buildTree(player, enemy, 0, true, root);
    return root;
}

void BattleSystem::buildTree(Entity &player, Entity &enemy, int level, bool turn, TreeNode *parent) {
    if (level >= max_depth) {
        return;
    }

    if (turn) {
        buildNode(parent, enemy, true);
    } else {
        buildNode(parent, player, false);
    }

    for (TreeNode *tn: parent->children) {
        buildTree(player, enemy, level + 1, !turn, tn);
    }
}

void BattleSystem::buildNode(TreeNode *tn, Entity &e, bool turn) {
    vector<Attack> attack_list = registry.attackLists.get(e).available_attacks;
    for (Attack a: attack_list) {
        TreeNode *child = new TreeNode();
        child->enemy = turn;
        child->moveType = MoveType::ATTACK;
        child->a = a;
        child->parent = tn;
        tn->children.push_back(child);
    }

    MagicList magicList = registry.magicLists.get(e);
    vector<Magic> magic_list;
    magic_list.insert(magic_list.end(), magicList.available_magic_attacks.begin(),
                      magicList.available_magic_attacks.end());
    magic_list.insert(magic_list.end(), magicList.available_magic_defenses.begin(),
                      magicList.available_magic_defenses.end());
    magic_list.insert(magic_list.end(), magicList.available_magic_effects.begin(),
                      magicList.available_magic_effects.end());
    for (Magic m: magic_list) {
        TreeNode *child = new TreeNode();
        child->enemy = turn;
        child->moveType = MoveType::MAGIC;
        child->m = m;
        child->parent = tn;
        tn->children.push_back(child);
    }

    // TODO: uncomment when potions are implemented
//    vector<GameItem> gameItem_list = registry.inventories.get(e).items;
//    vector<GameItem> useableItems;
//    for(GameItem gi: gameItem_list) {
//        if (gi.health > 0) {
//            useableItems.push_back(gi);
//        }
//    }

//    for(GameItem gi: useableItems) {
//        TreeNode * child = new TreeNode();
//        child->enemy = turn;
//        child->moveType = MoveType::ITEM;
//        child->g = gi;
//        child->parent = tn;
//        tn->children.push_back(child);
//    }

}

void BattleSystem::freeTree(TreeNode *currNode) {
    if (currNode == nullptr) {
        return;
    }

    for (TreeNode *child: currNode->children) {
        freeTree(child);
    };

    free(currNode);
}

float BattleSystem::DFS(TreeNode *curr, Entity &player, Entity &enemy) {
    if (curr == nullptr) {
        return 0.f;
    }

    // get relevant information
    Health player_health = registry.health.get(player);
    Mana player_mana = registry.mana.get(player);
    // TODO: uncomment when potions are implemented
    // Inventory player_gameItemList = registry.inventories.get(player);
    StatusEffect player_effects = registry.statusEffects.get(player);

    Health enemy_health = registry.health.get(enemy);
    Mana enemy_mana = registry.mana.get(enemy);
    // Inventory enemy_gameItemList = registry.inventories.get(enemy);
    StatusEffect enemy_effects = registry.statusEffects.get(enemy);

    MoveType currMT = curr->moveType;
    Turn t;
    switch (currMT) {
        case MoveType::ATTACK:
            t = Turn(curr->a.name);
            break;
        case MoveType::MAGIC:
            t = Turn(curr->m.name);
            break;
        case MoveType::ITEM:
            t = Turn(curr->g.item_name);
            break;
        default:
            break;
    }

    if (curr->enemy) {
        process_enemy_turn(t, enemy);
    } else {
        process_player_turn(t);
    }
    float sum = 0;
    // Do DFS on children
    for (TreeNode *tn: curr->children) {
        sum += DFS(tn, player, enemy);
    }

    if (curr->children.empty()) {
        sum = scoreState(player, enemy);
    }

    // Restore important properties
    registry.health.get(player) = player_health;
    registry.mana.get(player) = player_mana;
    // registry.inventories.get(player) = player_gameItemList;
    registry.statusEffects.get(player) = player_effects;

    registry.health.get(enemy) = enemy_health;
    registry.mana.get(enemy) = enemy_mana;
    // registry.inventories.get(enemy) = enemy_gameItemList;
    registry.statusEffects.get(enemy) = enemy_effects;

    return sum;
}



//TreeNode * BattleSystem::getEnemyMove(TreeNode * tn) {
//    while(tn->parent->parent != nullptr) {
//        tn = tn->parent;
//    }
//
//    return tn;
//}

Turn BattleSystem::selectEnemyMove(Entity &player, Entity &enemy) {
    auto * scores = new float[advancedDecisionTree->children.size()];

    for (int i = 0; i < advancedDecisionTree->children.size(); i++) {
        scores[i] = (DFS(advancedDecisionTree->children[i], player, enemy));
    }

    float max = std::numeric_limits<float>::lowest();
    int maxIdx = 0;
    int secondIdx = 0;
    for (int i = 0; i < advancedDecisionTree->children.size(); i++) {
        if (scores[i] > max) {
            max = scores[i];
            secondIdx = maxIdx;
            maxIdx = i;
        }
    }

    unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
    std::minstd_rand0 generator(seed);
    float chance = generator() % 100;

    int pickedIdx = maxIdx;
    
    if (chance > enemyBestMovePercent) {
        pickedIdx = secondIdx;
    }

    delete[] scores;

    TreeNode *bestMove = advancedDecisionTree->children[pickedIdx];

    MoveType currMT = bestMove->moveType;
    Turn t;
    switch (currMT) {
        case MoveType::ATTACK:
            t = Turn(bestMove->a.name);
            break;
        case MoveType::MAGIC:
            t = Turn(bestMove->m.name);
            break;
        case MoveType::ITEM:
            t = Turn(bestMove->g.item_name);
            break;
        default:
            break;
    }
    return t;
}

float BattleSystem::scoreState(Entity &player, Entity &enemy) {
    return (float) (registry.health.get(enemy).health - registry.health.get(player).health);
}
