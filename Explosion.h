#pragma once
#include "MonoBehavior.h"
class Explosion :
    public MonoBehavior
{
public:
    int frame = 0;
    static constexpr int FRAME_PER_SECOND = 30;


};

