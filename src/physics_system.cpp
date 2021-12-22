// internal
#include <iostream>
#include "physics_system.hpp"
#include "world_init.hpp"

// Returns the local bounding coordinates scaled by the current size of the entity
vec2 get_bounding_box(const Motion& motion)
{
    // abs is to avoid negative scale due to the facing direction.
    return { abs(motion.scale.x), abs(motion.scale.y) };
}

// This is a SUPER APPROXIMATE check that puts a circle around the bounding boxes and sees
// if the center point of either object is inside the other's bounding-box-circle. You can
// surely implement a more accurate detection
bool collides(const Motion& motion1, const Motion& motion2)
{
    vec2 dp = motion1.position - motion2.position;
    float dist_squared = dot(dp,dp);
    const vec2 other_bonding_box = get_bounding_box(motion1) / 2.f;
    const float other_r_squared = dot(other_bonding_box, other_bonding_box);
    const vec2 my_bonding_box = get_bounding_box(motion2) / 2.f;
    const float my_r_squared = dot(my_bonding_box, my_bonding_box);
    const float r_squared = max(other_r_squared, my_r_squared);
    if (dist_squared < r_squared)
        return true;
    return false;
}

// This whole function has duplicated code from handleWallCollision, should be refactored.
bool checkWallCollision(Entity entity, Entity wall) {
    Motion& entity_motion = registry.motions.get(entity);
    vec2 entity_bounding_box = get_bounding_box(entity_motion) / 2.0f;

    Motion& wall_motion = registry.motions.get(wall);
    vec2 wall_bounding_box = get_bounding_box(wall_motion) / 2.0f;

    vec2 player_min = { entity_motion.position[0] - entity_bounding_box[0], entity_motion.position[1] + entity_bounding_box[1] };
    vec2 player_max = { entity_motion.position[0] + entity_bounding_box[0], entity_motion.position[1] - entity_bounding_box[1] };
    vec2 wall_min = { wall_motion.position[0] - wall_bounding_box[0], wall_motion.position[1] + wall_bounding_box[1] };
    vec2 wall_max = { wall_motion.position[0] + wall_bounding_box[0], wall_motion.position[1] - wall_bounding_box[1] };

    if ((player_min[0] < wall_max[0]) && (wall_min[0] < player_max[0]) && (player_min[1] > wall_max[1]) && (wall_min[1] > player_min[1])) {
        return true;
    }
    return false;
}

void checkFakeCollision(Entity e, bool forAI) {
    ComponentContainer<Motion>& motion_container = registry.motions;
    Motion m = registry.motions.get(e);
    std::vector<Entity> entities = registry.motions.entities;

    for (uint i = 0; i < motion_container.components.size(); i++)
    {
        Motion& motion_i = motion_container.components[i];
        if (motion_i.collision_proof == 1 || (unsigned int)entities[i] == (unsigned int)e || registry.debugComponents.has(entities[i])) {
            continue;
        }

        if (forAI && registry.players.has(entities[i])) {
            continue;
        }

        // these two if blocks are pretty clumsy, might want to refactor
        if (registry.walls.has(entities[i])) {
            if (checkWallCollision(e, entities[i])) {
                if (forAI && registry.AIEntities.has(entities[i])) {
                    AIEntity& ai = registry.AIEntities.get(entities[i]);
                    ai.coveredPositions.push_back(std::make_pair(
                            round(m.position.x / 50.f),
                            round(m.position.y / 50.f)
                    ));
                }

                registry.remove_all_components_of(e);
                return;
            }
            else {
                continue;
            }
        }

        if (collides(motion_i, m))
        {
            if (forAI && registry.AIEntities.has(entities[i])) {
                AIEntity& ai = registry.AIEntities.get(entities[i]);
                ai.coveredPositions.push_back(std::make_pair(
                        round(m.position.x / 50.f),
                        round(m.position.y / 50.f)
                ));
            }

            registry.remove_all_components_of(e);
            return;
        }
    }
}

std::function<void(Entity, bool)> PhysicsSystem::getCollisionFunction() {
    return checkFakeCollision;
}


