#include "ogp_utils.h"

#include "ogp_cloth.h"
#include "ogp_debug_draw.h"
#include "ogp_fx.h"
#include "ogp_input.h"
#include "ogp_opengl.h"
#include "ogp_messages.h"
#include "ogp_models.h"
#include "ogp_mesh.h"
#include "ogp_player.h"
#include "ogp_render.h"
#include "ogp_settings.h"
#include "ogp_shader.h"
#include "ogp_sdl.h"

#include <chrono>

using namespace ogp;

int main(int argc, char *argv[])
{
    (void) argc;
    (void) argv;

    ogp_log_info("nailminator 0.1.0");
    print_working_dir();
    print_sdl_info();

    ogp_log_debug("delta.s  = %g", settings.delta.s);

    SDL_Window *window = nullptr;
    SDL_GLContext glcontext = nullptr;

    u32 init_flags   = SDL_INIT_EVERYTHING;
    u32 window_flags = SDL_WINDOW_OPENGL
                     | SDL_WINDOW_SHOWN
                     | SDL_WINDOW_RESIZABLE;

    if (settings.fullscreen) {
        window_flags |= SDL_WINDOW_FULLSCREEN;
    }

    std::set_terminate(Mr_Terminator);

    if (SDL_Init(init_flags) != 0) {
        ogp_log_error("cannot initialize sdl: %s", SDL_GetError());
        return -1;
    }

    // NULL - open the default OpenGL library
    SDL_GL_LoadLibrary(NULL);

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1);

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

    SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);

    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

    // SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
    // SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 4);

    window = SDL_CreateWindow("OpenGL Playground",
                              SDL_WINDOWPOS_CENTERED,
                              SDL_WINDOWPOS_CENTERED,
                              settings.size.window.x,
                              settings.size.window.y,
                              window_flags);

    if (window == NULL) {
        ogp_log_error("cannot create window: %s", SDL_GetError());
        return -1;
    }

    glcontext = SDL_GL_CreateContext(window);
    SDL_GL_MakeCurrent(window, glcontext);

    // 0 for immediate updates
    // 1 for updates synchronized with the vertical retrace
    // -1 for late swap tearing; see Remarks
    SDL_GL_SetSwapInterval(1);

    glewExperimental = GL_TRUE;  // mac
    if (glewInit() != GLEW_OK) {
        glGetError();
        ogp_log_error("cannot initialize GLEW");
        return -1;
    }

    gl_check_error();

    if (!GLEW_VERSION_3_2) {
        ogp_log_error("OpenGL 3.2 API is not available");
        return -1;
    }

    print_opengl_info();

    window_resize(settings.size.window.x, settings.size.window.y);

    gl_check_error();

    // AUDIO ...................................................................

    ogp_log_debug("Audio drivers:");
    for (i32 i = 0; i < SDL_GetNumAudioDrivers(); ++i) {
        const char *driver_name = SDL_GetAudioDriver(i);
        ogp_log_debug("    - %s", driver_name);
        if (SDL_AudioInit(driver_name)) {
            printf("Audio driver failed to initialize: %s\n", driver_name);
            continue;
        }
        SDL_AudioQuit();
    }

    // SUBSYSTEMS ..............................................................

    input_t input;
    input.init();

    physics_t physics;

    // SETUP ...................................................................

    glClearColor(0.1f, 0.2f, 0.4f, 1.0f);
    set_cursor("assets/textures/arrow.png");
    gl_setup_texture_filtering();

    player_t player {{0.5f, 0.5f, 2.0f}};
    camera_t camera_2d {{0.0f, 0.0f, 0.0f}};
    camera_t camera_3d {{0.5f, 0.5f, 2.0f}};
    camera_3d.set_projection_3d();
    camera_2d.set_projection_2d();
    renderer_t renderer;

    renderer.default_gl_setup();
    renderer.set_skybox("assets/skybox/skybox_0");

    message_bus_t message_bus;

    message_box_t message_box;
    message_box.subscribe(msg_type_e::QUIT);
    message_box.subscribe(msg_type_e::MOUSE_MOTION);
    message_box.subscribe(msg_type_e::MOUSE_BUTTON_DOWN);
    message_box.subscribe(msg_type_e::MOUSE_BUTTON_UP);
    message_box.subscribe(msg_type_e::WINDOW_FOCUS_GAINED);
    message_box.subscribe(msg_type_e::WINDOW_RESIZED);

    // DEBUG DRAW CONTEXTS .....................................................

    render_context_t rc_game;
    render_context_t rc_hud;

    bool o_debug_grid_3d    = false;
    bool o_debug_grid_2d    = false;
    bool o_debug_physics    = false;
    bool o_debug_cheatsheet = false;

    // GAME OBJECTS ............................................................

    cloth_t cloth {};
    physics.debug_print_stats();
    cloth.create(8, 8, 1.0f, 1.0f, 0.0f, &physics);

    box_t box {};
    box.create({0.5f, 2.0f, 0.5f}, &physics);

    pyramids_t pyramids;
    for (i32 i = -2; i < 3; ++i) {
        for (i32 j = -2; j < 3; ++j) {
            pyramids.add({1.75f * i, 0.0f, 1.75f * j});
        }
    }

    mesh_t cube = create_cube(0.5f);
    mesh_t teapot = load_mesh("assets/meshes/teapot.obj");

    physics.debug_print_stats();

    // MAINLOOP ................................................................

    bool quit = false;
    auto last_time = std::chrono::high_resolution_clock::now();
    f32 accumulator = 0.0f;

    while (!quit) {
        auto current_time = std::chrono::high_resolution_clock::now();
        auto frame_time = std::chrono::duration_cast<std::chrono::microseconds>(
                              current_time - last_time)
                              .count();
        last_time = current_time;
        accumulator += frame_time;

        // LOGIC ...............................................................

        i32 update_frames = 0;
        while (accumulator >= settings.delta.us) {
            accumulator -= settings.delta.us;
            if (update_frames >= 1) ogp_log_warning("update frames = %d", update_frames + 1);
            update_frames += 1;

            // poll sdl events
            input.poll_events();

            // keyboard
            update_keyboard();

            i32 mouse_xrel = 0;
            i32 mouse_yrel = 0;

            while (message_box.inbox.size() > 0) {
                message_t msg = message_box.receive();
                switch (msg.type) {

                    case msg_type_e::QUIT:
                    {
                        quit = true;
                        break;
                    }

                    case msg_type_e::WINDOW_RESIZED:
                    {
                        window_resize(settings.size.window.x, settings.size.window.y);
                        camera_3d.set_projection_3d();
                        camera_2d.set_projection_2d();
                        break;
                    }

                    case msg_type_e::MOUSE_MOTION:
                    {
                        mouse_motion_t const *data = msg_data_ptr<mouse_motion_t>(&msg);
                        mouse_xrel += data->rel.x;
                        mouse_yrel += data->rel.y;
                        break;
                    }

                    case msg_type_e::MOUSE_BUTTON_DOWN:
                    {
                        mouse_button_t const *data = msg_data_ptr<mouse_button_t>(&msg);
                        if (data->button == mouse_button_e::right) {
                            input.mousetrap(true);
                        }
                        break;
                    }

                    case msg_type_e::MOUSE_BUTTON_UP:
                    {
                        mouse_button_t const *data = msg_data_ptr<mouse_button_t>(&msg);
                        if (data->button == mouse_button_e::right) {
                            input.mousetrap(false);
                        }
                        break;
                    }

                }
            }

            // some framework logic
            if (key_just_pressed("Escape")) {
                quit = true;
            }

            if (key_just_pressed("F")) {
                if (settings.fullscreen) {
                    fullscreen(window, false);
                }
                else {
                    fullscreen(window, true);
                }
            }

            if (key_just_pressed("F1")) {
                o_debug_cheatsheet ^= true;
            }

            if (key_just_pressed("F2")) {
                settings.use_fbo ^= true;
                if (settings.use_fbo) ogp_log_info("FBO on");
                else ogp_log_error("FBO off");
            }

            if (key_just_pressed("`")) {
                // console.turn_on_off();
            }

            if (key_just_pressed("Y")) {
                renderer.resize();
            }

            if (key_just_pressed("2")) o_debug_grid_2d ^= true;
            if (key_just_pressed("3")) o_debug_grid_3d ^= true;
            if (key_just_pressed("4")) o_debug_physics ^= true;

            // some game logic
            if (input.is_mouse_trapped()) {

                player.mouse_movement(mouse_xrel, mouse_yrel);

                f32 const speed = 3.0f;
                f32 forward = 0.0f;
                f32 strafe = 0.0f;
                f32 dt = settings.delta.s;

                if (key_pressed("W")) forward = speed * dt;
                else if (key_pressed("S")) forward = -speed * dt;

                if (key_pressed("A")) strafe = -speed * dt;
                else if (key_pressed("D")) strafe = speed * dt;

                player.move(forward, strafe);
            }

            if (key_just_pressed("P")) {
                physics.debug_print_stats();
            }

            if (key_just_pressed("R")) {
                cloth.destroy(&physics);

                physics.debug_print_stats();

                cloth.create(8, 8, 1.0f, 1.0f, 0.0f, &physics);
            }

            pyramids.step(settings.delta.s);

            physics.step(settings.delta.s);

            vec3 ppos = player.get_position();
            quat player_orientation = player.get_orientation();
            camera_3d.move_to(ppos);
            camera_3d.step(player_orientation);

            // engine steps
            effects.step(settings.delta.s);
            message_bus.manage(&message_box);
            message_bus.manage(input.message_box_ptr());
            message_bus.step();
        }

        // DRAW ................................................................

        f32 frame_interpolation = accumulator / settings.delta.us;

        camera_3d.draw_step(frame_interpolation);

        rc_game.reset();
        rc_hud.reset();

        cloth.update_faces(&physics, frame_interpolation);
        box.update_faces(&physics, frame_interpolation);

        rc_game.set_fill_color(76, 70, 50);
        rc_game.draw_color_face({-10.0f, 0.0f, 10.0f}, {10.0f, 0.0f, 10.0f}, {-10.0f, 0.0f, -10.0f});
        rc_game.draw_color_face({-10.0f, 0.0f, -10.0f}, {10.0f, 0.0f, 10.0f}, {10.0f, 0.0f, -10.0f});

        cloth.draw(&rc_game);
        box.draw(&rc_game);
        // pyramids.draw(&rc_game, frame_interpolation);

        { // CUBES
            rc_game.draw_color_mesh(cube);

            static f32 rot_y = 0.0f;
            rot_y += 1.0f;
            transform_t transform;
            transform.translation = {-4.0f, 0.4f, 0.0f};
            transform.origin = transform.translation;
            transform.rotation = {rot_y, 0.0f, 0.0f};
            rc_game.draw_color_mesh(cube, transform);

            transform_t tr2;
            tr2.translation = {1.2f, 0.5f, 0.5f};
            rc_game.draw_color_mesh(cube, tr2);

        }

        { // TEAPOTS
            for (i32 i = 0; i < 5; ++i) {
                for (i32 j = 0; j < 5; ++j) {
                    static f32 rot_y = 0.0f;
                    rot_y += 0.05f;

                    transform_t transform;
                    transform.translation = {-2.0f + 1.0f * i, 0.0f, -2.0f + 1.0f * j};
                    transform.origin = transform.translation;
                    transform.rotation = {0.0f, rot_y, 0.0f};
                    transform.scale = {0.1f, 0.1f, 0.1f};
                    rc_game.draw_color_mesh(teapot, transform);
                }
            }
        }

        { // LIGHTS
            static float angle = 0.0f;
            angle += 0.5f;
            vec3 base_purple_pos {2.5f, 0.5f, 2.5f};
            vec3 base_yellow_pos {-2.5f, 0.5f, 2.5f};

            vec3 rot_axis = {0.0f, 1.0f, 0.0f};

            quat orient = glm::angleAxis(glm::radians(angle), rot_axis);

            vec3 purple_pos = orient * base_purple_pos;
            vec3 yellow_pos = orient * base_yellow_pos;

            rc_game.add_point_light({0.5f, 0.5f, 0.5f}, color_t {255, 255, 255});
            rc_game.add_point_light(yellow_pos, color_t {255, 255, 0});
            rc_game.add_point_light(purple_pos, color_t {255, 0, 255});
            rc_game.add_point_light(cloth.get_seventh_pos(&physics, frame_interpolation), color_t {0, 255, 0});

            // candle: 255, 147, 41
            rc_game.add_directional_light({1.0f, -1.0f, -1.0f}, color_t {16, 16, 16});
        }

        /*
         * static f32 pyramid_rot_y = 0.0f;
         * pyramid_rot_y += 2.0f * frame_interpolation;
         * rc_game.set_rotation({0.0f, pyramid_rot_y, 0.0f});
         * draw_pyramid(&rc_game);
         */

        // DEBUG DRAW OVERLAY ..................................................

        if (o_debug_grid_3d) debug_grid_3d(&rc_game);
        if (o_debug_grid_2d) debug_grid_2d(&rc_hud);

        if (o_debug_physics) physics.debug_draw(&rc_game, frame_interpolation);
        /*
         * if (console.enabled()) {
         *     console.draw(frame_interpolation);
         * }
         */

        /*
         * if (o_debug_cheatsheet) {
         *     // TODO draw debug_cheatsheet
         * }
         */

        // RENDER ..............................................................

        renderer.default_gl_setup();

        if (settings.use_fbo) {
            renderer.begin_frame();
        }
        else {
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        }

        renderer.render_skybox(&camera_3d);

        rc_game.update_vbo();
        rc_hud.update_vbo();

        rc_game.render(&camera_3d);
        rc_hud.render(&camera_2d);

        // renderer.render(&rc_game, &camera_3d);

        // DEBUG RENDER OVERLAY ................................................

        /*
         * if (console.enabled()) {
         *     renderer.render(&console.rc_game, camera_3d);
         * }
         */

        if (settings.use_fbo) {
            renderer.end_frame();
        }

        SDL_GL_SwapWindow(window);
    }

    SDL_GL_DeleteContext(glcontext);
    SDL_DestroyWindow(window);

    ogp_log_info("bye bye");
    SDL_Quit();
    ogp_log_info("bye bye after sdl");
    return 0;
}
