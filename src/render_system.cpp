// internal
#include "render_system.hpp"
#include <SDL.h>
#include <iostream>

#include "tiny_ecs_registry.hpp"

Player p;
int fbWidth;
int fbHeight;

void RenderSystem::findVar() {
    p = registry.players.get(registry.players.entities[0]);
    fbWidth = p.fBWidth;
    fbHeight = p.fBHeight;
}
// internal
#include "render_system.hpp"
#include <SDL.h>

#include "tiny_ecs_registry.hpp"

void RenderSystem::drawLightAndGlowEffect()
{
    glUseProgram(effects[(GLuint)EFFECT_ASSET_ID::GLOWING_TEXTURE]);
    gl_has_errors();

    // Clearing backbuffer
    int w, h;
    glfwGetFramebufferSize(window, &w, &h);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glViewport(0, 0, w, h);
    glDepthRange(0, 10);
    glClearColor(0, 0, 0, 1.0);
    glClearDepth(1.f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    gl_has_errors();

    // Enabling alpha channel for textures
    glEnable(GL_BLEND);
    // glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDisable(GL_DEPTH_TEST);
    // Draw the screen texture on the quad geometry
    glBindBuffer(GL_ARRAY_BUFFER, vertex_buffers[(GLuint)GEOMETRY_BUFFER_ID::SCREEN_TRIANGLE]);
    glBindBuffer(
            GL_ELEMENT_ARRAY_BUFFER,
            index_buffers[(GLuint)GEOMETRY_BUFFER_ID::SCREEN_TRIANGLE]); // Note, GL_ELEMENT_ARRAY_BUFFER associates
    // indices to the bound GL_ARRAY_BUFFER
    gl_has_errors();

    const GLuint blur_program = effects[(GLuint)EFFECT_ASSET_ID::GLOWING_TEXTURE];

    // Set clock
    GLuint time_uloc = glGetUniformLocation(blur_program, "time");
    glUniform1f(time_uloc, (float)(glfwGetTime() * 10.0f));

    //Blur_check
    GLuint blur_uloc = glGetUniformLocation(blur_program, "blur_state");
    Background& background = registry.backgrounds.get(registry.backgrounds.entities[0]);
    Entity player = registry.players.entities[0];
    glUniform1i(blur_uloc, background.blur_state);

    // Pass in player position
    Motion& motion = registry.motions.get(player);
    GLint playerPos_uloc = glGetUniformLocation(blur_program, "player_position");
    glUniform2fv(playerPos_uloc, 1, ((float *)&motion.position));

    // Pass in background sprite number
    RenderRequest& backgroundRR = registry.renderRequests.get(registry.backgrounds.entities[0]);
    GLint bg_sprite_uloc = glGetUniformLocation(blur_program, "bg_sprite");
    int bg_texture = (int)backgroundRR.used_texture;
    glUniform1i(bg_sprite_uloc, bg_texture);




    gl_has_errors();

    // Set the vertex position and vertex texture coordinates (both stored in the same VBO)
    GLint in_position_loc = glGetAttribLocation(blur_program, "in_position");
    glEnableVertexAttribArray(in_position_loc);
    glVertexAttribPointer(in_position_loc, 3, GL_FLOAT, GL_FALSE, sizeof(vec3), (void*)0);
    gl_has_errors();

    // Bind our texture in Texture Unit 0
    glActiveTexture(GL_TEXTURE0);

    glBindTexture(GL_TEXTURE_2D, off_screen_render_buffer_color);
    gl_has_errors();

    // Draw
    glDrawElements(
            GL_TRIANGLES, 3, GL_UNSIGNED_SHORT,
            nullptr); // one triangle = 3 vertices; nullptr indicates that there is
    // no offset from the bound index buffer
    gl_has_errors();
}

void RenderSystem::drawTexturedMesh(Entity entity,
                                    const mat3 &projection)
{
    Motion &motion = registry.motions.get(entity);
    Transform transform;
    transform.translate(motion.position);
    transform.scale(motion.scale);
    if (registry.animatronics.has(entity)){
        LimbGroup limbGroup = registry.animatronics.get(entity).limbGroup;
        std::map<LimbGroup, float> movementInfo = registry.animatronicMovementInfo.
                get(registry.animatronicMovementInfo.entities[0]).limbRotation;
        switch (limbGroup) {
            case (LimbGroup::LUA):
                transform.rotateWithOffset(movementInfo[LimbGroup::LUA], {0, -0.2});
            break;
            case (LimbGroup:: LLA):
                transform.rotateWithOffset(movementInfo[LimbGroup::LUA], {0, -1});
                transform.rotateWithOffset(movementInfo[LimbGroup::LLA], {0, -0.4});
            break;
            case (LimbGroup::RUA):
                transform.rotateWithOffset(movementInfo[LimbGroup::RUA], {0, -0.2});
            break;
            case (LimbGroup:: RLA):
                transform.rotateWithOffset(movementInfo[LimbGroup::RUA], {0, -1});
                transform.rotateWithOffset(movementInfo[LimbGroup::RLA], {0, -0.4});
            break;
            case (LimbGroup:: LUL):
                transform.rotateWithOffset(movementInfo[LimbGroup::LUL], {0, -0.2});
            break;
            case (LimbGroup:: LLL):
                transform.rotateWithOffset(movementInfo[LimbGroup::LUL], {0, -1});
                transform.rotateWithOffset(movementInfo[LimbGroup::LLL], {0, -0.4});
            break;
            case (LimbGroup:: RUL):
                transform.rotateWithOffset(movementInfo[LimbGroup::RUL], {0, -0.2});
                break;
            case (LimbGroup:: RLL):
                transform.rotateWithOffset(movementInfo[LimbGroup::RUL], {0, -1});
                transform.rotateWithOffset(movementInfo[LimbGroup::RLL], {0, -0.4});
            break;
        }
    }

    assert(registry.renderRequests.has(entity));
    const RenderRequest &render_request = registry.renderRequests.get(entity);

    const GLuint used_effect_enum = (GLuint)render_request.used_effect;
    assert(used_effect_enum != (GLuint)EFFECT_ASSET_ID::EFFECT_COUNT);
    const GLuint program = (GLuint)effects[used_effect_enum];

    // Setting shaders
    glUseProgram(program);
    gl_has_errors();

    assert(render_request.used_geometry != GEOMETRY_BUFFER_ID::GEOMETRY_COUNT);
    const GLuint vbo = vertex_buffers[(GLuint)render_request.used_geometry];
    const GLuint ibo = index_buffers[(GLuint)render_request.used_geometry];

    // Setting vertex and index buffers
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
    gl_has_errors();

    // Input data location as in the vertex buffer
    if (render_request.used_effect == EFFECT_ASSET_ID::TEXTURED ||
        render_request.used_effect == EFFECT_ASSET_ID::HELP_SCREEN ||
        render_request.used_effect == EFFECT_ASSET_ID::TEXTURED_ANIMATION)
    {
        GLint in_position_loc = glGetAttribLocation(program, "in_position");
        GLint in_texcoord_loc = glGetAttribLocation(program, "in_texcoord");
        gl_has_errors();
        assert(in_texcoord_loc >= 0);

        glEnableVertexAttribArray(in_position_loc);
        glVertexAttribPointer(in_position_loc, 3, GL_FLOAT, GL_FALSE,
                              sizeof(TexturedVertex), (void *)0);
        gl_has_errors();

        glEnableVertexAttribArray(in_texcoord_loc);
        glVertexAttribPointer(
                in_texcoord_loc, 2, GL_FLOAT, GL_FALSE, sizeof(TexturedVertex),
                (void *)sizeof(
                        vec3)); // note the stride to skip the preceeding vertex position
        // Enabling and binding texture to slot 0
        glActiveTexture(GL_TEXTURE0);
        gl_has_errors();

        assert(registry.renderRequests.has(entity));
        GLuint texture_id =
                texture_gl_handles[(GLuint)registry.renderRequests.get(entity).used_texture];

        glBindTexture(GL_TEXTURE_2D, texture_id);
        gl_has_errors();

        if (render_request.used_effect == EFFECT_ASSET_ID::TEXTURED_ANIMATION) {
            drawAnimatedSprite(entity);
        }
    }
    else if (render_request.used_effect == EFFECT_ASSET_ID::SHADOW) {
        GLint in_position_loc = glGetAttribLocation(program, "in_position");
        glEnableVertexAttribArray(in_position_loc);
        glVertexAttribPointer(in_position_loc, 3, GL_FLOAT, GL_FALSE,
            sizeof(ColoredVertex), (void*)0);
        gl_has_errors();

        GLint is_mac_os_uloc = glGetUniformLocation(program, "is_mac_os");
        assert(is_mac_os_uloc >= 0);
        float isMacOS = 0;
#ifdef __APPLE__
        isMacOS = 1;
#endif
        glUniform1f(is_mac_os_uloc, isMacOS);
        gl_has_errors();

        GLint in_color_loc = glGetAttribLocation(program, "in_color");
        glEnableVertexAttribArray(in_color_loc);
        glVertexAttribPointer(in_color_loc, 3, GL_FLOAT, GL_FALSE,
            sizeof(ColoredVertex), (void*)sizeof(vec3));
        gl_has_errors();

        // Pass in player position
        Entity player = registry.players.entities[0];
        Motion& motion = registry.motions.get(player);
        GLint playerPos_uloc = glGetUniformLocation(program, "player_position");
        glUniform2fv(playerPos_uloc, 1, ((float*)&motion.position));
        gl_has_errors();

        // Pass in Shadow stuff
        Motion& motion_shadow = registry.motions.get(entity);
        GLint shadow_size_uloc = glGetUniformLocation(program, "shadow_size");
        glUniform2fv(shadow_size_uloc, 1, ((float*)&motion_shadow.scale));
        GLint shadow_pos = glGetUniformLocation(program, "shadow_pos");
        glUniform2fv(shadow_pos, 1, ((float*)&motion_shadow.position));
        GLint shadow_num = glGetUniformLocation(program, "shadow_num");
        glUniform1i(shadow_num, registry.shadow.get(entity).num);
        

        gl_has_errors();


    }
    else if (render_request.used_effect == EFFECT_ASSET_ID::PEBBLE ||
        render_request.used_effect == EFFECT_ASSET_ID::HEALTHBAR_PLAYER ||
        render_request.used_effect == EFFECT_ASSET_ID::HEALTHBAR_ENEMY)
    {
        GLint in_position_loc = glGetAttribLocation(program, "in_position");
        GLint in_color_loc = glGetAttribLocation(program, "in_color");
        gl_has_errors();

        glEnableVertexAttribArray(in_position_loc);
        glVertexAttribPointer(in_position_loc, 3, GL_FLOAT, GL_FALSE,
                              sizeof(ColoredVertex), (void *)0);
        gl_has_errors();

        if (render_request.used_effect == EFFECT_ASSET_ID::HEALTHBAR_PLAYER) {
            GLint player_pct_uloc = glGetUniformLocation(program, "player_health_pct");
            assert(player_pct_uloc >= 0);
            float player_health_pct = 0;
            Health& player_health = registry.health.get(registry.players.entities[0]);
                if (player_health.maxHP > 0) {
                    player_health_pct = (float) player_health.health / (float) player_health.maxHP;
#ifdef __APPLE__
                    player_health_pct += 0.5;
#endif
                }
            glUniform1f(player_pct_uloc, player_health_pct);
            GLint is_mac_os_uloc = glGetUniformLocation(program, "is_mac_os");
            assert(is_mac_os_uloc >= 0);
            float isMacOS = 0;
#ifdef __APPLE__
            isMacOS = 1;
#endif
            glUniform1f(is_mac_os_uloc, isMacOS);
            gl_has_errors();

            GLint player_mana_pct_uloc = glGetUniformLocation(program, "player_mana_pct");
            assert(player_mana_pct_uloc >= 0);
            float player_mana_pct = 0;
            Mana& player_mana = registry.mana.get(registry.players.entities[0]);
            if (player_mana.maxMana > 0) {
                player_mana_pct = (float)player_mana.mana / (float)player_mana.maxMana;
#ifdef __APPLE__
                player_mana_pct += 0.5;
#endif
            }
            glUniform1f(player_mana_pct_uloc, player_mana_pct);
            gl_has_errors();
        }
        else if (render_request.used_effect == EFFECT_ASSET_ID::HEALTHBAR_ENEMY) {
            GLint enemy_pct_uloc = glGetUniformLocation(program, "enemy_health_pct");
            assert(enemy_pct_uloc >= 0);
            float enemy_health_pct = 0;
            Health& enemy_health = registry.health.get(registry.currentEnemies.entities[0]);
            if (enemy_health.maxHP > 0) {
                enemy_health_pct = (float)enemy_health.health / (float)enemy_health.maxHP;
            }
#ifdef __APPLE__
            enemy_health_pct += 0.5;
#endif

            glUniform1f(enemy_pct_uloc, enemy_health_pct);
        }
    }
    else
    {
        assert(false && "Type of render request not supported");
    }

    // Getting uniform locations for glUniform* calls
    GLint color_uloc = glGetUniformLocation(program, "fcolor");
    const vec3 color = registry.colors.has(entity) ? registry.colors.get(entity) : vec3(1);
    glUniform3fv(color_uloc, 1, (float *)&color);
    gl_has_errors();

    // Get number of indices from index buffer, which has elements uint16_t
    GLint size = 0;
    glGetBufferParameteriv(GL_ELEMENT_ARRAY_BUFFER, GL_BUFFER_SIZE, &size);
    gl_has_errors();

    GLsizei num_indices = size / sizeof(uint16_t); 
    // GLsizei num_triangles = num_indices / 3;

    GLint currProgram;
    glGetIntegerv(GL_CURRENT_PROGRAM, &currProgram);
    // Setting uniform values to the currently bound program
    GLuint transform_loc = glGetUniformLocation(currProgram, "transform");
    glUniformMatrix3fv(transform_loc, 1, GL_FALSE, (float *)&transform.mat);
    GLuint projection_loc = glGetUniformLocation(currProgram, "projection");
    glUniformMatrix3fv(projection_loc, 1, GL_FALSE, (float *)&projection);
    gl_has_errors();
    // Drawing of num_indices/3 triangles specified in the index buffer
    glDrawElements(GL_TRIANGLES, num_indices, GL_UNSIGNED_SHORT, nullptr);
    gl_has_errors();
}

// Draw the intermediate texture to the screen, with some potentially some blur depending onthe state
// Blur
void RenderSystem::drawToScreen()
{
    // Setting shaders
    // get the BLUR texture, sprite mesh, and program
    glUseProgram(effects[(GLuint)EFFECT_ASSET_ID::BLUR]);
    gl_has_errors();

    // Clearing backbuffer
    int w, h;
    glfwGetFramebufferSize(window, &w, &h);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glViewport(0, 0, w, h);
    glDepthRange(0, 10);
    glClearColor(0.f, 0, 0, 1.0);
    glClearDepth(1.f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    gl_has_errors();

    // Enabling alpha channel for textures
    glEnable(GL_BLEND);
    // glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDisable(GL_DEPTH_TEST);
    // Draw the screen texture on the quad geometry
    glBindBuffer(GL_ARRAY_BUFFER, vertex_buffers[(GLuint)GEOMETRY_BUFFER_ID::SCREEN_TRIANGLE]);
    glBindBuffer(
            GL_ELEMENT_ARRAY_BUFFER,
            index_buffers[(GLuint)GEOMETRY_BUFFER_ID::SCREEN_TRIANGLE]); // Note, GL_ELEMENT_ARRAY_BUFFER associates
    // indices to the bound GL_ARRAY_BUFFER
    gl_has_errors();

    const GLuint blur_program = effects[(GLuint)EFFECT_ASSET_ID::BLUR];

    // Set clock // For future potential implementations
    GLuint time_uloc = glGetUniformLocation(blur_program, "time");

    //Blur_check
    GLuint blur_uloc = glGetUniformLocation(blur_program, "blur_state");
    Background& background = registry.backgrounds.get(registry.backgrounds.entities[0]);
    glUniform1i(blur_uloc, background.blur_state);
    glUniform1f(time_uloc, (float)(glfwGetTime() * 10.0f));
    gl_has_errors();

    // Set the vertex position and vertex texture coordinates (both stored in the same VBO)
    GLint in_position_loc = glGetAttribLocation(blur_program, "in_position");
    glEnableVertexAttribArray(in_position_loc);
    glVertexAttribPointer(in_position_loc, 3, GL_FLOAT, GL_FALSE, sizeof(vec3), (void *)0);
    gl_has_errors();

    // Bind our texture in Texture Unit 0
    glActiveTexture(GL_TEXTURE0);

    glBindTexture(GL_TEXTURE_2D, off_screen_render_buffer_color);
    gl_has_errors();

    // Draw
    glDrawElements(
            GL_TRIANGLES, 3, GL_UNSIGNED_SHORT,
            nullptr); // one triangle = 3 vertices; nullptr indicates that there is
    // no offset from the bound index buffer
    gl_has_errors();

    drawLightAndGlowEffect();

}

void RenderSystem::drawAnimatedSprite(Entity entity) {
    AnimatedSprite& animated_sprite = registry.animatedSprites.get(entity);
    GLuint textured_animation_program = effects[(GLuint)EFFECT_ASSET_ID::TEXTURED_ANIMATION];
    glUseProgram(textured_animation_program);

    GLint frame_height_uloc = glGetUniformLocation(textured_animation_program, "frame_height");
    glUniform1f(frame_height_uloc, animated_sprite.frame_height);
    GLint frame_width_uloc = glGetUniformLocation(textured_animation_program, "frame_width");
    glUniform1f(frame_width_uloc, animated_sprite.frame_width);

    GLint animation_frame_uloc = glGetUniformLocation(textured_animation_program, "frame");
    glUniform1f(animation_frame_uloc, (float)animated_sprite.current_frame);

    GLint animation_type_uloc = glGetUniformLocation(textured_animation_program, "animation_type");
    glUniform1f(animation_type_uloc, (float)animated_sprite.current_type);

   
    GLint enemy_position_uloc = glGetUniformLocation(textured_animation_program, "enemy_position");
    vec2 pos = registry.motions.get(entity).position;
    glUniform2fv(enemy_position_uloc, 1, (float *) &pos);

    GLint is_squished_uloc = glGetUniformLocation(textured_animation_program, "is_squished");
    GLint squish_countdown_uloc = glGetUniformLocation(textured_animation_program, "squish_countdown");

    const RenderRequest& render_request = registry.renderRequests.get(entity);

    if (registry.squished.has(entity) && registry.battleEnemies.has(entity) && render_request.used_texture == TEXTURE_ASSET_ID::ENEMIES) {
        glUniform1i(is_squished_uloc, (int) 1);

        float squish_countdown = registry.squished.get(entity).countdown;
        glUniform1f(squish_countdown_uloc, (float)squish_countdown);
        if (squish_countdown <= 0) {
            registry.squished.remove(entity);
        }
        else {
            Squish& squish = registry.squished.get(entity);
            squish.countdown--;
        }
    }
    else {
        glUniform1i(is_squished_uloc, (int) 0);
        glUniform1f(squish_countdown_uloc, (float) 0);
    }

    gl_has_errors();
}

void RenderSystem::horizontalBlur()
{
    // Setting shaders
    // get the BLUR texture, sprite mesh, and program
    glUseProgram(effects[(GLuint)EFFECT_ASSET_ID::REBLUR]);
    gl_has_errors();

    // Clearing backbuffer
    int w, h;
    glfwGetFramebufferSize(window, &w, &h);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glViewport(0, 0, w, h);
    glDepthRange(0, 10);
    glClearColor(1.f, 0, 0, 1.0);
    glClearDepth(1.f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    gl_has_errors();

    // Enabling alpha channel for textures
    glEnable(GL_BLEND);
    // glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDisable(GL_DEPTH_TEST);
    // Draw the screen texture on the quad geometry
    glBindBuffer(GL_ARRAY_BUFFER, vertex_buffers[(GLuint)GEOMETRY_BUFFER_ID::SCREEN_TRIANGLE]);
    glBindBuffer(
            GL_ELEMENT_ARRAY_BUFFER,
            index_buffers[(GLuint)GEOMETRY_BUFFER_ID::SCREEN_TRIANGLE]); // Note, GL_ELEMENT_ARRAY_BUFFER associates
    // indices to the bound GL_ARRAY_BUFFER
    gl_has_errors();

    const GLuint blur_program = effects[(GLuint)EFFECT_ASSET_ID::REBLUR];

    // Set clock // For future potential implementations
    GLuint time_uloc = glGetUniformLocation(blur_program, "time");

    //Blur_check
    GLuint blur_uloc = glGetUniformLocation(blur_program, "blur_state");
    Background& background = registry.backgrounds.get(registry.backgrounds.entities[0]);
    glUniform1i(blur_uloc, background.blur_state);
    glUniform1f(time_uloc, (float)(glfwGetTime() * 10.0f));
    gl_has_errors();

    // Set the vertex position and vertex texture coordinates (both stored in the same VBO)
    GLint in_position_loc = glGetAttribLocation(blur_program, "in_position");
    glEnableVertexAttribArray(in_position_loc);
    glVertexAttribPointer(in_position_loc, 3, GL_FLOAT, GL_FALSE, sizeof(vec3), (void*)0);
    gl_has_errors();

    // Bind our texture in Texture Unit 0
    glActiveTexture(GL_TEXTURE0);

    glBindTexture(GL_TEXTURE_2D, off_screen_render_buffer_color);
    gl_has_errors();

    // Draw
    glDrawElements(
            GL_TRIANGLES, 3, GL_UNSIGNED_SHORT,
            nullptr); // one triangle = 3 vertices; nullptr indicates that there is
    // no offset from the bound index buffer
    gl_has_errors();
}

void RenderSystem::drawOverlayWindow(Entity entity,
                                     const mat3 &projection)
{
    Motion &motion = registry.motions.get(entity);
    Transform transform;
    transform.translate(motion.position);
    transform.scale(motion.scale);
    if (registry.animatronics.has(entity)){
        LimbGroup limbGroup = registry.animatronics.get(entity).limbGroup;
        std::map<LimbGroup, float> movementInfo = registry.animatronicMovementInfo.
                get(registry.animatronicMovementInfo.entities[0]).limbRotation;
        switch (limbGroup) {
            case (LimbGroup::LUA):
                transform.rotateWithOffset(movementInfo[LimbGroup::LUA], {0, -0.2});
                break;
            case (LimbGroup:: LLA):
                transform.rotateWithOffset(movementInfo[LimbGroup::LUA], {0, -0.8});
                transform.rotateWithOffset(movementInfo[LimbGroup::LLA], {0, -0.4});
                break;
            case (LimbGroup::RUA):
                transform.rotateWithOffset(movementInfo[LimbGroup::RUA], {0, -0.2});
                break;
            case (LimbGroup:: RLA):
                transform.rotateWithOffset(movementInfo[LimbGroup::RUA], {0, -0.8});
                transform.rotateWithOffset(movementInfo[LimbGroup::RLA], {0, -0.4});
                break;
            case (LimbGroup:: LUL):
                transform.rotateWithOffset(movementInfo[LimbGroup::LUL], {0, -0.2});
                break;
            case (LimbGroup:: LLL):
                transform.rotateWithOffset(movementInfo[LimbGroup::LUL], {0, -1});
                transform.rotateWithOffset(movementInfo[LimbGroup::LLL], {0, -0.4});
                break;
            case (LimbGroup:: RUL):
                transform.rotateWithOffset(movementInfo[LimbGroup::RUL], {0, -0.2});
                break;
            case (LimbGroup:: RLL):
                transform.rotateWithOffset(movementInfo[LimbGroup::RUL], {0, -1});
                transform.rotateWithOffset(movementInfo[LimbGroup::RLL], {0, -0.4});
                break;
        }
    }

    assert(registry.renderRequests.has(entity));
    const RenderRequest &render_request = registry.renderRequests.get(entity);

    const GLuint used_effect_enum = (GLuint)render_request.used_effect;
    assert(used_effect_enum != (GLuint)EFFECT_ASSET_ID::EFFECT_COUNT);
    const GLuint program = (GLuint)effects[used_effect_enum];

    // Setting shaders
    glUseProgram(program);
    gl_has_errors();

    assert(render_request.used_geometry != GEOMETRY_BUFFER_ID::GEOMETRY_COUNT);
    const GLuint vbo = vertex_buffers[(GLuint)render_request.used_geometry];
    const GLuint ibo = index_buffers[(GLuint)render_request.used_geometry];

    // Setting vertex and index buffers
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
    gl_has_errors();

    if (render_request.used_effect == EFFECT_ASSET_ID::HELP_SCREEN
    || render_request.used_effect == EFFECT_ASSET_ID::TEXTURED_ANIMATION
    || render_request.used_effect == EFFECT_ASSET_ID::TEXTURED) {
        GLint in_position_loc = glGetAttribLocation(program, "in_position");
        GLint in_texcoord_loc = glGetAttribLocation(program, "in_texcoord");
        gl_has_errors();
        assert(in_texcoord_loc >= 0);

        glEnableVertexAttribArray(in_position_loc);
        glVertexAttribPointer(in_position_loc, 3, GL_FLOAT, GL_FALSE,
                              sizeof(TexturedVertex), (void *)0);
        gl_has_errors();

        glEnableVertexAttribArray(in_texcoord_loc);
        glVertexAttribPointer(
                in_texcoord_loc, 2, GL_FLOAT, GL_FALSE, sizeof(TexturedVertex),
                (void *)sizeof(
                        vec3)); // note the stride to skip the preceeding vertex position
        // Enabling and binding texture to slot 0
        glActiveTexture(GL_TEXTURE0);
        gl_has_errors();

        assert(registry.renderRequests.has(entity));
        GLuint texture_id =
                texture_gl_handles[(GLuint)registry.renderRequests.get(entity).used_texture];

        glBindTexture(GL_TEXTURE_2D, texture_id);
        gl_has_errors();
    }

    // Get number of indices from index buffer, which has elements uint16_t
    GLint size = 0;
    glGetBufferParameteriv(GL_ELEMENT_ARRAY_BUFFER, GL_BUFFER_SIZE, &size);
    gl_has_errors();

    GLsizei num_indices = size / sizeof(uint16_t);
    // GLsizei num_triangles = num_indices / 3;

    GLint currProgram;
    glGetIntegerv(GL_CURRENT_PROGRAM, &currProgram);
    // Setting uniform values to the currently bound program
    GLuint transform_loc = glGetUniformLocation(currProgram, "transform");
    glUniformMatrix3fv(transform_loc, 1, GL_FALSE, (float *)&transform.mat);
    GLuint projection_loc = glGetUniformLocation(currProgram, "projection");
    glUniformMatrix3fv(projection_loc, 1, GL_FALSE, (float *)&projection);
    gl_has_errors();
    // Drawing of num_indices/3 triangles specified in the index buffer
    glDrawElements(GL_TRIANGLES, num_indices, GL_UNSIGNED_SHORT, nullptr);
    gl_has_errors();
}

// Render our game world
// http://www.opengl-tutorial.org/intermediate-tutorials/tutorial-14-render-to-texture/
void RenderSystem::draw()
{
    findVar();
    // Getting size of window
    int w, h;
    glfwGetFramebufferSize(window, &w, &h);

    // First render to the custom framebuffer
    glBindFramebuffer(GL_FRAMEBUFFER, frame_buffer);
    gl_has_errors();
    // Clearing backbuffer
    glViewport(0, 0, fbWidth, fbHeight);
    glDepthRange(0.00001, 10);
    glClearColor(0, 1, 1, 1.0); // background color
    glClearDepth(1.f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDisable(GL_DEPTH_TEST); // native OpenGL does not work with a depth buffer
    // and alpha blending, one would have to sort
    // sprites back to front
    gl_has_errors();
    mat3 projection_2D = createProjectionMatrix();
    // Draw all textured meshes that have a position and size component
    for (Entity entity : registry.renderRequests.entities)
    {
        if (!registry.motions.has(entity))
            continue;

        drawTexturedMesh(entity, projection_2D);
    }

    // Truely render to the screen
    drawToScreen();
    for (Entity entity : registry.renderRequests.entities) {
        // help screen
        if (isInOverlayWindow1(entity)) {
            drawOverlayWindow(entity, projection_2D);
        }
    }
    for (Entity entity : registry.renderRequests.entities) {
        // battle screen
        if (isInOverlayWindow2(entity)) {
            drawOverlayWindow(entity, projection_2D);
        }
    }
    for (Entity entity : registry.renderRequests.entities) {
        // battle doll
        if (isInOverlayWindow3(entity)) {
            drawOverlayWindow(entity, projection_2D);
        }
    }
    for (Entity entity : registry.renderRequests.entities) {
        // battle menu buttons
        if (isInOverlayWindow4(entity)) {
            drawOverlayWindow(entity, projection_2D);
        }
        // popups -> dialogue
        if (registry.popups.has(entity)) {
            drawOverlayWindow(entity, projection_2D);
        }
        // enemy moves on battle screen
        if (registry.battleUsedMoves.has(entity)) {
            drawOverlayWindow(entity, projection_2D);
        }
    }
    for (Entity entity : registry.renderRequests.entities) {
        if (isInOverlayWindow5(entity)) {
            drawOverlayWindow(entity, projection_2D);
        }
    }    // flicker-free display with a double buffer
    glfwSwapBuffers(window);
    gl_has_errors();
}

bool RenderSystem::isInOverlayWindow1(Entity entity) {
    return (registry.helpScreens.has(entity));
}

bool RenderSystem::isInOverlayWindow2(Entity entity) {
    return (registry.battleScreens.has(entity));
}


bool RenderSystem::isInOverlayWindow3(Entity entity) {
    return (registry.battleDolls.has(entity)
                || registry.battleEnemies.has(entity) || registry.battleMenus.has(entity));
}

bool RenderSystem::isInOverlayWindow4(Entity entity) {
    return (registry.battleMenuButtons.has(entity) ||
            registry.battleMenuPlayerMoves.has(entity));
}

bool RenderSystem::isInOverlayWindow5(Entity entity) {
    return (registry.healthbars.has(entity));
}


mat3 RenderSystem::specialProjectionMatrix()
{
    // Fake projection matrix, scales with respect to window coordinates
    float left = 0.f;
    float top = 0.f;

    gl_has_errors();
    float right = (float) 1;
    float bottom = (float) 1;

    float sx = 2.f / (right - left);
    float sy = 2.f / (top - bottom);
    float tx = -(right + left) / (right - left);
    float ty = -(top + bottom) / (top - bottom);
    return {{sx, 0.f, 0.f}, {0.f, sy, 0.f}, {tx, ty, 1.f}};
}

mat3 RenderSystem::createProjectionMatrix()
{
    // Fake projection matrix, scales with respect to window coordinates
    float left = 0.f;
    float top = 0.f;

    gl_has_errors();
    float right = (float) window_width_px;
    float bottom = (float) window_height_px;

    float sx = 2.f / (right - left);
    float sy = 2.f / (top - bottom);
    float tx = -(right + left) / (right - left);
    float ty = -(top + bottom) / (top - bottom);
    return {{sx, 0.f, 0.f}, {0.f, sy, 0.f}, {tx, ty, 1.f}};
}