#pragma once
#include "MonoBehavior.h"
class Explosion :
    public MonoBehaviour
{
public:
    int frame = 0;
    static constexpr int FRAME_PER_SECOND = 30;


};

