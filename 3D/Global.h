#ifndef GLOBAL_H
#define GLOBAL_H
inline constexpr float globalTimeStep = 0.00833;
inline int iterations = 0;
inline float fixedTimer = 0.0f;
inline double deltaTime = 0.0f;

inline void FixedUpdate(float frameTime)
{
    fixedTimer += frameTime;
    while (fixedTimer > globalTimeStep)
    {
        iterations += 1;
        fixedTimer -= globalTimeStep;
    }
}

#endif