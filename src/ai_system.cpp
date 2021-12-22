// internal
#include "ai_system.hpp"
#include "world_init.hpp"
#include "world_system.hpp"

#include <iostream>
#include <random>

#include <vector>
#include <chrono>
#include <queue>

float ms_since_recompute = 0;
float enemy_speed = 80;
int detection_radius = 4;
std::vector<std::vector<int>> grid;
std::pair<int, int> dollPos;
std::pair<int, int> enemyPos;

// DETECT: Detects if the player is in detection range
bool detect(Entity& enemy) {
	// Is the player in range
	float x = pow(enemyPos.first - dollPos.first, 2);
	float y = pow(enemyPos.second - dollPos.second, 2);
	float h = sqrt(x + y);

	if (registry.bears.has(enemy)) {
		if (h <= detection_radius + 1) {
			return true;
		}
	}

	if (registry.bunnies.has(enemy)) {
		if (h <= detection_radius) {
			return true;
		}
	}
	
	return false;
}


// Chasing: Enemy will endlessly pursue the player
void chase(Entity& e, Behaviour& behaviour) {
	// Run
	behaviour.CHASE = BTStates::BT_Running;
}
// ROAM: Enemy will roam around in a pattern spsecific to their type
void roam(Entity& e, Behaviour& behaviour, std::vector<std::vector<int>> copygrid) {
	// Roam around the map
	if (detect(e)) {
		behaviour.ROAM = BTStates::BT_Success;
		registry.roaming.remove(e);
		chase(e, behaviour);
	}

	if (behaviour.ROAM == BTStates::BT_Success) {
		chase(e, behaviour);
	}
	else {
		if (behaviour.ROAM == BTStates::BT_None) {
			behaviour.ROAM = BTStates::BT_Running;
			Roaming ro = Roaming(registry.enemies.get(e).name);
			registry.roaming.insert(e, ro, true);
		}
	}

	Motion& e_motion = registry.motions.get(e);

	if (behaviour.ROAM == BTStates::BT_Running) {
		Roaming& ro = registry.roaming.get(e);
		bool left = false;
		bool right = false;
		bool up = false;
		bool down = false;

		if (enemyPos.first - 1 >= 0)
		{
			if (copygrid[enemyPos.first - 1][enemyPos.second] == 1) {
				left = true;
			}
		}
		if (enemyPos.first + 1 < grid.size()) {
			if (copygrid[enemyPos.first + 1][enemyPos.second] == 1) {
				right = true;
			}
		}
		if (enemyPos.second - 1 >= 0)
		{
			if (copygrid[enemyPos.first][enemyPos.second - 1] == 1) {
				up = true;
			}
		}
		if (enemyPos.second + 1 < grid[0].size()) {
			if (copygrid[enemyPos.first][enemyPos.second + 1] == 1) {
				down = true;
			}
		}

		if (ro.type == EnemyName::DUST_BUNNY) {
			
			if (!ro.MOVE_LEFT && !ro.MOVE_RIGHT) {
				if (left) {
					ro.MOVE_LEFT = true;
					ro.roaming_timer = 2;
					e_motion.velocity = { -enemy_speed, 0 };
				} else if (right) {
					ro.MOVE_RIGHT = true;
					ro.roaming_timer = 2;
					e_motion.velocity = { enemy_speed, 0 };
				}
				else {
					e_motion.velocity = { 0, 0 };
				}
				
			}
			else if (ro.MOVE_LEFT && ro.roaming_timer > 0) {
				if (left) {
					ro.roaming_timer--;
					e_motion.velocity = { -enemy_speed, 0 };
					
				}
				else if (right) {
					ro.MOVE_LEFT = false;
					ro.MOVE_RIGHT = true;
					ro.roaming_timer = 2;
					e_motion.velocity = { enemy_speed, 0 };
					
				}
				
			}
			else if (ro.MOVE_LEFT && ro.roaming_timer == 0){
				ro.MOVE_LEFT = false;
				if (right) {
					ro.MOVE_RIGHT = true;
					ro.roaming_timer = 2;
					e_motion.velocity = { enemy_speed, 0 };
				}
				else if (left) {
					ro.MOVE_LEFT = true;
					ro.roaming_timer = 2;
					e_motion.velocity = { -enemy_speed, 0 };
				}
				else {
					e_motion.velocity = { 0, 0 };
				}
				
			}
			else if (ro.MOVE_RIGHT && ro.roaming_timer > 0) {
				if (right) {
					ro.roaming_timer--;
					e_motion.velocity = { +enemy_speed, 0 };

				}
				else if (left) {
					ro.MOVE_LEFT = true;
					ro.MOVE_RIGHT = false;
					ro.roaming_timer = 2;
					e_motion.velocity = { -enemy_speed, 0 };

				}
				else {
					e_motion.velocity = { 0, 0 };
				}
			}
			else if (ro.MOVE_RIGHT && ro.roaming_timer == 0) {
				ro.MOVE_RIGHT = false;
				if (left) {
					ro.MOVE_LEFT = true;
					ro.roaming_timer = 2;
					e_motion.velocity = { -enemy_speed, 0 };
				}
				else if (right) {
					ro.MOVE_RIGHT = true;
					ro.roaming_timer = 2;
					e_motion.velocity = { +enemy_speed, 0 };
				}
				else {
					e_motion.velocity = { 0, 0 };
				}

			}
			
		}
		else if (ro.type == EnemyName::TEDDY_BEAR) {
			// BASE CASE
			if (!ro.MOVE_LEFT && !ro.MOVE_RIGHT && !ro.MOVE_UP && !ro.MOVE_DOWN) {
				ro.roaming_timer = 2;
				if (left) {
					ro.MOVE_LEFT = true;
					e_motion.velocity = { -enemy_speed, 0 };
				}
				else if (up) {
					ro.MOVE_UP = true;
					e_motion.velocity = { 0, -enemy_speed };
				}
				else if (right) {
					ro.MOVE_RIGHT = true;
					e_motion.velocity = { +enemy_speed, 0 };
				}
				else if (down) {
					ro.MOVE_DOWN = true;
					e_motion.velocity = { 0, enemy_speed };
				}
				
			}
			// LEFT
			else if (ro.MOVE_LEFT && ro.roaming_timer > 0){

				if (left) {
					ro.roaming_timer--;
					e_motion.velocity = { -enemy_speed, 0 };
				}
				else if (up) {
					ro.MOVE_LEFT = false;
					ro.MOVE_UP = true;
					ro.roaming_timer = 2;
					e_motion.velocity = { 0, -enemy_speed };
				}
				else if (right) {
					ro.MOVE_LEFT = false;
					ro.MOVE_RIGHT = true;
					ro.roaming_timer = 2;
					e_motion.velocity = { +enemy_speed, 0 };
				}
				else if (down) {
					ro.MOVE_LEFT = false;
					ro.MOVE_DOWN = true;
					ro.roaming_timer = 2;
					e_motion.velocity = { 0, enemy_speed };
				}
				

			}else if (ro.MOVE_LEFT && ro.roaming_timer == 0) {
				ro.MOVE_LEFT = false;

				if (up) {
					ro.MOVE_UP = true;
					ro.roaming_timer = 2;
					e_motion.velocity = { 0, -enemy_speed };
				}
				else if (right) {
					ro.MOVE_RIGHT = true;
					ro.roaming_timer = 2;
					e_motion.velocity = { +enemy_speed, 0 };
				}
				else if (down) {
					ro.MOVE_DOWN = true;
					ro.roaming_timer = 2;
					e_motion.velocity = { 0, enemy_speed };
				}
				else if (left) {
					ro.MOVE_LEFT = true;
					ro.roaming_timer = 2;
					e_motion.velocity = { -enemy_speed, 0 };
				}
				
			}
			// UP
			else if (ro.MOVE_UP && ro.roaming_timer > 0) {

				if (up) {
					ro.roaming_timer--;
					e_motion.velocity = { 0, -enemy_speed };
				}
				else if (right) {
					ro.MOVE_UP = false;
					ro.MOVE_RIGHT = true;
					ro.roaming_timer = 2;
					e_motion.velocity = { +enemy_speed, 0 };
				}
				else if (down) {
					ro.MOVE_UP = false;
					ro.MOVE_DOWN = true;
					ro.roaming_timer = 2;
					e_motion.velocity = { 0, enemy_speed };
				}
				else if (left) {
					ro.MOVE_UP = false;
					ro.MOVE_LEFT = true;
					ro.roaming_timer = 2;
					e_motion.velocity = { -enemy_speed, 0 };
				}


			}
			else if (ro.MOVE_UP && ro.roaming_timer == 0) {
				ro.MOVE_UP = false;

				if (right) {
					ro.MOVE_RIGHT = true;
					ro.roaming_timer = 2;
					e_motion.velocity = { +enemy_speed, 0 };
				}
				else if (down) {
					ro.MOVE_DOWN = true;
					ro.roaming_timer = 2;
					e_motion.velocity = { 0, enemy_speed };
				}
				else if (left) {
					ro.MOVE_LEFT = true;
					ro.roaming_timer = 2;
					e_motion.velocity = { -enemy_speed, 0 };
				}else if (up) {
					ro.MOVE_UP = true;
					ro.roaming_timer = 2;
					e_motion.velocity = { 0, -enemy_speed };
				}
				
			}
			// RIGHT
			else if (ro.MOVE_RIGHT && ro.roaming_timer > 0) {

				if (right) {
					ro.roaming_timer--;
					e_motion.velocity = { +enemy_speed, 0 };
				}
				else if (down) {
					ro.MOVE_RIGHT = false;
					ro.MOVE_DOWN = true;
					ro.roaming_timer = 2;
					e_motion.velocity = { 0, enemy_speed };
				}
				else if (left) {
					ro.MOVE_RIGHT = false;
					ro.MOVE_LEFT = true;
					ro.roaming_timer = 2;
					e_motion.velocity = { -enemy_speed, 0 };
				}
				else if (up) {
					ro.MOVE_RIGHT = false;
					ro.MOVE_UP = true;
					ro.roaming_timer = 2;
					e_motion.velocity = { 0, -enemy_speed };
				}
			}
			else if (ro.MOVE_RIGHT && ro.roaming_timer == 0) {
				ro.MOVE_RIGHT = false;

				if (down) {
					ro.MOVE_DOWN = true;
					ro.roaming_timer = 2;
					e_motion.velocity = { 0, enemy_speed };
				}
				else if (left) {
					ro.MOVE_LEFT = true;
					ro.roaming_timer = 2;
					e_motion.velocity = { -enemy_speed, 0 };
				}
				else if (up) {
					ro.MOVE_UP = true;
					ro.roaming_timer = 2;
					e_motion.velocity = { 0, -enemy_speed };
				}
				else if (right) {
					ro.MOVE_RIGHT = true;
					ro.roaming_timer = 2;
					e_motion.velocity = { +enemy_speed, 0 };
				}
			}// DOWN
			else if (ro.MOVE_DOWN && ro.roaming_timer > 0) {

				if (down) {
					ro.roaming_timer--;
					e_motion.velocity = { 0, enemy_speed };
				}
				else if (left) {
					ro.MOVE_DOWN = false;
					ro.MOVE_LEFT = true;
					ro.roaming_timer = 2;
					e_motion.velocity = { -enemy_speed, 0 };
				}
				else if (up) {
					ro.MOVE_DOWN = false;
					ro.MOVE_UP = true;
					ro.roaming_timer = 2;
					e_motion.velocity = { 0, -enemy_speed };
				} 
				else if (right) {
					ro.MOVE_DOWN = false;
					ro.MOVE_RIGHT = true;
					ro.roaming_timer = 2;
					e_motion.velocity = { +enemy_speed, 0 };
				}
				
			}
			else if (ro.MOVE_DOWN && ro.roaming_timer == 0) {
				ro.MOVE_DOWN = false;

				if (left) {
					ro.MOVE_LEFT = true;
					ro.roaming_timer = 2;
					e_motion.velocity = { -enemy_speed, 0 };
				}
				else if (up) {
					ro.MOVE_UP = true;
					ro.roaming_timer = 2;
					e_motion.velocity = { 0, -enemy_speed };
				}
				else if (right) {
					ro.MOVE_RIGHT = true;
					ro.roaming_timer = 2;
					e_motion.velocity = { +enemy_speed, 0 };
				}
				else if (down) {
					ro.MOVE_DOWN = true;
					ro.roaming_timer = 2;
					e_motion.velocity = { 0, enemy_speed };
				}
				
			}

		}

		if (!left && !up && !down && !right) {
			behaviour.ROAM == BTStates::BT_Failure;
		}
	}
}

