#include "ogp_math.h"

namespace ogp
{

quat euler_to_quat(vec3 angles)
{
    f32 x_rad = glm::radians(angles.x);
    f32 y_rad = glm::radians(angles.y);
    f32 z_rad = glm::radians(angles.z);
    return quat(vec3(x_rad, y_rad, z_rad));
}


}  // namespace ogp
