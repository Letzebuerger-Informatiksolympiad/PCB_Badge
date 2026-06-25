// example by Luca Courte
// The WEOI 2026 logo animation.

#include "sdk/Badge.hpp"
#include "sdk/MainLoop.hpp"
#include "sdk/AnimationWEOI.hpp"

class WEOIMainLoop : public MainLoop {
public:
    WEOIMainLoop(Badge& badge) : MainLoop(badge) {}

    void nextFrame(Badge& badge, const FrameData& data) override
    {
        if(data.buttons.up.pressed || data.buttons.down.pressed || data.buttons.left.pressed || data.buttons.right.pressed) {
            _off = !_off;
            _weoi_animation.reset();
        }
        if(!_off) {
            _weoi_animation.tick(badge);
        }
    }
private:
    bool _off = false;
    AnimationWEOI _weoi_animation;
};

int main() {
    Badge badge;

    WEOIMainLoop(badge).loop();
}
