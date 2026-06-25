#include "Animation.hpp"


Animation::Animation(unsigned frame_delay) : _ticker(frame_delay), _frame(0) { }

Animation::~Animation() = default;

void Animation::tick(Badge& badge) {
    _ticker.tick();
    if(_ticker.risen()) {
        _frame += 1;
    }
    drawFrame(badge, _frame);
}