// IDLE: Enemy behaviour of standing around, at the start to prepare for the day
void idling(Entity& e, std::vector<std::vector<int>> copygrid) {
	Behaviour& e_behaviour = registry.behaviours.get(e);
	// wait 3 ticks

	// Patch M4* Enemies will no longer detect during idle phase
	/*if (detect(e)) {
		e_behaviour.IDLE = BTStates::BT_Success;
		chase(e, e_behaviour);
	}*/

	if (e_behaviour.IDLE == BTStates::BT_Success) {
		roam(e, e_behaviour, copygrid);
		return;
	}
	else if (e_behaviour.IDLE == BTStates::BT_None) {
		e_behaviour.IDLE = BTStates::BT_Running;
		e_behaviour.IDLE_timer = 5;
	}
	
	if (e_behaviour.IDLE_timer > 0) {
		e_behaviour.IDLE_timer--;
		Motion& motion = registry.motions.get(e);
		motion.velocity = { 0,0 };
		return;
	}
	else {
		e_behaviour.IDLE = BTStates::BT_Success;
		roam(e, e_behaviour, copygrid);
		return;
	}
	
	return;
}


struct Choices
{
	float ATTACK = 30;
	float DEFEND = 25;
	float DEBUFF = 25;
	float HEAL = 20;
	bool MAGIC_ATTACK = false;

};

