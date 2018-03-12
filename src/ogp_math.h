#ifndef OGP_MATH_H
#define OGP_MATH_H

#include "ogp_defines.h"

#define GLM_FORCE_RADIANS

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace ogp
{

f32 degrees(f32 rad);  // converts rad to deg

f32 radians(f32 deg);  // converts rad to deg

quat euler_to_quat(vec3 angles);

}  // namespace ogp

#endif  // OGP_MATH_H
