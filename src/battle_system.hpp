#pragma once

#include <vector>

#include "world_system.hpp"
#include "tiny_ecs_registry.hpp"
#include "common.hpp"


class BattleSystem
{
public:

    void init(WorldSystem* world_system_arg);

    // run the battle 
    void handle_battle(float elapsed_ms);
    Turn selectEnemyMove(Entity& player, Entity& enemy);

private:
    TreeNode * advancedDecisionTree = nullptr;

    BattleMove process_player_turn(Turn& turn);

    BattleMove process_enemy_turn(Turn& turn, Entity& enemy);

    void end_battle(Game& game, std::vector<Entity> current_enemies);

    WorldSystem* world_system;

    void buildTree(Entity& player, Entity& enemy, int level, bool turn, TreeNode * parent);

    void buildNode(TreeNode * tn, Entity& e, bool turn);

    TreeNode * getRoot(Entity& player, Entity& enemy);

    void freeTree(TreeNode * currNode);

    float DFS(TreeNode * curr, Entity& player, Entity& enemy);

    // TreeNode * getEnemyMove(TreeNode * tn);

    float scoreState(Entity& player, Entity& enemy);
};
