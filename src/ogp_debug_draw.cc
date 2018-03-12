#include "ogp_debug_draw.h"

#include "ogp_defines.h"
#include "ogp_render.h"
#include "ogp_settings.h"

namespace ogp
{

void debug_grid_3d(render_context_t *rc)
{
    // arrow params
    f32 aw = 0.075f;
    f32 aL = 0.75f;
    f32 y0 = 0.0025f;

    // 3D x axis
    rc->set_line_color(230, 26, 26, 255);
    rc->set_fill_color(230, 26, 26, 255);
    rc->draw_line({0.0f, y0, 0.0f}, {1.0f, y0, 0.0f});

    rc->draw_color_face({aL, 0.0f, -aw}, {aL, 0.0f, aw}, {1.0f, 0.0f, 0.0f});
    rc->draw_color_face({aL, 0.0f, -aw}, {1.0f, 0.0f, 0.0f}, {aL, 0.0f, aw});

    // 3D y axis
    rc->set_line_color(26, 230, 26, 255);
    rc->set_fill_color(26, 230, 26, 255);
    rc->draw_line({0.0f, y0, 0.0f}, {0.0f, 1.0f, 0.0f});
    rc->draw_color_face({0.0f, aL, aw}, {0.0f, aL, -aw}, {0.0f, 1.0f, 0.0f});
    rc->draw_color_face({0.0f, aL, aw}, {0.0f, 1.0f, 0.0f}, {0.0f, aL, -aw});

    // 3D z axis
    rc->set_line_color(26, 26, 230, 255);
    rc->set_fill_color(26, 26, 230, 255);
    rc->draw_line({0.0f, y0, 0.0f}, {0.0f, y0, 1.0f});
    rc->draw_color_face({aw, 0.0f, aL}, {-aw, 0.0f, aL}, {0.0f, 0.0f, 1.0f});
    rc->draw_color_face({aw, 0.0f, aL}, {0.0f, 0.0f, 1.0f}, {-aw, 0.0f, aL});

    // 3D grid
    rc->set_line_color(128, 128, 128, 32);
    i32 grid_len = 120;
    for (i32 i = 0; i < grid_len; ++i) {
        // f32 ratio = (f32)i / grid_len;
        auto x      = -((f32)grid_len / 2.0f) + 1.0f * i;
        rc->draw_line({x, y0, -60.0f}, {x, y0, 60.0f});
        rc->draw_line({-60.0f, y0, x}, {60.0f, y0, x});
    }

    // 3D center point
    rc->set_line_color(255, 255, 255, 255);
    rc->draw_point({0.0f, y0, 0.0f});
}
void debug_grid_2d(render_context_t *rc, vec2 tile)
{
    // 2D grid
    f32 gwidth2 = settings.size.game.x / 2.0f;
    f32 gheight2 = settings.size.game.y / 2.0f;

    i32 num_tiles_x = gwidth2 / tile.x;
    i32 num_tiles_y = gheight2 / tile.y;

    // rc->set_layer(layer_e::debug_0);

    // game rect
    rc->set_line_color(221, 206, 110, 128);
    rc->draw_line({-gwidth2, gheight2, 0.0f}, {gwidth2, gheight2, 0.0f});    // top
    rc->draw_line({-gwidth2, -gheight2, 0.0f}, {gwidth2, -gheight2, 0.0f});  // bottom
    rc->draw_line({-gwidth2, -gheight2, 0.0f}, {-gwidth2, gheight2, 0.0f});  // left
    rc->draw_line({gwidth2, -gheight2, 0.0f}, {gwidth2, gheight2, 0.0f});    // right

    // game grid
    f32 grid_begin_x = 0.0f - num_tiles_x * tile.x;
    f32 grid_begin_y = 0.0f - num_tiles_y * tile.y;

    // TODO make move independent
    rc->set_line_color(128, 128, 128, 64);
    for (i32 i = 0; i < num_tiles_x * 2 + 1; ++i) {
        f32 x = grid_begin_x + tile.x * i;
        rc->draw_line({x, -gheight2, 0.0f}, {x, gheight2, 0.0f});
    }

    for (i32 i = 0; i < num_tiles_y * 2 + 1; ++i) {
        f32 y = grid_begin_y + tile.y * i;
        rc->draw_line({-gwidth2, y, 0.0f}, {gwidth2, y, 0.0f});
    }

    // arrow params
    f32 aw = 0.05f;
    f32 aL = 0.75f;

    f32 z0 = 0.001f;

    // 2D center point
    rc->set_line_color(255, 255, 255, 128);
    rc->draw_point({0.0f, 0.0f, z0});

    // 2D x axis
    rc->set_line_color(230, 26, 26, 128);
    rc->set_fill_color(230, 26, 26, 128);
    rc->draw_line({0.0f, 0.0f, z0}, {1.0f, 0.0f, z0});
    rc->draw_color_face({aL, aw, z0 * 2}, {aL, -aw, z0}, {1.0f, 0.0f, z0 * 2});

    // 2D y axis
    rc->set_line_color(26, 230, 26, 128);
    rc->set_fill_color(26, 230, 26, 128);
    rc->draw_line({0.0f, 0.0f, z0}, {0.0f, 1.0f, z0});
    rc->draw_color_face({-aw, aL, z0 * 2}, {aw, aL, z0}, {0.0f, 1.0f, z0 * 2});
}

void debug_drawings(render_context_t *rc)
{
    static f32 x = 0.0f;
    x += 0.0001f;

    static f32 rot_y = 0.0f;
    rot_y += 2.0f;

    rc->set_line_color(255, 255, 0);
    rc->draw_point({-1.0f, 0.0f, 0.0f});
    rc->draw_point({x, 2.0f, 0.0f});
    rc->draw_point({x * 2, 2.1f, 0.0f});
    rc->draw_point({x * 3, 2.2f, 0.0f});
    rc->draw_point({x * 4, 2.4f, 0.0f});
    rc->draw_point({x * 5, 2.8f, 0.0f});

    rc->set_fill_color(40, 120, 80);
    rc->draw_color_face({0.0f, 0.0f, 0.5f}, {0.5f, 0.0f, 0.0f}, {0.0f, 0.5f, 0.0f});

    rc->set_line_color(200, 200, 200);

    vec3 prev_rotation = rc->set_rotation({rot_y, rot_y, rot_y});

    vec3 prev_translation = rc->set_translation({1.0f, 0.1f, 0.0f});
    vec3 prev_origin = rc->set_origin({1.5f, 0.6f, 0.5f});

    rc->draw_cube();

    rc->set_translation({3.0f, 0.1f, 0.0f});
    rc->set_origin({3.5f, 0.6f, 0.5f});
    rc->draw_cube();

    rc->set_translation({2.0f, 0.1f, 3.0f});
    rc->set_origin({2.5f, 0.6f, 3.5f});
    rc->draw_cube();

    rc->set_rotation(prev_rotation);
    rc->set_translation(prev_translation);
    rc->set_origin(prev_origin);
}

void debug_draw_points_grid(render_context_t *rc)
{
    i32 const N = 10;
    f32 const edge = 0.5f;

    for (i32 i = 0; i < N; ++i) {
        for (i32 j = 0; j < N; ++j) {
            for (i32 k = 0; k < N; ++k) {
                rc->draw_point({edge * i, edge * j, edge * k});
            }
        }
    }
}

}  // namespace ogp