// Update enemy state
void updateEnemyState(Entity& enemy, Entity& player) {
	EnemyState& enemy_state = registry.currentEnemies.get(enemy).enemyState;
	int enemy_health = registry.health.get(enemy).health;
	int player_health = registry.health.get(player).health;
	int maxHP = registry.health.get(enemy).maxHP;

	if (enemy_health > 0.8*maxHP) {
		enemy_state = EnemyState::HEALTHY;
	}
	else if (enemy_health > 0.5 * maxHP) {
		enemy_state = EnemyState::DAMAGED;
	}
	else if (enemy_health > player_health + 20) {
		enemy_state = EnemyState::BOLD;
	}
	else if (enemy_health < player_health - 20) {
		enemy_state = EnemyState::THREATENED;
	}
	else if (enemy_health < 0.2 * maxHP) {
		enemy_state = EnemyState::NEAR_DEATH;
	}
	else if (registry.statusEffects.get(enemy).TAUNTED) {
		enemy_state = EnemyState::TAUNTED;
	}
}

void decideMoveType(Entity& enemy, Choices& choices, EnemyState state, StatusEffect player_status, StatusEffect enemy_status) {
	
	if (state == EnemyState::BOLD) {
		choices.DEFEND = 0;
		choices.ATTACK = 55;
		choices.HEAL = 10;
	}
	if (state == EnemyState::THREATENED) {
		choices.ATTACK = 10;
		choices.DEFEND = 40;
		choices.DEBUFF = 30;
	}
	if (state == EnemyState::NEAR_DEATH) {
		choices.HEAL = 50;
		choices.DEFEND = 30;
	}
	// Check if Enemy has moves to fulfill conditions
	//This should not ever happen, but just in case there's an enemy that does not have attacks.
	// Situations that will override the probabilities

	if (player_status.TAUNTED) {
		choices.DEBUFF = 15;
	}
	
	if (!registry.attackLists.has(enemy)) {
		if (registry.attackLists.get(enemy).available_attacks.size() == 0) {
			choices.ATTACK = 0;
		}
	}
	if (registry.magicLists.has(enemy)) {
		if (registry.magicLists.get(enemy).available_magic_attacks.size() == 0) {
			choices.MAGIC_ATTACK = false;
		}
		else {
			choices.MAGIC_ATTACK = true;
		}
		// Defense
		if (registry.magicLists.get(enemy).available_magic_defenses.size() == 0) {
			choices.DEFEND = 0;
		}
		// Debuffs
		if (registry.magicLists.get(enemy).available_magic_effects.size() == 0) {
			choices.DEBUFF = 0;
		}
	}
	
	if (!registry.gameItems.has(enemy) || state == EnemyState::HEALTHY || registry.gameItems.get(enemy).item_name != GameItemName::HEALING_POTION) {
		choices.HEAL = 0;
	}

}

