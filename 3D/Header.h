#pragma once


//THIS CODE IS LEFT OVER USED TO RESOLVE AN OBJECT WITH PHYSICS APPLIED (FORCE * DIRECTION VECTOR)
//DOES NOT ACCOUNT FOR ROTATIONAL FORCE
//IF TO FURTHER IMPLEMENT A RIGID BODY THIS WILL NEED ROTATIONAL FORCE AND AN EXTENSION OF GJK+EPA WHERE POINT OF CONTACT IS KNOWN
/*
void rigidBody()
{
    if (movingcube.pos.y < -200)
    {
        movingcube.setTransform(glm::vec3(0.0f, 5.0f, 0.0), glm::vec3(0.0));
        gravity = glm::vec3(0.0);
    }

    ResolutionData r = GJK(movingcube, cube, true);
    if (r.hasCollision)
    {
        gravity = glm::vec3(gravity.y * r.Normal.x, gravity.y, gravity.y * r.Normal.z);
        gravity.y = -gravity.y * 0.8f;


    }
    else
    {
        gravity.y -= (0.1);
        if (gravity.y < -300)
        {
            gravity.y = -300;
        }
    }
    GJK(movingcube, cameraCollider, true);

    mapRender(c, shaderPBRT, mySphere, movingcube.rot, movingcube.pos);
    mapRender(c, shaderPBRT, myPaddle, cube.rot, cube.pos);
}
*/