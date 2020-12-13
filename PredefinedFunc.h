#pragma once
#include <time.h>
#include "Physics.h"

void preciseDelay(double tD)
{
    clock_t start = clock();
    clock_t current;
    do
    {
        current = clock();
    } while (((double)(current - start)) / CLOCKS_PER_SEC < tD);
};

Angle normalize(Angle a)
{
    if (a.Pitch > 89)
    {
        a.Pitch = 89;
    }

    else if (a.Pitch < -89)
    {
        a.Pitch = -89;
    }

    else
    {

    }

    if (a.Yaw > 180)
    {
        a.Yaw -= 360;
    }

    else if (a.Yaw < -180)
    {
        a.Yaw += 360;
    }

    else
    {

    }

    return(a);
}

Angle calcAngle(vec3 me, vec3 ent)
{
    Angle tR;

    double delta[3] = { (me.x - ent.x), (me.y - ent.y), (me.z - ent.z) };
    double hyp = sqrt(delta[0] * delta[0] + delta[1] * delta[1]);
    tR.Pitch = (float)(asinf(delta[2] / hyp) * 57.295779513082f);
    tR.Yaw = (float)(atanf(delta[1] / delta[0]) * 57.295779513082f);
    tR.Roll = 0.0f;
    if (delta[0] >= 0.0)
    {
        tR.Yaw += 180.0f;
    }
    return(tR);
}

float magnitude(Vector v)
{
    return(sqrt((v.x * v.x) + (v.z * v.z) + (v.y * v.y)));
}