void normalizeChoices(Choices& choices) {
	float total = choices.ATTACK + choices.DEBUFF + choices.DEFEND + choices.HEAL;

	if (choices.ATTACK > 0) {
		choices.ATTACK = (choices.ATTACK / total) * 100;
	}
	if (choices.DEBUFF > 0) {
		choices.DEBUFF = (choices.DEBUFF / total) * 100;
	}
	if (choices.DEFEND > 0) {
		choices.DEFEND = (choices.DEFEND / total) *  100;
	}
	if (choices.HEAL > 0) {
		choices.HEAL = (choices.HEAL / total) * 100;
	}
}


// Decide on a move based on state and statuses
Turn AISystem::decideMove(Entity& enemy, Entity& player){
	EnemyState enemy_state = registry.currentEnemies.components[0].enemyState;
	StatusEffect enemy_status = registry.statusEffects.get(enemy);
	StatusEffect player_status = registry.statusEffects.get(player);
	GameItem item = GameItem();
	if (registry.gameItems.has(enemy)) {
		GameItem& item = registry.gameItems.get(enemy);
	}
	
	// Chance roll to see what to do
	unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
	std::minstd_rand0 generator(seed);  // minstd_rand0 is a standard linear_congruential_engine
	float chance_roll = generator() % 100;
	
	// Choices and their probabilities
	Choices choices;
	// Update move probability list based on Player and Enemy States and statuses.
	decideMoveType(enemy, choices, enemy_state, player_status, enemy_status);
	// Normalize probability
	normalizeChoices(choices);
	

	// If taunted, no choice of moves, must do an attacks, will use their first attacks (usually their signature base move)
	if (enemy_state == EnemyState::TAUNTED) {
		if(registry.attackLists.get(enemy).available_attacks.size() > 0)
			return Turn(registry.attackLists.get(enemy).available_attacks[0].name);
	}
	// Attack
	if ((chance_roll >= 0) && (chance_roll < choices.ATTACK)) {
		int total = 0;

		AttackList& attacks = registry.attackLists.get(enemy);
		int attackChoices = attacks.available_attacks.size();
		total += attackChoices;

		if (choices.MAGIC_ATTACK) {
			MagicList& magics = registry.magicLists.get(enemy);
			int magicChoices = magics.available_magic_attacks.size();
			total += magicChoices;
		}
		
		int attackChoice = generator() % (total);

		if (attackChoice < attackChoices) {
			return Turn(attacks.available_attacks[attackChoice].name);
		}
		else {
			if (choices.MAGIC_ATTACK) {
				MagicList& magics = registry.magicLists.get(enemy);
				MagicName mn = magics.available_magic_attacks[attackChoice - attackChoices].name;
				if (canMakeMove(enemy, mn)) {
					return Turn(mn);
				}
				else {
					return decideMove(enemy, player);
				}
			}
		}
	}
	// Defend/ Self Buff
	else if ((chance_roll >= choices.ATTACK) && (chance_roll < (choices.ATTACK + choices.DEFEND))) {
		MagicList& magics = registry.magicLists.get(enemy);
		int magicChoices = magics.available_magic_defenses.size();
		int magicChoice = generator() % magicChoices;
		MagicName mn = magics.available_magic_defenses[magicChoice].name;
		if (canMakeMove(enemy, mn)) {
			return Turn(mn);
		}
	}
	// Debuff Player
	else if ((chance_roll >= (choices.ATTACK + choices.DEFEND)) && (chance_roll < (choices.ATTACK + choices.DEFEND + choices.DEBUFF))) {
		MagicList& magics = registry.magicLists.get(enemy);
		int magicChoices = magics.available_magic_effects.size();
		int magicChoice = generator() % magicChoices;
		MagicName mn = magics.available_magic_effects[magicChoice].name;
		if (canMakeMove(enemy, mn)) {
			return Turn(mn);
		}
	}
	// Heal
	else if ((chance_roll >= (choices.ATTACK + choices.DEFEND + choices.DEBUFF)) && (chance_roll < (choices.ATTACK + choices.DEFEND + choices.DEBUFF + choices.HEAL))) {
		return Turn(GameItemName::HEALING_POTION);
	}

	// if nothing has worked so far, default to first available attack
	return Turn(registry.attackLists.get(enemy).available_attacks[0].name);
}


