#pragma once

#include "Animation.hpp"

namespace {
    void draw_W(int x, int y, Badge& badge) {
        for(int i = 0; i < 5; ++i) {
            badge.setPixel(x-2, y-2 + i);
            badge.setPixel(x+2, y-2 + i);
        }
        badge.setPixel(x, y);
        badge.setPixel(x, y+1);
        badge.setPixel(x, y+2);
        badge.setPixel(x+1, y+2);
        badge.setPixel(x-1, y+2);
    }

    void draw_E(int x, int y, Badge& badge) {
        for(int i = 0; i < 3; ++i) {
            badge.setPixel(x-1+i, y-2);
            badge.setPixel(x-1+i, y);
            badge.setPixel(x-1+i, y+2);
        }
        badge.setPixel(x-1, y-1);
        badge.setPixel(x-1, y+1);
    }

    void draw_O(int x, int y, Badge& badge) {
        for(int i = 0; i < 4; ++i) {
            badge.setPixel(x-2+i, y-2);
            badge.setPixel(x-2+i, y+2);
        }
        for(int i = 0; i < 5; ++i) {
            badge.setPixel(x-2, y-2+i);
            badge.setPixel(x+1, y-2+i);
        }
    }

    void draw_I(int x, int y, Badge& badge) {
        for(int i = 0; i < 3; ++i) {
            badge.setPixel(x-1+i, y-2);
            badge.setPixel(x-1+i, y+2);
        }
        for(int i = 0; i < 3; ++i) {
            badge.setPixel(x, y-1+i);
        }
    }

    void draw_2(int x, int y, Badge& badge) {
        for(int i = 0; i < 5; ++i) {
            if(i % 2 == 0) {
                badge.setPixel(x-1, y-2+i);
                badge.setPixel(x, y-2+i);
                badge.setPixel(x+1, y-2+i);
            }
        }
        badge.setPixel(x-1, y+1);
        badge.setPixel(x+1, y-1);
    }

    void draw_0(int x, int y, Badge& badge) {
        for(int i = 0; i < 3; ++i) {
            badge.setPixel(x-2+i, y-2);
            badge.setPixel(x-2+i, y+2);
        }
        for(int i = 0; i < 5; ++i) {
            badge.setPixel(x-2, y-2+i);
            badge.setPixel(x, y-2+i);
        }
    }

    void draw_6(int x, int y, Badge& badge) {
        for(int i = 0; i < 5; ++i) {
            if(i % 2 == 0) {
                badge.setPixel(x-1, y-2+i);
                badge.setPixel(x, y-2+i);
                badge.setPixel(x+1, y-2+i);
            }
        }
        badge.setPixel(x+1, y+1);
        badge.setPixel(x-1, y-1);
        badge.setPixel(x-1, y+1);
    }
}

class AnimationWEOI : public Animation {
public:
    AnimationWEOI() : Animation(2) {}

    void drawFrame(Badge& badge, unsigned frame) override
    {
        if (frame < 12*20) {
            int progress = frame;

            int fullRows = progress / 20;
            int remainder = progress % 20;

            for (int y = 0; y < fullRows; ++y) {
                for (int x = 0; x < 20; ++x) {
                    badge.setPixel(x, y);
                }
            }

            if (fullRows < 12) {
                for (int x = 0; x < remainder; ++x) {
                    badge.setPixel(x, fullRows);
                }
            }
        }
        else {
            frame -= 12*20;

            draw_W(3, 3, badge);
            draw_E(8, 3, badge);
            draw_O(13, 3, badge);
            draw_I(17, 3, badge);

            draw_2(4, 9, badge);
            draw_0(9, 9, badge);
            draw_2(12, 9, badge);
            draw_6(16, 9, badge);
            if (frame < 12*20) {
                int progress = 12 * 20 - frame;

                int fullRows = progress / 20;
                int remainder = progress % 20;

                for (int y = 0; y < fullRows; ++y) {
                    for (int x = 0; x < 20; ++x) {
                        badge.setPixel(x, y);
                    }
                }

                if (fullRows < 12) {
                    for (int x = 0; x < remainder; ++x) {
                        badge.setPixel(x, fullRows);
                    }
                }
            }
        }
    }
};
