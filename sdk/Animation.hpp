#pragma once

#include "Ticker.hpp"
#include "Badge.hpp"

class Animation {
public:
    Animation(unsigned frame_delay = 20);
    virtual ~Animation();

    void tick(Badge& badge);
    inline void reset() { _frame = 0; _ticker.reset(); }

    virtual void drawFrame(Badge& badge, unsigned frame) = 0;
private:
    Ticker _ticker;
    unsigned _frame;
};