void AISystem::step(float elapsed_ms)
{
	Entity player_doll = registry.players.entities[0];
	Motion& player_motion = registry.motions.get(player_doll);
	Game& game = registry.game.get(player_doll);

	if (game.battle_state == BattleState::ENEMY_TURN) {
        do_enemy_turn();
		return;
	}

	if (debugging.in_debug_mode) {
		for (int i = 0; i < grid.size(); i++) {
			for (int j = 0; j < grid[0].size(); j++) {
				if (grid[i][j]) {
					createLine({ 50.f * i, 50.f * j }, { 5.f, 5.f });
				}
			}
		}
	}

	ms_since_recompute -= elapsed_ms;
	if (ms_since_recompute <= 0) {
		ms_since_recompute = 500;


		for (int i = 0; i < registry.AIEntities.entities.size(); i++) {
			registry.AIEntities.get(registry.AIEntities.entities[i]).coveredPositions = {};
		}
		grid = {};
		PhysicsSystem physics;
		std::function<void(Entity, bool)> callback = physics.getCollisionFunction();
		for (int i = 0; i < window_width_px / 50.f; i++) {
			std::vector<int> col;
			for (int j = 0; j < window_height_px / 50.f; j++) {
				Entity testEntity = createTestEnemy({ 50.f * i, 50.f * j });

				callback(testEntity, true);

				if (registry.motions.has(testEntity)) {
					col.push_back(1);
					registry.remove_all_components_of(testEntity);
				}
				else {
					col.push_back(0);
				}
			}
			grid.push_back(col);
		}


		dollPos = std::make_pair(
			round((player_motion.position.x - fmod(player_motion.position.x, 50)) / 50.f),
			round(((player_motion.position.y + player_motion.scale.y/4) - fmod((player_motion.position.y + player_motion.scale.y / 4), 50)) / 50.f)
		);

		if (dollPos.first >= 0 && dollPos.first < grid.size() && dollPos.second >= 0 && dollPos.second < grid[0].size() && grid[dollPos.first][dollPos.second] == 0) {
			grid[dollPos.first][dollPos.second] = 1;
		}


		// Do a BFS
		for (int i = 0; i < registry.enemies.size(); i++) {
			std::vector<std::vector<int>> copyGrid = grid;
			Motion& enemy_motion = registry.motions.get(registry.enemies.entities[i]);
			enemyPos = std::make_pair(
				round((enemy_motion.position.x - fmod(enemy_motion.position.x, 50)) / 50.f),
				round((enemy_motion.position.y - fmod(enemy_motion.position.y, 50)) / 50.f)
			);

			

			for (int j = 0; j < registry.AIEntities.entities.size(); j++) {
				std::vector<std::pair<int, int>> coveredPositions = registry.AIEntities.get(registry.AIEntities.entities[j]).coveredPositions;
				for (int k = 0; k < coveredPositions.size(); k++) {
					copyGrid[coveredPositions[k].first][coveredPositions[k].second] = 1;
				}
			}

			// Run behaviour tree dollPos enemyPos
			Entity& enemy = registry.enemies.entities[i];
			if (registry.behaviours.has(enemy)) {
                idling(enemy, copyGrid);

                if (registry.behaviours.get(enemy).CHASE != BTStates::BT_Running) {
                    continue;
                }

                std::vector<std::pair<int, int>> path = bfs(copyGrid, enemyPos, dollPos);
                if (path.size() > 1) {
                    enemy_motion.velocity = {(path[1].first - enemyPos.first) * enemy_speed,
                                             (path[1].second - enemyPos.second) * enemy_speed};
                    if (enemy_motion.velocity.x + enemy_motion.velocity.y > enemy_speed) {
                        enemy_motion.velocity = {
                                (enemy_motion.velocity.x / (enemy_motion.velocity.x + enemy_motion.velocity.y)) *
                                enemy_speed,
                                (enemy_motion.velocity.y / (enemy_motion.velocity.x + enemy_motion.velocity.y)) *
                                enemy_speed
                        };
                    }
                } else {
                    enemy_motion.velocity = {0.f, 0.f};
                }
            }
		}

	}
}

