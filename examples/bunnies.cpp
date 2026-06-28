// example by Luca Courte
// Animated bunnies; the buttons switch the scene.

#include "sdk/Badge.hpp"
#include "sdk/MainLoop.hpp"
#include "sdk/AnimationWEOI.hpp"

struct BunnyData {
    int x;
    int y;
    bool ears_forward = false;
    bool flipped = false;
    bool head_forward = false;
};

void draw_bunny(Badge& badge, const BunnyData& data) {
    badge.setPixel(data.x - 2 + (data.head_forward ? (data.flipped ? -1 : 1) : 0), data.y);
    badge.setPixel(data.x - 2 + (data.head_forward ? (data.flipped ? -1 : 1) : 0), data.y - 1);
    badge.setPixel(data.x + 2 + (data.head_forward ? (data.flipped ? -1 : 1) : 0), data.y);
    badge.setPixel(data.x + 2 + (data.head_forward ? (data.flipped ? -1 : 1) : 0), data.y - 1);

    badge.setPixel(data.x - 1 + (data.head_forward ? (data.flipped ? -1 : 1) : 0), data.y + 1);
    badge.setPixel(data.x  + (data.head_forward ? (data.flipped ? -1 : 1) : 0), data.y + 1);
    badge.setPixel(data.x + 1 + (data.head_forward ? (data.flipped ? -1 : 1) : 0), data.y + 1);

    badge.setPixel(data.x - 1, data.y - 2);
    badge.setPixel(data.x, data.y - 2);
    badge.setPixel(data.x + 1, data.y - 2);

    badge.setPixel(data.x - 1  + (data.head_forward ? (data.flipped ? -1 : 1) : 0), data.y - 3);
    badge.setPixel(data.x + 1  + (data.head_forward ? (data.flipped ? -1 : 1) : 0), data.y - 3);
    badge.setPixel(data.x - 1 - (data.ears_forward ? (data.flipped ? -1 : 1) : 0) + (data.head_forward ? (data.flipped ? -1 : 1) : 0), data.y - 4);
    badge.setPixel(data.x + 1 - (data.ears_forward ? (data.flipped ? -1 : 1) : 0) + (data.head_forward ? (data.flipped ? -1 : 1) : 0), data.y - 4);

    badge.setPixel(data.x - 2, data.y + 2);
    badge.setPixel(data.x - 2, data.y + 3);
    badge.setPixel(data.x + 2, data.y + 2);
    badge.setPixel(data.x + 2, data.y + 3);

    badge.setPixel(data.x - 1, data.y + 4);
    badge.setPixel(data.x, data.y + 4);
    badge.setPixel(data.x + 1, data.y + 4);

    badge.setPixel(data.x - (data.flipped ? -3 : 3), data.y + 3 - (data.ears_forward ? 1 : 0));
}

class Bunny : public Animation {
public:
    Bunny() : Animation() {}

    void drawFrame(Badge& badge, unsigned frame) override
    {
        BunnyData data;
        data.x = 10;
        data.y = 16 - (frame >= 10 ? 10 : frame);
        data.ears_forward = frame >= 17 && (frame % 2);
        draw_bunny(badge, data);
        if(frame == 11) {
            badge.setPixel(8, 10);
        }
        if(frame == 12) {
            badge.setPixel(7, 11, 255);
        }
        if(frame == 13) {
            badge.setPixel(8, 10, 255);
            badge.setPixel(6, 11, 255);
        }
        if(frame == 14) {
            badge.setPixel(7, 11, 255);
            badge.setPixel(5, 11, 255);
        }
        if(frame == 15) {
            badge.setPixel(8, 10, 255);

            badge.setPixel(6, 11, 255);
            badge.setPixel(4, 11, 255);
        }
        if(frame >= 16) {
            badge.setPixel(7, 11, 255);
            badge.setPixel(5, 11, 255);
            badge.setPixel(3, 11, 255);
        }
    }
};

class BunnyDate : public Animation {
public:
    BunnyDate() : Animation() {}

    void drawFrame(Badge& badge, unsigned frame) override
    {
        BunnyData left_bunny;
        left_bunny.y = 5;

        BunnyData right_bunny;
        right_bunny.y = 5;
        right_bunny.flipped = true;

        if (frame <= 7) {
            left_bunny.x = -1 + frame;
            right_bunny.x = 20 - frame;
        }
        if(frame == 8) {
            left_bunny.x = 6;
            left_bunny.head_forward = true;
            right_bunny.x = 13;
            right_bunny.head_forward = true;
        }
        if(frame == 9) {
            left_bunny.x = 6;
            right_bunny.x = 13;
        }
        if(frame == 10) {
            left_bunny.x = 5;
            right_bunny.x = 14;
        }
        if(frame >= 11) {
            left_bunny.x = 5;
            right_bunny.x = 14;
            right_bunny.ears_forward = frame % 2;
            left_bunny.ears_forward = frame % 2;
        }
        draw_bunny(badge, left_bunny);
        draw_bunny(badge, right_bunny);
    }
};

class BunnyMainLoop : public MainLoop {
public:
    BunnyMainLoop(Badge& badge) : MainLoop(badge), _current_animation(&_weoi_animation) {}

    void nextFrame(Badge& badge, const FrameData& data) override
    {
        if(data.buttons.up.pressed) {
            _current_animation = &_bunny_animation;
            _current_animation->reset();
        }

        if(data.buttons.down.pressed) {
            _current_animation = &_date_animation;
            _current_animation->reset();
        }
        if(data.buttons.left.pressed || data.buttons.right.pressed) {
            _current_animation = &_weoi_animation;
            _current_animation->reset();
        }

        _current_animation->tick(badge);
    }
private:
    Animation* _current_animation;
    BunnyDate _date_animation;
    Bunny _bunny_animation;
    AnimationWEOI _weoi_animation;
};

int main() {
    Badge badge;

    BunnyMainLoop loop(badge);

    loop.loop();
}
