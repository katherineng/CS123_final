#include "camera.h"
#include <qgl.h>
#include "CS123Common.h"
void OrbitCamera::mouseMove(const Vector2 &delta)
{
    // Rotate the eye vector around the origin
    theta += delta.x * 0.01f;
   // phi += delta.y * 0.01;

    // Keep theta in [0, 2pi] and phi in [-pi/2, pi/2]
    theta -= floorf(theta / (2*M_PI)) * (2*M_PI);
    phi = 0;//MAX(0.01 - M_PI / 2, MIN(M_PI / 2 - 0.01, phi));

}

void OrbitCamera::mouseWheel(float delta)
{
    zoom *= powf(0.999f, delta);
}