// Enemy Turn handling
void AISystem::do_enemy_turn() {
	Entity player_doll = registry.players.entities[0];
	Game& game = registry.game.get(player_doll);

	assert(game.state == GameState::BATTLE || game.state == GameState::BOSS);
	assert(!registry.currentEnemies.entities.empty());

	// Get the current enemies in battle
	std::vector<Entity> current_enemies = registry.currentEnemies.entities;

	// Just have one enemy attacks with whatever its first attacks is for now
	AttackList& attack_list = registry.attackLists.get(current_enemies[0]);
	assert(!attack_list.available_attacks.empty()); // TODO: deal with having no attacks

	// Decide what attacks to do based on state enemy is in
   if (!registry.turns.has(current_enemies[0])) {
       Entity enemy = current_enemies[0];
       updateEnemyState(enemy, player_doll);
       Turn chosen_move;
       if (game.state == GameState::BOSS) {
           chosen_move = battleSystem->selectEnemyMove(player_doll, enemy);
           if (chosen_move.move_type == MoveType::MAGIC) {
               MagicList& enemy_magic = registry.magicLists.get(enemy);
               Magic magic = enemy_magic.getAttack(chosen_move.magic_name);
               Mana enemy_mana = registry.mana.get(enemy);
               if (magic.manaCost > enemy_mana.mana) {
                   chosen_move = decideMove(enemy, player_doll);
               }
           }
       } else {
           chosen_move = decideMove(enemy, player_doll);
       }
       registry.turns.insert(current_enemies[0], chosen_move);
   }

}