void PhysicsSystem::step(float elapsed_ms, float window_width_px, float window_height_px)
{
    // Move entity based on how much time has passed, this is to (partially) avoid
    // having entities move at different speed based on the machine.
    auto& motion_registry = registry.motions;
    ComponentContainer<Motion>& motion_container = registry.motions;

    if (registry.game.get(registry.players.entities[0]).state == GameState::PLAYING) {
        for (uint i = 0; i < motion_registry.size(); i++)
        {
            Motion& motion = motion_registry.components[i];
            Entity entity = motion_registry.entities[i];
            float step_seconds = 1.0f * (elapsed_ms / 1000.f);
            // Check that step does not take player out of bounds
            vec2 potential_pos = motion.position + (motion.velocity * step_seconds);
            if (potential_pos[0] > 40 && potential_pos[0] < window_width_px  - 40 && potential_pos[1] > 40 && potential_pos[1] < window_height_px - 40) {
                motion.position = motion.position + (motion.velocity * step_seconds);
            }
        }

        // Check for collisions between all moving entities
        for (uint i = 0; i < motion_container.components.size(); i++)
        {
            Motion& motion_i = motion_container.components[i];
            Entity entity_i = motion_container.entities[i];
            for (uint j = 0; j < motion_container.components.size(); j++) // i+1
            {
                if (i == j)
                    continue;

                Entity entity_j = motion_container.entities[j];
                Motion& motion_j = motion_container.components[j];

                if (registry.players.has(entity_i) && registry.walls.has(entity_j)) {
                    handleWallCollision(entity_i, entity_j);
                    continue;
                }
                else if (registry.walls.has(entity_i) && registry.players.has(entity_j)) {
                    handleWallCollision(entity_j, entity_i);
                    continue;
                }

                if (collides(motion_i, motion_j))
                {
                    Entity entity_j = motion_container.entities[j];
                    // Create a collisions event
                    // We are abusing the ECS system a bit in that we potentially insert muliple collisions for the same entity
                    registry.collisions.emplace_with_duplicates(entity_i, entity_j);
                    registry.collisions.emplace_with_duplicates(entity_j, entity_i);
                }
            }
        }
    }

    // Move parallax background
    GameState gs = registry.game.get(registry.players.entities[0]).state;
    if (gs == GameState::BATTLE || gs == GameState::BOSS) {
        ComponentContainer<BattleParallaxBackground>& parallaxRegistry = registry.parallaxBackground;
        for (uint i = 0; i < parallaxRegistry.components.size(); i++){
            Entity parallaxEntity = parallaxRegistry.entities[i];
            Motion& parallaxMotion = registry.motions.get(parallaxEntity);
            float step_seconds = 1.0f * (elapsed_ms / 1000.f);
            // Check that step does not take player out of bounds
            vec2 potential_pos = parallaxMotion.position + (parallaxMotion.velocity * step_seconds);
            parallaxMotion.position = parallaxMotion.position + (parallaxMotion.velocity * step_seconds);
        }
    }

    // you may need the following quantities to compute wall positions
    (float)window_width_px; (float)window_height_px;

    // debugging of bounding boxes
    if (debugging.in_debug_mode)
    {

        // create a red line around the wall boxes
        for (Entity wallBox: registry.walls.entities) {
            Motion& wallMotion = registry.motions.get(wallBox);
            createBox(wallMotion.position, wallMotion.scale);
        }
        Entity playerDoll = registry.players.entities[0];
        Motion& dollMotion = registry.motions.get(playerDoll);
        createBox(dollMotion.position, dollMotion.scale);

        for (Entity enemy: registry.enemies.entities) {
            Motion& enemyMotion = registry.motions.get(enemy);
            createBox(enemyMotion.position, enemyMotion.scale);
        }

        // debug for shadow
        if (registry.shadow.size() > 0) {

            // Mesh Debug Display Mesh Dots
            Mesh*& mesh = registry.meshPtrs.get(registry.shadow.entities[0]);
            Motion& mesh_m = registry.motions.get(registry.shadow.entities[0]);

            // Transformation matrix
            Transform trans;
            trans.translate(mesh_m.position);
            trans.rotate(mesh_m.angle);
            trans.scale(mesh_m.scale);

            auto vertices_size = mesh->vertices.size();
            for (uint i = 0; i < vertices_size; i++) {
                // Position using the local to global
                vec3 position = trans.mat * vec3(mesh->vertices[i].position.x, mesh->vertices[i].position.y, 1.0);
                Entity vertex_debug = createLine(vec2(position.x, position.y), vec2(5, 5));
            }

        }
    }


}

void PhysicsSystem::handleWallCollision(Entity player, Entity wall) {
    Motion& player_motion = registry.motions.get(player);
    vec2 player_bounding_box = get_bounding_box(player_motion) / 2.0f;
    // temporary hack to account for whitespace
    player_bounding_box = { player_bounding_box[0] - 30, player_bounding_box[1] - 30 };

    Motion& wall_motion = registry.motions.get(wall);
    vec2 wall_bounding_box = get_bounding_box(wall_motion) / 2.0f;

    vec2 player_min = { player_motion.position[0] - player_bounding_box[0], player_motion.position[1] + player_bounding_box[1] };
    vec2 player_max = { player_motion.position[0] + player_bounding_box[0], player_motion.position[1] - player_bounding_box[1] };
    vec2 wall_min = { wall_motion.position[0] - wall_bounding_box[0], wall_motion.position[1] + wall_bounding_box[1] };
    vec2 wall_max = { wall_motion.position[0] + wall_bounding_box[0], wall_motion.position[1] - wall_bounding_box[1] };

    if ((player_min[0] < wall_max[0]) && (wall_min[0] < player_max[0]) && (player_min[1] > wall_max[1]) && (wall_min[1] > player_min[1])) {
        if (player_motion.dir == Direction::UP) {
            float overlap = wall_min[1] - player_min[1];
            player_motion.position[1] = player_motion.position[1] + overlap;
        }
        else if (player_motion.dir == Direction::DOWN) {
            float overlap = player_min[1] - wall_max[1];
            player_motion.position[1] = player_motion.position[1] - overlap;
        }
        else if (player_motion.dir == Direction::LEFT) {
            float overlap = wall_max[0] - player_min[0];
            player_motion.position[0] = player_motion.position[0] + overlap;
        }
        else if (player_motion.dir == Direction::RIGHT) {
            float overlap = player_max[0] - wall_min[0];
            player_motion.position[0] = player_motion.position[0] - overlap;
        }
    }
    return;
}