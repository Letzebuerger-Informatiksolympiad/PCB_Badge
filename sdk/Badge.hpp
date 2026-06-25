#pragma once

enum class Key {
    Up,
    Down,
    Left,
    Right
};

class Badge final {
public:
    Badge();

    Badge(const Badge&) = delete;
    Badge(Badge&&) = delete;
    Badge& operator=(const Badge&) = delete;
    Badge& operator=(Badge&&) = delete;

    void clearPixels();
    void setPixel(unsigned x, unsigned y, bool on = true);
    void updatePixels() const;

    bool isPressed(Key key) const;
private:
    bool _framebuffer[12][20];
};
