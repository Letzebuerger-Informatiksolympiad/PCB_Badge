#pragma once

#include "Badge.hpp"

struct ButtonData {
    bool pressed;
    bool released;
    bool down;
};

struct FrameData {
    struct {
        ButtonData up;
        ButtonData down;
        ButtonData left;
        ButtonData right;
    } buttons;
};

class MainLoop {
public:
    MainLoop(Badge& badge);
    virtual ~MainLoop();

    void loop();
    inline void exit() { _exit = true; }

    virtual void nextFrame(Badge& badge, const FrameData& data) = 0;
private:
    Badge& _badge;
    bool _exit = false;
};

