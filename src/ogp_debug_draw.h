#ifndef OGP_DEBUG_DRAW
#define OGP_DEBUG_DRAW

#include "ogp_defines.h"

namespace ogp
{

class render_context_t;

void debug_grid_3d(render_context_t *rc);

void debug_grid_2d(render_context_t *rc, vec2 tile = {1.0f, 1.0f});

void debug_drawings(render_context_t *rc);

void debug_draw_points_grid(render_context_t *rc);

}  // namespace ogp

#endif  // OGP_DEBUG_DRAW
