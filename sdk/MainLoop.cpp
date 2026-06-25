#include "MainLoop.hpp"


MainLoop::MainLoop(Badge& badge) : _badge(badge) {}
MainLoop::~MainLoop() = default;

void MainLoop::loop() {
    bool lastUp = _badge.isPressed(Key::Up);
    bool lastDown = _badge.isPressed(Key::Down);
    bool lastLeft = _badge.isPressed(Key::Left);
    bool lastRight = _badge.isPressed(Key::Right);

    while(!_exit) {
        bool up = _badge.isPressed(Key::Up);
        bool down = _badge.isPressed(Key::Down);
        bool left = _badge.isPressed(Key::Left);
        bool right = _badge.isPressed(Key::Right);

        FrameData data;
        data.buttons.up.down = up;
        data.buttons.up.pressed = !lastUp && up;
        data.buttons.up.released = lastUp && !up;
        lastUp = up;
        
        data.buttons.down.down = down;
        data.buttons.down.pressed = !lastDown && down;
        data.buttons.down.released = lastDown && !down;
        lastDown = down;
        
        data.buttons.left.down = left;
        data.buttons.left.pressed = !lastLeft && left;
        data.buttons.left.released = lastLeft && !left;
        lastLeft = left;
        
        data.buttons.right.down = right;
        data.buttons.right.pressed = !lastRight && right;
        data.buttons.right.released = lastRight && !right;
        lastRight = right; 

        _badge.clearPixels();
        nextFrame(_badge, data);
        _badge.updatePixels();
    }
}