std::vector<std::pair<int, int>> getNeighbours(std::pair<int, int > center) {
	std::vector<std::pair<int, int>> neighbours = {};
	neighbours.push_back(std::make_pair(center.first, center.second - 1));
	neighbours.push_back(std::make_pair(center.first - 1, center.second));
	neighbours.push_back(std::make_pair(center.first + 1, center.second));
	neighbours.push_back(std::make_pair(center.first, center.second + 1));
	neighbours.push_back(std::make_pair(center.first - 1, center.second - 1));
	neighbours.push_back(std::make_pair(center.first + 1, center.second - 1));
	neighbours.push_back(std::make_pair(center.first - 1, center.second + 1));
	neighbours.push_back(std::make_pair(center.first + 1, center.second + 1));

	return neighbours;
}

// do bfs to find shortest path from curr to goal and return the position of the first node in that path
std::vector<std::pair<int, int>> AISystem::bfs(std::vector<std::vector<int>> grid, std::pair<int, int> curr, std::pair<int, int> goal) {
	std::queue<std::vector<std::pair<int, int>>> queue;
	std::vector<std::pair<int, int>> currVec;
	currVec.push_back(curr);
	queue.push(currVec);
	grid[curr.first][curr.second] = 0;

	while (!queue.empty()) {
		std::vector<std::pair<int, int>> curr_path = queue.front();
		std::pair<int, int> last_node = curr_path.back();
		if (last_node.first == goal.first && last_node.second == goal.second) {
			return curr_path;
		}

		std::vector<std::pair<int, int>> neighbours = getNeighbours(last_node);

		for (int i = 0; i < neighbours.size(); i++) {
			if (neighbours[i].first >= 0 && neighbours[i].first < grid.size() && neighbours[i].second >= 0 && neighbours[i].second < grid[0].size() && grid[neighbours[i].first][neighbours[i].second] == 1) {
				grid[neighbours[i].first][neighbours[i].second] = 0;
				std::vector<std::pair<int, int>> new_path = curr_path;
				new_path.push_back(std::make_pair(neighbours[i].first, neighbours[i].second));
				queue.push(new_path);
			}
		}
		queue.pop();
	}
	return {};
}

AISystem::AISystem(BattleSystem *b) {
    battleSystem = b;
}


bool AISystem::canMakeMove(Entity attacker, MagicName name) {
	MagicList& player_attacks = registry.magicLists.get(attacker);
	Magic chosen_attack = player_attacks.getAttack(name);
	Mana player_mana = registry.mana.get(attacker);

	return chosen_attack.manaCost <= player_mana.mana; 
}
